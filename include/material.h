#pragma once

#include "texture.h"
#include "vector3.h"

// enum describing available material render modes
enum MaterialMode
{
	SOLID,
	ALBEDO
};

// class storing material configuration
class Material
{
private:
	MaterialMode mode;
public:
	Vector3 colour;
	Texture* albedo;

	float shininess;
	bool is_unlit = false;

	Material(Vector3 solid_colour, float reflectance = 0.3f, Texture* albedo_texture = NULL);
	
	MaterialMode getMode();

	~Material();
};