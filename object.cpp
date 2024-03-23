#include "object.h"

void Object::addChild(Object* obj, bool keep_world_transform)
{
	// if the child already has a parent, abort
	if (obj->parent != NULL) return;

	// establish relationship
	obj->parent = this;
	children.push_back(obj);

	// alter transform
	// TODO: apply inverse transform of this to child transform to compensate for new relationship
}

Object::Object(ObjectType type, Vector3 position, Vector3 rotation, Vector3 scale)
{
	object_type = type;

	geometry = NULL;
	parent = NULL;

	local_position = position;
	local_rotation = rotation;
	local_scale = scale;
}

