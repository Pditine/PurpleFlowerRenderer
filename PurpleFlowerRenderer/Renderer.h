#pragma once
#include "Object.h"
#include "Camera.h"

constexpr float PI = 3.1415926f;

class Renderer
{
private:
	int _width, _height;

	Matrix4f _modelMatrix, _viewMatrix, _clipMatrix;
	Matrix4f _mvp;
	Matrix4f _viewport;

	std::vector<Vector3f> _frameBuffer;
	std::vector<float> _zBuffer;

public:

	Eigen::Vector3f& GetPixelColor(int x,int y); // 获取颜色

	std::vector<Vector3f>& GetFrameBuffer();

	Renderer(int w, int h);

	void Clear(); // 清除缓存

	static float ToRadian(float angle); // 弧度转角度

	int GetPixelIndex(int x, int y); // 获取像素索引

	void SetModelMatrix(const Object& o);

	void SetViewMatrix(const Camera& c);

	void SetClipMatrix(const Camera& c);

	void VertexShader(std::vector<Object>& objectList, Camera c);

	static bool InsideTriangle(const float x, const float y, const Triangle& t); // 点是否在三角形中

	static std::tuple<float, float, float> Barycentric(float x, float y, const Vector4f* v); // 重心坐标

	static Vector2f Interpolate(float alpha, float beta, float gamma, const Vector2f& vert1, const Vector2f& vert2, const Vector2f& vert3); // 通过重心坐标插值

	static Vector3f Interpolate(float alpha, float beta, float gamma, const Vector3f& vert1, const Vector3f& vert2, const Vector3f& vert3); // 通过重心坐标插值

	static Vector4f Interpolate(float alpha, float beta, float gamma, const Vector4f& vert1, const Vector4f& vert2, const Vector4f& vert3);

	float Interpolate(float alpha, float beta, float gamma, const float& vert1, const float& vert2, const float& vert3); // 通过重心坐标插值

	void SetPixelColor(int x,int y, const Vector3f color); // 使颜色存入帧缓冲

	void FragmentShader(std::vector<Object>& objects);
};