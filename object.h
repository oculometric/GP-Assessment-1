#pragma once

#include "vector3.h"
#include "mesh.h"
#include <vector>

class Object
{
public:
	// TODO: improve this by caching the matrix (requires matrix4)
	Vector3 local_position;
	Vector3 local_rotation;
	Vector3 local_scale;

	Mesh* geometry;

	std::vector<Object*> children;
	Object* parent;

	void generateMatrix();
	void addChild(Object* obj, bool keep_world_transform);
	void removeFromParent(bool keep_world_transform);

	Object(Mesh* geom = NULL, Vector3 position = { 0,0,0 }, Vector3 rotation = { 0,0,0 }, Vector3 scale = {1,1,1});

	// TODO: different object types
};