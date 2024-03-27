#pragma once

#include "texture.h"
#include "vector3.h"

enum MaterialMode
{
	SOLID,
	ALBEDO,
	ALBEDO_NORMAL,
	NORMAL
};

class Material
{
private:
	MaterialMode mode;
public:
	Vector3 colour;
	Texture* albedo;
	Texture* normal;

	float shininess;

	Material(Vector3 solid_colour, float shininess = 0.3f, Texture* albedo_texture = NULL, Texture* normal_texture = NULL);
	
	MaterialMode getMode();

	~Material();
};