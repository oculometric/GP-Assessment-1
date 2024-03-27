#include "material.h"

Material::Material(Vector3 solid_colour, float reflectance, Texture* albedo_texture)
{
    colour = solid_colour;
    shininess = reflectance;
    mode = MaterialMode::SOLID;

    albedo = albedo_texture;
    if (albedo_texture)
        mode = MaterialMode::ALBEDO;
}

MaterialMode Material::getMode()
{
    return mode;
}

Material::~Material()
{ }
