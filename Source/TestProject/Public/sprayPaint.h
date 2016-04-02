// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"


class TESTPROJECT_API sprayPaint : public AActor
{
public:
	sprayPaint();
	~sprayPaint();

	int getWidth() const { return texture_width; }
	int getHeight() const { return texture_height; }

	void setParams(int width, int height) { texture_width = width; texture_height = height; }

private:
	int texture_width = 0;
	int texture_height = 0;
};
