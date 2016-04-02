// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "spraypaint_utilities.generated.h"

/**
*
*/
UCLASS(BlueprintType)
class TESTPROJECT_API Uspraypaint_utilities : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	Uspraypaint_utilities();
	~Uspraypaint_utilities();

	UFUNCTION(BlueprintCallable, Category = "Spraypaint Utils")
	UTexture2D* getDiffuseMap() const { return diffuse_texture; }

	UFUNCTION(BlueprintCallable, Category = "Spraypaint Utils")
	UTexture2D* getSpecularMap() const { return specular_texture; }

	UFUNCTION(BlueprintCallable, Category = "Spraypaint Utils")
	UTexture2D* getGlowMask() const { return glow_mask_texture; }

	UFUNCTION(BlueprintCallable, Category = "Spraypaint Utils")
	UTexture2D* getGlowRGBA() const { return glow_rgba_texture; }

	UFUNCTION(BlueprintCallable, Category = "Spraypaint Utils")
	UTexture2D* getNormalMap() const { return normal_texture; }

	UFUNCTION(BlueprintCallable, Category = "Spraypaint Utils")
	bool createTexture(const int32 &width, const int32 &height);

	UFUNCTION(BlueprintCallable, Category = "Spraypaint Utils")
	bool textureInitialized() const { return 0 != texture_width; }

	// assuming mouse position is in floats, top-left being 0.0, 0.0, converted to ints in-engine using Floor
	UFUNCTION(BlueprintCallable, Category = "Spraypaint Utils")
	bool updateTexture(
		const FColor &diffuse_color, 				// brush color
		const FColor &glow_color,					// emissive color
		const float &specular_coefficient, 			// overall specular quality
		const float &sparkle_density, 				// controls frequency of specular spikes
		const float &sparkle_range,					// controls max possible value of specular spikes
		const float &glow_intensity,				// controls 
		const float &u_pos, 						// u position on texture
		const float &v_pos, 						// v position on texture
		const float &brush_radius, 					// brush size
		const float &spray_falloff);				// 0 = solid circle, 1 = dense middle and faded edges

	// Diffuse Texture Info
	uint8* diffuse_mip;
	UTexture2D* diffuse_texture;

	// Specular Map Texture Info
	uint8* specular_mip;
	UTexture2D* specular_texture;

	// Glow Map Texture Info
	uint8* glow_mask_mip;
	UTexture2D* glow_mask_texture;

	// Glow Color Info, separate from diffuse to avoid separate glow marks for each stray pixel
	uint8* glow_rgba_mip;
	UTexture2D* glow_rgba_texture;

	// Normal Map Texture Info
	uint8* normal_mip;
	UTexture2D* normal_texture;

	// within the pair, element.Key = width, element.Value = height
	int32 texture_width = 0;
	int32 texture_height = 0;

	// TODO make below methods private
	void updatePixel(
		const int32 &index,
		const float &distance_coefficient,
		const FColor &diffuse_color,
		const FColor &glow_color,
		const float &specular_coefficient,
		const float &sparkle_density,
		const float &sparkle_range,
		const float &glow_intensity,
		const float &spray_falloff);

	void prepareUTexture2D(
		const int32 &width,
		const int32 &height,
		UTexture2D* &texture,
		uint8* &mip);
};