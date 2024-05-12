#include "texture.h"
#include "iostream"
#include "fstream"

#include <Windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>

Texture::Texture()
{
	texture_id = -1;
	texture_width = 0;
	texture_height = 0;
}

bool Texture::loadBMP(std::string path)
{
	BMPHeader header;
	
	std::ifstream file;
	file.open(path, std::ios::binary);
	if (!file.is_open()) return false;

	file.seekg(file.beg);
	file.read((char*)(&header), sizeof(header));
	if (header.id_0 != 'B' || header.id_1 != 'M') { file.close();  return false; }

	uint32_t dib_header_size;
	file.read((char*)(&dib_header_size), 4);
	if (dib_header_size != 40) { file.close(); return false; }

	BitmapInfoHeader bitmap_header;
	file.read((char*)(&bitmap_header) + 4, sizeof(bitmap_header) - 4);

	if (bitmap_header.compression_method != BI_RGB) { file.close(); return false; }
	if (bitmap_header.num_colour_planes != 1) { file.close(); return false; }
	if (bitmap_header.num_bits_per_pixel < 24) { file.close(); return false; }

	bitmap_header.image_size = bitmap_header.width_pixels * bitmap_header.height_pixels * bitmap_header.num_bits_per_pixel / 8;
	uint8_t* pixel_data = new uint8_t[bitmap_header.image_size];
	file.seekg(file.beg + header.image_data_offset);
	file.read((char*)pixel_data, bitmap_header.image_size);

	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_2D, texture_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, bitmap_header.width_pixels, bitmap_header.height_pixels, 0, bitmap_header.num_bits_per_pixel == 24 ? GL_BGR_EXT : GL_BGRA_EXT, GL_UNSIGNED_BYTE, pixel_data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	file.close();

	std::cout << "loaded texture " << path << std::endl;
	std::cout << "    width: " << bitmap_header.width_pixels << std::endl;
	std::cout << "    height: " << bitmap_header.height_pixels << std::endl;
	std::cout << "    bits per pixel: " << bitmap_header.num_bits_per_pixel << std::endl;
	std::cout << "    has ID: " << texture_id << std::endl;

	return true;
}

bool Texture::loadBMPRaw(std::string path, Vector3*& ptr)
{
	BMPHeader header;

	std::ifstream file;
	file.open(path, std::ios::binary);
	if (!file.is_open()) return false;

	file.seekg(file.beg);
	file.read((char*)(&header), sizeof(header));
	if (header.id_0 != 'B' || header.id_1 != 'M') { file.close();  return false; }

	uint32_t dib_header_size;
	file.read((char*)(&dib_header_size), 4);
	if (dib_header_size != 40) { file.close(); return false; }

	BitmapInfoHeader bitmap_header;
	file.read((char*)(&bitmap_header) + 4, sizeof(bitmap_header) - 4);

	if (bitmap_header.compression_method != BI_RGB) { file.close(); return false; }
	if (bitmap_header.num_colour_planes != 1) { file.close(); return false; }
	if (bitmap_header.num_bits_per_pixel < 24) { file.close(); return false; }

	bitmap_header.image_size = bitmap_header.width_pixels * bitmap_header.height_pixels * bitmap_header.num_bits_per_pixel / 8;
	uint8_t* pixel_data = new uint8_t[bitmap_header.image_size];
	file.seekg(file.beg + header.image_data_offset);
	file.read((char*)pixel_data, bitmap_header.image_size);

	int bytes_per_pixel = bitmap_header.num_bits_per_pixel / 8;
	ptr = new Vector3[bitmap_header.width_pixels * bitmap_header.height_pixels];
	for (int i = 0; i < bitmap_header.width_pixels * bitmap_header.height_pixels; i++)
	{
		Vector3 pixel;
		pixel.z = (pixel_data[(i * bytes_per_pixel) + 0] / 255.0f);	// B
		pixel.y = (pixel_data[(i * bytes_per_pixel) + 1] / 255.0f);	// G
		pixel.x = (pixel_data[(i * bytes_per_pixel) + 2] / 255.0f);	// R
		// why the fuck does BMP store the buffer upside down??
		ptr[(i % bitmap_header.width_pixels) + ((bitmap_header.height_pixels - (i / bitmap_header.width_pixels) - 1) * bitmap_header.width_pixels)] = pixel;
	}

	file.close();

	std::cout << "loaded texture raw " << path << std::endl;
	std::cout << "    width: " << bitmap_header.width_pixels << std::endl;
	std::cout << "    height: " << bitmap_header.height_pixels << std::endl;
	std::cout << "    bits per pixel: " << bitmap_header.num_bits_per_pixel << std::endl;

	return true;
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
