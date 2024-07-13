#pragma once
#include <Eigen>
#include "Light.h"
#include "Camera.h"
#include "iostream"
#include "Texture.h"

struct Object;

using namespace Eigen;

struct FragmentData
{
	Object* object;
	Vector4f viewportPos;
	Vector4f worldPos;
	Vector3f color;
	Vector4f normal;
	Vector2f uv;
};

class Shader
{
public:

	virtual Vector3f GetColor(const FragmentData& data) = 0;

};

class NullShader : public Shader
{
	Vector3f GetColor(const FragmentData& data) override
	{
		return data.color;
	}
};

class NormalShader : public Shader
{
public:

	Vector3f GetColor(const FragmentData& data) override
	{
		Vector3f worldNormal = Vector3f(data.normal.x(), data.normal.y(), data.normal.z());
		Vector3f returnColor = (worldNormal + Vector3f(1.0f, 1.0f, 1.0f)) / 2.0f;
		return returnColor;
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
	return Vector3f(a.x() * 0.5f + b.x() * 0.5f, a.y() * 0.5f + b.y() * 0.5f, a.z() * 0.5f + b.z() * 0.5f);
}

inline Vector3f BlendColor(Vector3f a, Vector3f b,float rote)
{
	a *= rote;
	b *= 1-rote;
	return Vector3f(a.x() + b.x(), a.y() + b.y(), a.z() + b.z());
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

		Vector3f diffuse = BlendColor(_light->Color*_light->Intensity,data.color) * halfLambert;

		Vector3f ambient = Vector3f(0.05f, 0.05f, 0.05f);

		Vector3f halfDirection = (_camera->Direction + _light->Direction).normalized();

		Vector3f specular = BlendColor(_light->Color,Vector3f(1,1,1)) * pow(Clamp(halfDirection.dot(worldNormal)),50 )*_light->Intensity;

		resultColor += ambient+ diffuse + specular;

		return resultColor;
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

		return resultColor;
	}
};

class CartoonShader : public Shader
{
private:
	Light* _light;
	Camera* _camera;
	Vector3f _baseColor;

public:
	CartoonShader(Light* light, Camera* camera,Vector3f baseColor) : _light(light), _camera(camera),_baseColor(baseColor)
	{
	}

	Vector3f GetColor(const FragmentData& data) override
	{
		Vector3f worldNormal = Vector3f(data.normal.x(), data.normal.y(), data.normal.z());

		Vector3f resultColor = Vector3f(0, 0, 0);

		float halfLambert = (worldNormal.dot(_light->Direction) + 1) / 2;

		Vector3f diffuse = BlendColor(_light->Color * _light->Intensity, data.color) * halfLambert;

		Vector3f ambient = Vector3f(0.05f, 0.05f, 0.05f);

		Vector3f halfDirection = (_camera->Direction + _light->Direction).normalized();

		Vector3f specular = BlendColor(_light->Color, Vector3f(1, 1, 1)) * pow(Clamp(halfDirection.dot(worldNormal)), 50) * _light->Intensity;

		resultColor += ambient + diffuse + specular;

		float num = resultColor.norm();
		Vector3f returnColor = Vector3f();
		if (num > 0.8f) returnColor = _baseColor;
		else if (num > 0.6f) returnColor = _baseColor*0.8f;
		else if (num > 0.4f) returnColor = _baseColor*0.6f;
		else if (num > 0.2f) returnColor = _baseColor*0.4f;

		return returnColor;
	}
};

class DissolveShader : public Shader
{
private:
	Light* _light;
	Camera* _camera;
	Vector3f _lineColor;
	Texture* _noise;
	float* _cutValue;
	float* _lineWidth;

public:
	DissolveShader(Light* light, Camera* camera,Vector3f lineColor,Texture* noise,float* cutValue,float* lineWidth) :
	_light(light), _camera(camera),_lineColor(lineColor),_noise(noise), _cutValue(cutValue),_lineWidth(lineWidth) {}

	Vector3f GetColor(const FragmentData& data) override
	{
		Vector3f worldNormal = Vector3f(data.normal.x(), data.normal.y(), data.normal.z());

		Vector3f resultColor = Vector3f(0, 0, 0);

		float halfLambert = (worldNormal.dot(_light->Direction) + 1) / 2;

		Vector3f diffuse = BlendColor(_light->Color * _light->Intensity, data.color, 0.3f) * halfLambert;

		Vector3f ambient = Vector3f(0.05f, 0.05f, 0.05f);

		Vector3f halfDirection = (_camera->Direction + _light->Direction).normalized();

		Vector3f specular = BlendColor(_light->Color, Vector3f(1, 1, 1)) * pow(Clamp(halfDirection.dot(worldNormal)), 50) * _light->Intensity;

		resultColor += ambient + diffuse + specular;

		if (_noise->GetColor(data.uv.x(), data.uv.y()).r - *_cutValue *255 < 0)
		{
			return Vector3f(0,0,0);
		}

		if (_noise->GetColor(data.uv.x(), data.uv.y()).r - *_cutValue *255 - *_lineWidth< 0)
			return _lineColor;

		return resultColor;
	}
};

class ShadowShader : public Shader
{
private:
	Light* _light;
	Camera* _camera;
public:
	ShadowShader(Light* light, Camera* camera) : _light(light), _camera(camera) { }

