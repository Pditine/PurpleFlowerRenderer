#pragma once
#include <Eigen>
using namespace Eigen;

class Triangle
{
public:
	Vector4f vertex[3];
	Vector4f worldVertex[3];
	Vector4f normal[3];
	Vector2f uv[3];
	Vector3f color[3];

	Triangle()
	{
		vertex[0] << 0, 0, 0, 1;
		vertex[1] << 0, 0, 0, 1;
		vertex[2] << 0, 0, 0, 1;

		color[0] << 0, 0, 0;
		color[1] << 0, 0, 0;
		color[2] << 0, 0, 0;
	}

	void SetVertex(const Vector3f& v0, const Vector3f& v1, const Vector3f& v2)
	{
		vertex[0] = Vector4f(v0.x(), v0.y(), v0.z(), 1.0f);
		vertex[1] = Vector4f(v1.x(), v1.y(), v1.z(), 1.0f);
		vertex[2] = Vector4f(v2.x(), v2.y(), v2.z(), 1.0f);
	}

	void SetVertex(const int& i, const Vector3f& ver)
	{
		vertex[i] = Vector4f(ver.x(), ver.y(), ver.z(), 1.f);
	}

	void SetColor(const Vector3f& c0, const Vector3f& c1, const Vector3f& c2)
	{
		color[0] = c0;
		color[1] = c1;
		color[2] = c2;
	}

	void SetColor(const int& i, const Vector3f& col)
	{
		color[i] = col;
	}

	void SetNormal(const int& i, const Vector4f& n)
	{
		normal[i] = Vector4f(n.x(), n.y(), n.z(),1.f);
	}

	void SetUV(const int& i, const Vector2f& tC)
	{
		uv[i] = tC;
	}

};