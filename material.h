#pragma once

#include "texture.h"
#include "vector3.h"

enum MaterialMode
{
	SOLID,
	ALBEDO,
};

class Material
{
private:
	MaterialMode mode;
public:
	Vector3 colour;
	Texture* albedo;

	float shininess;

	Material(Vector3 solid_colour, float reflectance = 0.3f, Texture* albedo_texture = NULL);
	
	MaterialMode getMode();

	~Material();
};