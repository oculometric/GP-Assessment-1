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

void Object::removeFromParent(bool keep_world_transform)
{
	if (parent == NULL) return;

	int index = 0;
	for (Object* obj : parent->children)
	{
		if (obj == this) break;
		index++;
	}

	if (index >= parent->children.size())
	{
		parent = NULL;
		return;
	}

	std::swap(parent->children[index], parent->children.back());
	parent->children.pop_back();
	parent = NULL;
}

void Object::performPhysicsUpdate(float delta_time)
{
	local_position += velocity_lin * delta_time;
	local_rotation += velocity_ang * delta_time;
}

ObjectType Object::getType()
{
	return ObjectType::EMPTY;
}

Object::Object(Vector3 position, Vector3 rotation, Vector3 scale)
{
	parent = NULL;

	local_position = position;
	local_rotation = rotation;
	local_scale = scale;
}

ObjectType MeshObject::getType()
{
	return ObjectType::MESH;
}

MeshObject::MeshObject(Mesh* _geometry, Vector3 position, Vector3 rotation, Vector3 scale)
{
	geometry = _geometry;
	
	parent = NULL;
	local_position = position;
	local_rotation = rotation;
	local_scale = scale;
}

ObjectType CameraObject::getType()
{
	return ObjectType::CAMERA;
}

CameraObject::CameraObject(float _fov_degrees, float near, float far, float aspect, Vector3 position, Vector3 rotation, Vector3 scale)
{
	fov_degrees = _fov_degrees;
	near_clip = near;
	far_clip = far;
	aspect_ratio = aspect;
	
	parent = NULL;
	local_position = position;
	local_rotation = rotation;
	local_scale = scale;
}

ObjectType LightObject::getType()
{
	return ObjectType::LIGHT;
}

LightObject::LightObject(LightType _type, Vector3 colour, Vector3 position, Vector3 _direction)
{
	type = _type;
	diffuse_colour = colour;
	specular_colour = colour;
	ambient_colour = colour;
	direction = _direction;
	local_position = position;
	enabled = true;

	parent = NULL;
	local_position = position;
}

LightObject::LightObject()
{
	parent = NULL;
}
