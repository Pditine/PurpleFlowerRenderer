#pragma once
#include <Eigen>
#include "Light.h"
#include "Camera.h"
#include "iostream"
#include "Texture.h"
using namespace Eigen;

struct FragmentData
{
	Vector3f color;
	Vector4f normal;
	Vector2f uv;
};

class Shader
{
public:

	virtual Vector3f GetColor(const FragmentData& data)
	{
		return data.color*255.0f;
	}

};

class NormalShader : public Shader
{
public:

	Vector3f GetColor(const FragmentData& data) override
	{
		Vector3f worldNormal = Vector3f(data.normal.x(), data.normal.y(), data.normal.z());
		Vector3f returnColor = (worldNormal + Vector3f(1.0f, 1.0f, 1.0f)) / 2.0f;
		return returnColor * 255.0f;
	}
};

inline float Clamp(float num)
{
	if (num < 0)
		return 0;
	if (num > 1)
		return 1;
	return num;
}

inline Vector3f BlendColor(Vector3f a,Vector3f b)
{
	return Vector3f(a.x()*b.x(),a.y()*b.y(),a.z()*b.z());
}

inline Vector3f BlendColor(Vector3f a, Vector3f b,float rote)
{
	a *= rote;
	b /= rote;
	return Vector3f(a.x() * b.x(), a.y() * b.y(), a.z() * b.z());
}

class BlinnPhongShader : public Shader
{
private:
	Light* _light;
	Camera* _camera;

public:
	BlinnPhongShader(Light* light,Camera* camera): _light(light),_camera(camera) {}

	Vector3f GetColor(const FragmentData& data) override
	{
		Vector3f worldNormal = Vector3f(data.normal.x(),data.normal.y(),data.normal.z());

		Vector3f resultColor = Vector3f(0, 0, 0);

		float halfLambert = (worldNormal.dot(_light->Direction)+1)/2;

		Vector3f diffuse = BlendColor(_light->Color*_light->Intensity,data.color,0.3f) * halfLambert;

		Vector3f ambient = Vector3f(0.05f, 0.05f, 0.05f);

		Vector3f halfDirection = (_camera->Direction + _light->Direction).normalized();

		Vector3f specular = BlendColor(_light->Color,Vector3f(1,1,1)) * pow(Clamp(halfDirection.dot(worldNormal)),50 )*_light->Intensity;

		resultColor += ambient+ diffuse + specular;

		return resultColor*255.f;
	}
};

class TextureShader : public Shader
{
private:
	Texture* _texture;
	Light* _light;
	Camera* _camera;

public:
	TextureShader(Light* light, Camera* camera,Texture* texture) : _light(light), _camera(camera),_texture(texture){}

	Vector3f GetColor(const FragmentData& data) override
	{
		auto color = _texture->GetColor(data.uv.x(), data.uv.y());
		Vector3f modelColor = Vector3f(static_cast<float>(color.r)/255.0f, static_cast<float>(color.g) / 255.0f, static_cast<float>(color.b) / 255.0f);

		Vector3f worldNormal = Vector3f(data.normal.x(), data.normal.y(), data.normal.z());

		Vector3f resultColor = Vector3f(0, 0, 0);

		float halfLambert = (worldNormal.dot(_light->Direction) + 1) / 2;

		Vector3f diffuse = BlendColor(_light->Color * _light->Intensity, modelColor) * halfLambert;

		Vector3f ambient = Vector3f(0.05f, 0.05f, 0.05f);

		Vector3f halfDirection = (_camera->Direction + _light->Direction).normalized();

		Vector3f specular = BlendColor(_light->Color, Vector3f(1, 1, 1)) * pow(Clamp(halfDirection.dot(worldNormal)), 50) * _light->Intensity;

		resultColor += ambient + diffuse + specular;

		return resultColor * 255.f;
	}
};

