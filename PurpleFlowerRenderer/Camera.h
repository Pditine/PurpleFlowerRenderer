#pragma once
#include <Eigen>
using namespace Eigen;
struct Camera
{
	Vector3f Position;
	Vector3f Up;
	Vector3f Direction;
	float Fov;
	float Near; 
	float Far;
	float AspectRatio;
};