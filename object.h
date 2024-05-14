#pragma once

#include "vector3.h"
#include "mesh.h"
#include <vector>
#include <string>
#include "linked_list.h"

enum ObjectType
{
	EMPTY,
	MESH,
	CAMERA,
	TEXT,
	LIGHT,
	PARTICLE
};

class Object
{
public:
	// TODO: improve this by caching the matrix (requires matrix4)
	Vector3 local_position;
	Vector3 local_rotation;
	Vector3 local_scale;

	// linear and angular velocity in local space
	Vector3 velocity_lin = { 0,0,0 };
	Vector3 velocity_ang = { 0,0,0 };

	LinkedList<Object*> children;
	Object* parent = NULL;

	std::string name = "object";

	void addChild(Object* obj, bool keep_world_transform);
	void destroy();
	void removeFromParent(bool keep_world_transform);

	void performPhysicsUpdate(float delta_time);

	virtual ObjectType getType();

	Object(Vector3 position = { 0,0,0 }, Vector3 rotation = { 0,0,0 }, Vector3 scale = {1,1,1});
};

class MeshObject : public Object
{
public:
	Mesh* geometry = NULL;

	ObjectType getType() override;

	MeshObject(Mesh* _geometry = NULL, Vector3 position = { 0,0,0 }, Vector3 rotation = { 0,0,0 }, Vector3 scale = { 1,1,1 });
};

class CameraObject : public Object
{
public:
	float aspect_ratio = 1.0f;
	float fov_degrees = 90.0f;
	float near_clip = 0.01f;
	float far_clip = 10.0f;

	ObjectType getType() override;

	CameraObject(float _fov_degrees, float near, float far, float aspect = 1.0f, Vector3 position = { 0,0,0 }, Vector3 rotation = { 0,0,0 }, Vector3 scale = { 1,1,1 });
};

enum LightType
{
	DIRECTIONAL,	// sun lamp
	POSITIONAL		// spot lamp
};

class LightObject : public Object
{
public:
	Vector3 diffuse_colour = Vector3{ 0.0f, 0.0f, 1.0f };
	Vector3 specular_colour = Vector3{ 1.0f, 1.0f, 1.0f };
	Vector3 ambient_colour = Vector3{ 1.0f, 1.0f, 1.0f };
	Vector3 direction = Vector3{ 0.0f, 0.0f, 1.0f };
	LightType type = LightType::DIRECTIONAL;
	float spot_exponent = 0.0f;
	float spot_angle = 30.0f;

	float attenuation_constant = 1.0f;
	float attenuation_linear = 0.0f;
	float attenuation_quadratic = 0.0f;

	bool enabled = false;

	ObjectType getType() override;

	LightObject(LightType _type, Vector3 colour, Vector3 position = { 0,0,0 }, Vector3 _direction = { 0,0,0 });
	LightObject();
};

class TextObject : public Object
{
public:
	Vector2 raster_position = Vector2{ 0.0f, 0.0f };
	std::string text = "text";
	Vector3 colour = Vector3{ 1.0f, 1.0f, 1.0f };
	void* font;

	ObjectType getType() override;

	TextObject(Vector2 _position, std::string _text, Vector3 _colour, void* _font);
};

class ParticleObject : public Object
{
public:
	float lifetime;
	float time_alive = 0;

	Material* material = NULL;

	ObjectType getType() override;

	ParticleObject(float _lifetime, Vector3 position = { 0,0,0 }, Vector3 rotation = { 0,0,0 }, Vector3 scale = { 0,0,0 }, Material* mat = NULL);
};