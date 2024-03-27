#include "texture.h"

Texture::Texture()
{
	texture_id = -1;
	texture_width = 0;
	texture_height = 0;
}

bool Texture::loadBMP(std::string path)
{
	return false;
}

unsigned int Texture::getID()
{
	return texture_id;
}

unsigned int Texture::getWidth()
{
	return texture_width;
}

unsigned int Texture::getHeight()
{
	return texture_height;
}

Texture::~Texture()
{ }
