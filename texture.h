#pragma once

#include <string>

class Texture
{
private:
	unsigned int texture_id;
	unsigned int texture_width, texture_height;

public:
	Texture();

	bool loadPNG(std::string path);

	unsigned int getID();
	unsigned int getWidth();
	unsigned int getHeight();

	~Texture();
};