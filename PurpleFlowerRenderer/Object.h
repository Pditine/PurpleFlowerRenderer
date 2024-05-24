#pragma once
#include "Triangle.h"
#include "Shader.h"

struct Object
{
	Vector3f Position;
	Vector3f Rotation;
	Vector3f Scale;

	std::vector<Triangle> Mesh;

	Shader* Shader;
};