	Vector3f GetColor(const FragmentData& data) override
	{
		Vector3f worldNormal = Vector3f(data.normal.x(), data.normal.y(), data.normal.z());

		Vector3f resultColor = Vector3f(0, 0, 0);

		float halfLambert = (worldNormal.dot(_light->Direction) + 1) / 2;

		Vector3f diffuse = BlendColor(_light->Color * _light->Intensity, data.color, 0.5f) * halfLambert;

		Vector3f ambient = Vector3f(0.05f, 0.05f, 0.05f);

		Vector3f halfDirection = (_camera->Direction + _light->Direction).normalized();

		Vector3f specular = BlendColor(_light->Color, Vector3f(1, 1, 1)) * pow(Clamp(halfDirection.dot(worldNormal)), 50) * _light->Intensity;

		Camera virtualCamera = _light->VirtualCamera;

		Vector4f worldPos = data.worldPos;

		Matrix4f lightViewport;
		lightViewport << _light->ShadowMapWidth / 2, 0, 0, _light->ShadowMapWidth / 2,
			0, _light->ShadowMapHeight / 2, 0, _light->ShadowMapHeight / 2,
			0, 0, 1, 0,
			0, 0, 0, 1;

		Vector4f lightViewportPos = lightViewport * Renderer::GetClipMatrix(virtualCamera) * Renderer::GetViewMatrix(virtualCamera)*worldPos;

		lightViewportPos.x() /= lightViewportPos.w();
		lightViewportPos.y() /= lightViewportPos.w();
		lightViewportPos.z() /= lightViewportPos.w();
		lightViewportPos.w() /= lightViewportPos.w();

		int x = floor(lightViewportPos.x());
		int y = floor(lightViewportPos.y());

		if (x >= 0 && x < _light->ShadowMapWidth &&
			y >= 0 && y < _light->ShadowMapHeight && 
			_light->ShadowMap[x + (_light->ShadowMapHeight - y - 1) * _light->ShadowMapWidth]!= std::numeric_limits<float>::infinity())
			if(lightViewportPos.z()> (_light->ShadowMap[x + (_light->ShadowMapHeight - y - 1) * _light->ShadowMapWidth])+0.00025f)
			{
				//std::cout <<lightViewportPos.z()<<":" << _light->ShadowMap[lightViewportPos.x() + (_light->ShadowMapHeight - lightViewportPos.y() - 1) * _light->ShadowMapWidth] << "\n";
				specular = Vector3f(0, 0, 0);
				diffuse = Vector3f(0, 0, 0);
			}

		resultColor += ambient + diffuse + specular;

		return resultColor;
	}
};

class ShadowTextureShader : public Shader
{
private:
	Light* _light;
	Camera* _camera;
	Texture* _texture;
public:
	ShadowTextureShader(Light* light, Camera* camera,Texture* texture) : _light(light), _camera(camera),_texture(texture) { }

	Vector3f GetColor(const FragmentData& data) override
	{
		auto color = _texture->GetColor(data.uv.x(), data.uv.y());
		Vector3f modelColor = Vector3f(static_cast<float>(color.r) / 255.0f, static_cast<float>(color.g) / 255.0f, static_cast<float>(color.b) / 255.0f);

		Vector3f worldNormal = Vector3f(data.normal.x(), data.normal.y(), data.normal.z());

		Vector3f resultColor = Vector3f(0, 0, 0);

		float halfLambert = (worldNormal.dot(_light->Direction) + 1) / 2;

		Vector3f diffuse =  modelColor * halfLambert;

		Vector3f ambient = Vector3f(0.05f, 0.05f, 0.05f);

		Vector3f halfDirection = (_camera->Direction + _light->Direction).normalized();

		Vector3f specular = BlendColor(_light->Color, Vector3f(1, 1, 1)) * pow(Clamp(halfDirection.dot(worldNormal)), 50) * _light->Intensity;

		Camera virtualCamera = _light->VirtualCamera;

		Vector4f worldPos = data.worldPos;

		Matrix4f lightViewport;
		lightViewport << _light->ShadowMapWidth / 2, 0, 0, _light->ShadowMapWidth / 2,
			0, _light->ShadowMapHeight / 2, 0, _light->ShadowMapHeight / 2,
			0, 0, 1, 0,
			0, 0, 0, 1;

		Vector4f lightViewportPos = lightViewport * Renderer::GetClipMatrix(virtualCamera) * Renderer::GetViewMatrix(virtualCamera) * worldPos;

		lightViewportPos.x() /= lightViewportPos.w();
		lightViewportPos.y() /= lightViewportPos.w();
		lightViewportPos.z() /= lightViewportPos.w();
		lightViewportPos.w() /= lightViewportPos.w();

		int x = floor(lightViewportPos.x());
		int y = floor(lightViewportPos.y());

		if (x >= 0 && x < _light->ShadowMapWidth &&
			y >= 0 && y < _light->ShadowMapHeight &&
			_light->ShadowMap[x + (_light->ShadowMapHeight - y - 1) * _light->ShadowMapWidth] != std::numeric_limits<float>::infinity())
			if (lightViewportPos.z() > (_light->ShadowMap[x + (_light->ShadowMapHeight - y - 1) * _light->ShadowMapWidth]) + 0.0003f)
			{
				//std::cout <<lightViewportPos.z()<<":" << _light->ShadowMap[lightViewportPos.x() + (_light->ShadowMapHeight - lightViewportPos.y() - 1) * _light->ShadowMapWidth] << "\n";
				specular = Vector3f(0, 0, 0);
				diffuse = modelColor*0.5f;
			}

		resultColor += ambient + diffuse + specular;

		return resultColor;
	}
};