// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "sprayPaintActor.generated.h"

UCLASS()
class TESTPROJECT_API AsprayPaintActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AsprayPaintActor();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UFUNCTION(BlueprintCallable, Category = MyCategory)
	int32 getHeight() const { return height; }

	UFUNCTION(BlueprintCallable, Category = MyCategory)
	int32 getWidth() const { return width; }

	UFUNCTION(BlueprintCallable, Category = MyCategory)
	void setDims(int32 texture_width, int32 texture_height) { width = texture_width; height = texture_height; }

	UPROPERTY(BlueprintReadwrite, Category = MyCategory)
	int32 width = 0;

	UPROPERTY(BlueprintReadwrite, Category = MyCategory)
	int32 height = 0;
};
