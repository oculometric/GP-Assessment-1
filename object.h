#pragma once

#include "vector3.h"
#include "mesh.h"
#include <vector>

enum ObjectType
{
	EMPTY,
	MESH,
	CAMERA
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

	std::vector<Object*> children;
	Object* parent = NULL;

	std::string name = "object";

	void generateMatrix();
	void addChild(Object* obj, bool keep_world_transform);
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