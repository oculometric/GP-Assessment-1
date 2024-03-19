#pragma once

#include <stdint.h>
#include "vector3.h"
#include "vector2.h"

class mesh
{
public:
	vector3* vertices;			// one per vertex
	uint32_t* triangles;		// three per triangle
	vector2* uvs;				// three per triangle
	vector3* vertex_normals;	// three per triangle

private:
	uint32_t num_verts = 0;		// length of the vertices array
	uint32_t num_tris = 0;		// length of the triangles array

public:
	// return the length of the vertices array
	uint32_t verts_count();
	// return the length of the tris array
	uint32_t tris_count();

	// create a mesh with sized capacity
	mesh(uint32_t verts_capacity, uint32_t tris_capacity);
	// create a mesh based on an .obj file
	mesh(std::string path);

	~mesh();
};