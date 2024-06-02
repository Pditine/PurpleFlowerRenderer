#pragma once
#include "Triangle.h"

class Shader;

struct Object
{
	Vector3f Position;
	Vector3f Rotation;
	Vector3f Scale;

	std::vector<Triangle> Mesh;

	Shader* Shader;
};