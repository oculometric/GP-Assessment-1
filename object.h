#pragma once

#include "vector3.h"
#include "mesh.h"
#include <vector>

class object
{
public:
	// TODO: improve this by caching the matrix (requires matrix4)
	vector3 local_position;
	vector3 local_rotation;
	vector3 local_scale;

	mesh* geometry;

	std::vector<object*> children;
	object* parent;

	void generate_matrix();
	void add_child(object* obj, bool keep_world_transform);
	void remove_from_parent(bool keep_world_transform);

	object(mesh* geom = NULL, vector3 position = { 0,0,0 }, vector3 rotation = { 0,0,0 }, vector3 scale = {1,1,1});

	// TODO: different object types
};