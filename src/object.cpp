#include "../include/object.h"

void Object::addChild(Object* obj)
{
	// if the child already has a parent, abort
	if (obj->parent != nullptr)
	{
		std::cout << "warning: attempt to reparent a child who already has a parent" << std::endl;
		return;
	}

	// establish relationship
	obj->parent = this;
	children.pushBack(obj);
}

void Object::destroy()
{
	// mark the object as being ready to die
	is_waiting_for_death = true;
}

void Object::removeFromParent()
{
	if (parent == nullptr) return;

	// find ourselves in the parent's list of children
	size_t index = 0;
	for (index = 0; index < parent->children.getLength(); index++)
	{
		if (parent->children[index] == this) break;
	}

	// if we find ourself, remove us from the parent
	if (index < parent->children.getLength())
		parent->children.removeAt(index);
	// if we didn't, something has gone horribly, horribly wrong
	else
		std::cout << "warning: unable to find child in parent's children list" << std::endl;
	parent = nullptr;
}

void Object::performPhysicsUpdate(float delta_time)
{
	// velocities are both in local units. i didn't want to write an entire 4x4 matrix library for this project
	local_position += velocity_lin * delta_time;
	local_rotation += velocity_ang * delta_time;
}

ObjectType Object::getType()
{
	return ObjectType::EMPTY;
}

Object::Object(Vector3 position, Vector3 rotation, Vector3 scale)
{
	parent = nullptr;

	local_position = position;
	local_rotation = rotation;
	local_scale = scale;
}

Object::~Object()
{
	for (size_t i = 0; i < children.getLength(); i++)
	{
		if (children[i])
			delete children[i];
	}
	removeFromParent();
}

ObjectType MeshObject::getType()
{
	return ObjectType::MESH;
}

MeshObject::MeshObject(Mesh* _geometry, Vector3 position, Vector3 rotation, Vector3 scale)
{
	geometry = _geometry;
	
	parent = nullptr;
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
	
	parent = nullptr;
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
	enabled = true;

	parent = nullptr;
	local_position = position;
}

LightObject::LightObject()
{
	parent = nullptr;
}

void LightObject::operator=(LightObject&& other) noexcept
{
	type = other.type;
	diffuse_colour = other.diffuse_colour;
	specular_colour = other.specular_colour;
	ambient_colour = other.ambient_colour;
	direction = other.direction;
	local_position = other.local_position;
	enabled = other.enabled;

	parent = other.parent;
}

ObjectType TextObject::getType()
{
	return ObjectType::TEXT;
}

TextObject::TextObject(Vector2 _position, std::string _text, Vector3 _colour, void* _font)
{
	raster_position = _position;
	text = _text;
	colour = _colour;
	font = _font;
}

ObjectType ParticleObject::getType()
{
	return ObjectType::PARTICLE;
}

ParticleObject::ParticleObject(float _lifetime, Vector3 position, Vector3 rotation, Vector3 scale, Material* mat)
{
	lifetime = _lifetime;
	material = mat;

	parent = nullptr;
	local_position = position;
	local_rotation = rotation;
	local_scale = scale;
}
