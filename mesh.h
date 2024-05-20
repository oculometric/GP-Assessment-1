#pragma once

#include <stdint.h>
#include "vector3.h"
#include "vector2.h"
#include "material.h"

class Mesh
{
public:
	Vector3* vertices;			// one per vertex
	uint32_t* triangles;		// three per triangle
	Vector2* uvs;				// three per triangle
	Vector3* vertex_normals;	// three per triangle
	Vector3 bounds_min{ 0,0,0 };
	Vector3 bounds_max{ 0,0,0 };

	Material* material = NULL;

private:
	uint32_t num_verts = 0;		// length of the vertices array
	uint32_t num_tris = 0;		// length of the triangles array

public:
	// return the length of the vertices array
	uint32_t vertsCount();
	// return the length of the tris array
	uint32_t trisCount();

	// create a mesh with sized capacity
	Mesh(uint32_t verts_capacity, uint32_t tris_capacity);
	// create a mesh based on an .obj file
	Mesh(std::string path);

	~Mesh();
};