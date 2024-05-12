#pragma once

#include <string>
#include "vector3.h"

#pragma pack(push, 1)
struct BMPHeader
{
	uint8_t id_0 = 0;
	uint8_t id_1 = 0;

	uint32_t file_size = 0;

	uint16_t reserved_0 = 0;
	uint16_t reserved_1 = 0;

	uint32_t image_data_offset = 0;
};

enum BitmapInfoHeaderCompression
{
	BI_RGB = 0,
	BI_RLE8,
	BI_RLE4,
	BI_BITFIELDS,
	BI_JPEG,
	BI_PNG,
	BI_ALPHABITFIELDS,
	BI_CMYK = 11,
	BI_CMYKRLE8,
	BI_CMYKRLE4
};

struct BitmapInfoHeader
{
	uint32_t header_size = 40;

	uint32_t width_pixels;
	uint32_t height_pixels;

	uint16_t num_colour_planes;

	uint16_t num_bits_per_pixel;

	uint32_t compression_method;

	uint32_t image_size;

	uint32_t horizontal_resolution;
	uint32_t vertical_resolution;

	uint32_t num_palette_colours;

	uint32_t num_important_colours;
};
#pragma pack(pop)

class Texture
{
private:
	unsigned int texture_id;
	unsigned int texture_width, texture_height;

public:
	Texture();

	bool loadBMP(std::string path);
	static bool loadBMPRaw(std::string path, Vector3*& ptr);

	unsigned int getID();
	unsigned int getWidth();
	unsigned int getHeight();

	~Texture();
};