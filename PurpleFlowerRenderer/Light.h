#pragma once
#include <Eigen>
using namespace Eigen;

struct Light
{
	//Vector4f Position;
	Vector3f Direction;
	Vector3f Color;
	float Intensity;

	std::vector<float> ShadowMap;
};