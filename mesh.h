#pragma once

#include <stdint.h>
#include "vector3.h"

struct mesh
{
	vector3* vertices;
	uint32_t* triangles;

	uint32_t num_verts = 0;
	uint32_t num_tris = 0;
};