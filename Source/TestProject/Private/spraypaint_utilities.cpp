// Fill out your copyright notice in the Description page of Project Settings.

#include "TestProject.h"
#include "spraypaint_utilities.h"

// increases a value by a given percentage of difference to an upper bound
template<typename T>
void increaseByPercentageRemaining(T &value_to_modify, const T &max_value, const float &percentage_fill)
{
	if (max_value <= value_to_modify)
		return;

	T cast_max(max_value);

	T difference = cast_max - value_to_modify;
	T amount_to_add = percentage_fill * difference;
	value_to_modify += T(amount_to_add);
}

int32 getPixelIndexConversion(const int32 &first_index, const int32 &first_width, const int32 &larger_height, const int32 &second_width, const int32 &second_height)
{
	int32 first_row_index = first_index / first_width;
	int32 second_row_index = first_row_index % second_height;

	int32 first_column_index = first_index % first_width;
	int32 second_column_index = first_column_index % second_width;

	return second_row_index * second_width + second_column_index;
}

Uspraypaint_utilities::Uspraypaint_utilities()
{

}

Uspraypaint_utilities::~Uspraypaint_utilities()
{

}

void Uspraypaint_utilities::prepareUTexture2D(
	const int32 &width,
	const int32 &height,
	UTexture2D* &texture,
	uint8* &mip)
{
	texture = UTexture2D::CreateTransient(width, height, PF_B8G8R8A8);
	mip = (uint8*)texture->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
	texture->PlatformData->Mips[0].BulkData.Unlock();
	texture->PlatformData->NumSlices = 1;
	texture->NeverStream = true;
}

bool Uspraypaint_utilities::createTexture(const int32 &width, const int32 &height)
{
	prepareUTexture2D(width, height, diffuse_texture, diffuse_mip);
	prepareUTexture2D(width, height, specular_texture, specular_mip);
	prepareUTexture2D(width, height, glow_mask_texture, glow_mask_mip);
	prepareUTexture2D(width, height, glow_rgba_texture, glow_rgba_mip);
	prepareUTexture2D(width, height, normal_texture, normal_mip);

	int32 index_count = width * height * 4;
	for (int32 i = 0; i < index_count; i++)
	{
		// alpha channel
		if (i % 4 == 3)
		{
			diffuse_mip[i] = (uint8)0;
			specular_mip[i] = (uint8)255;
			glow_mask_mip[i] = (uint8)255;
			glow_rgba_mip[i] = (uint8)255;
			normal_mip[i] = (uint8)0;
		}

		else
		{
			diffuse_mip[i] = (uint8)128;
			specular_mip[i] = (uint8)128;
			glow_mask_mip[i] = (uint8)0;
			glow_rgba_mip[i] = (uint8)0;

			// blue channel
			if (i % 4 == 0)
				normal_mip[i] = (uint8)255;

			// red and green channels
			else normal_mip[i] = (uint8)128;
		}
	}

	diffuse_texture->UpdateResource();
	specular_texture->UpdateResource();
	glow_mask_texture->UpdateResource();
	glow_rgba_texture->UpdateResource();
	normal_texture->UpdateResource();

	texture_width = width;
	texture_height = height;

	return true;
}

