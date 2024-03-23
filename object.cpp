#include "object.h"

void object::add_child(object* obj, bool keep_world_transform)
{
	// if the child already has a parent, abort
	if (obj->parent != NULL) return;

	// establish relationship
	obj->parent = this;
	children.push_back(obj);

	// alter transform
	// TODO: apply inverse transform of this to child transform to compensate for new relationship
}

object::object(mesh* geom, vector3 position, vector3 rotation, vector3 scale)
{
	geometry = geom;
	parent = NULL;

	local_position = position;
	local_rotation = rotation;
	local_scale = scale;
}

