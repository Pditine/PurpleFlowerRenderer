#pragma once
#include "Object.h"
#include "Camera.h"

constexpr float PI = 3.1415926f;

class Texture;

class Renderer
{
private:
	int _width, _height;

	Matrix4f _viewport;

	std::vector<Vector3f> _frameBuffer;
	std::vector<float> _zBuffer;

public:

	Matrix4f GetViewport();

	std::vector<float>& GetZBuffer();

	Eigen::Vector3f& GetPixelColor(int x,int y); // ��ȡ��ɫ

	std::vector<Vector3f>& GetFrameBuffer();

	Renderer(int w, int h);

	void Clear(); // �������
	void Clear(Texture* background); // �������

	static float ToRadian(float angle); // ����ת�Ƕ�

	int GetPixelIndex(int x, int y); // ��ȡ��������

	static Matrix4f GetModelMatrix(const Object& o);

	static Matrix4f GetViewMatrix(const Camera& c);

	static Matrix4f GetClipMatrix(const Camera& c);

	void VertexShader(std::vector<Object>& objectList, Camera& c);

	static bool InsideTriangle(const float x, const float y, const Triangle& t); // ���Ƿ�����������

	static std::tuple<float, float, float> Barycentric(float x, float y, const Vector4f* v); // ��������

	static Vector2f Interpolate(float alpha, float beta, float gamma, const Vector2f& vert1, const Vector2f& vert2, const Vector2f& vert3); // ͨ�����������ֵ

	static Vector3f Interpolate(float alpha, float beta, float gamma, const Vector3f& vert1, const Vector3f& vert2, const Vector3f& vert3); // ͨ�����������ֵ

	static Vector4f Interpolate(float alpha, float beta, float gamma, const Vector4f& vert1, const Vector4f& vert2, const Vector4f& vert3);

	static float Interpolate(float alpha, float beta, float gamma, const float& vert1, const float& vert2, const float& vert3); // ͨ�����������ֵ

	void SetPixelColor(int x,int y, const Vector3f color); // ʹ��ɫ����֡����

	void FragmentShader(std::vector<Object>& objects);
};