void Uspraypaint_utilities::updatePixel(
	const int32 &index,
	const float &distance_coefficient,
	const FColor &diffuse_color,
	const FColor &glow_color,
	const float &specular_coefficient,
	const float &sparkle_density,
	const float &sparkle_range,
	const float &glow_intensity,
	const float &spray_falloff)
{
	float inverse_distance = 1 - distance_coefficient;

	//experiment with exponentiating inverse distance
	float opacity_proc = inverse_distance * .6;

	if (FMath::FRand() > opacity_proc)
		return;

	int32 b_index = index * 4;
	int32 g_index = b_index + 1;
	int32 r_index = b_index + 2;
	int32 a_index = b_index + 3;

	uint8 glow_value = (uint8)FMath::Clamp(int(glow_intensity * 255), 0, 255);

	glow_mask_mip[r_index] = glow_value;
	glow_mask_mip[g_index] = glow_value;
	glow_mask_mip[b_index] = glow_value;

	glow_rgba_mip[r_index] = (uint8)glow_color.R;
	glow_rgba_mip[g_index] = (uint8)glow_color.G;
	glow_rgba_mip[b_index] = (uint8)glow_color.B;

	int8 global_specular = (int8)FMath::Clamp(int(255 * specular_coefficient), 0, 255);

	if (FMath::FRand() < sparkle_density * .5)
	{
		specular_mip[r_index] = (uint8)255;
		specular_mip[g_index] = (uint8)255;
		specular_mip[b_index] = (uint8)255;

		//normals currently not affecting surface as expected (decal behavior)
		//randomize normal to create shimmering effect
		/*uint8 normal_r = 128;
		uint8 normal_g = 128;
		uint8 normal_b = 255;

		increaseByPercentageRemaining<uint8>(normal_r, 200, FMath::FRand());
		increaseByPercentageRemaining<uint8>(normal_g, 200, FMath::FRand());

		FVector normal_direction((float)normal_r/255.0f, (float)normal_g/255.0f, (float)normal_b/255.0f);
		normal_direction.Normalize(0.01f);

		normal_mip[r_index] = FMath::Clamp((uint8)(normal_direction.X * 255), (uint8)128, (uint8)255);
		normal_mip[g_index] = FMath::Clamp((uint8)(normal_direction.Y * 255), (uint8)128, (uint8)255);
		normal_mip[b_index] = FMath::Clamp((uint8)(normal_direction.Z * 255), (uint8)128, (uint8)255);
		normal_mip[a_index] = (uint8)255;*/
	}

	else
	{
		specular_mip[r_index] = global_specular;
		specular_mip[g_index] = global_specular;
		specular_mip[b_index] = global_specular;

		normal_mip[a_index] = (uint8)0;
	}

	diffuse_mip[r_index] = (uint8)diffuse_color.R;
	diffuse_mip[g_index] = (uint8)diffuse_color.G;
	diffuse_mip[b_index] = (uint8)diffuse_color.B;
	diffuse_mip[a_index] = (uint8)255;
}

// assuming mouse position is in floats, top-left being 0.0, 0.0, converted to ints in-engine using Floor
bool Uspraypaint_utilities::updateTexture(
	const FColor &diffuse_color, 				// brush color
	const FColor &glow_color,					// emissive color
	const float &specular_coefficient, 			// overall specular quality
	const float &sparkle_density, 				// controls frequency of specular spikes
	const float &sparkle_range,					// controls max possible value of specular spikes
	const float &glow_intensity,				// controls 
	const float &u_pos, 						// u position of center
	const float &v_pos, 						// v position of center
	const float &brush_radius, 					// brush size
	const float &spray_falloff)					// 0 = solid circle, 1 = dense middle and faded edges
{
	// else division by zero exception thrown
	if (brush_radius < 1)
		return false;

	int32 y_pos = v_pos * texture_height;
	int32 x_pos = u_pos * texture_width;

	int32 center_pixel_index = y_pos * texture_width + x_pos;

	TMap<int32, float> pixel_data_array;

	//generate an array of pixel indices from the center of the mouse click
	for (int32 i = 0; i < (brush_radius * brush_radius * 4); i++)
	{
		int32 brush_row_index = i / (brush_radius * 2);
		int32 array_row_index = y_pos - brush_radius + brush_row_index;
		if (array_row_index < 0 || array_row_index >= texture_height)
			continue;

		int32 brush_column_index = i % ((int32)brush_radius * 2);
		int32 array_column_index = x_pos - brush_radius + brush_column_index;
		if (array_column_index < 0 || array_column_index >= texture_width)
			continue;

		FVector offset_vector(array_column_index - x_pos, array_row_index - y_pos, 0.0);
		float offset_vector_length = offset_vector.Size();

		if (offset_vector_length > brush_radius)
			continue;

		int32 pixel_index = array_row_index * texture_width + array_column_index;

		// adds vector length compared to size of brush to convert to value between 0 and 1
		pixel_data_array.Add(pixel_index, offset_vector_length / brush_radius);
	}

	int32 pixel_count = texture_width * texture_height;

	for (const auto& pixel_data : pixel_data_array)
	{
		int32 pixel_index = pixel_data.Key;
		if (pixel_index < pixel_count && pixel_index >= 0)
		{
			updatePixel(
				pixel_index,
				pixel_data.Value,
				diffuse_color,
				glow_color,
				specular_coefficient,
				sparkle_density,
				sparkle_range,
				glow_intensity,
				spray_falloff);
		}
	}

	diffuse_texture->UpdateResource();
	specular_texture->UpdateResource();
	glow_mask_texture->UpdateResource();
	glow_rgba_texture->UpdateResource();
	//normal_texture->UpdateResource();

	return true;
}


