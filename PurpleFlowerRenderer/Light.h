#pragma once
#include <Eigen>
#include "Object.h"
#include <vector>
#include "Renderer.h"
using namespace Eigen;

struct Light
{
	Vector3f Direction;
	Vector3f Color;
	float Intensity;
	Vector4f Position;

	std::vector<float> ShadowMap;

	void SetShadowMap(std::vector<Object>& objList,int width,int height)
	{
		ShadowMap.reserve(width * height);

		Renderer r = Renderer(width, height);
		Camera camera;
		camera.Position = Position;
		camera.Direction = Direction;
		camera.Up = Vector3f(0, 1, 0).normalized();
		camera.Fov = 60.f;
		camera.Near = 0.1f;
		camera.Far = 60.f;
		camera.AspectRatio = width/height;
		r.VertexShader(objList, camera);

		for (Object& object : objList)
		{
			for (Triangle& t : object.Mesh)
			{
				//包围盒
				float minXf, maxXf, minYf, maxYf;
				minXf = width;
				maxXf = 0;
				minYf = height;
				maxYf = 0;
				for (const auto& ver : t.vertex)
				{
					if (ver.x() < minXf) minXf = ver.x();
					if (ver.x() > maxXf) maxXf = ver.x();
					if (ver.y() < minYf) minYf = ver.y();
					if (ver.y() > maxYf) maxYf = ver.y();
				}

				if (minXf < 0) minXf = 0;
				if (maxXf > width) maxXf = width;
				if (minYf < 0) minYf = 0;
				if (maxYf > height) maxYf = height;

				//取整
				int minX, maxX, minY, maxY;
				minX = floor(minXf);
				maxX = ceil(maxXf);
				minY = floor(minYf);
				maxY = ceil(maxYf);

				//对包围盒内逐像素
				for (int y = minY; y < maxY; ++y)
				{
					for (int x = minX; x < maxX; ++x)
					{
						//判断像素是否在三角形内
						if (Renderer::InsideTriangle((float)x + 0.5f, (float)y + 0.5f, t))
						{
							//在的话计算2D重心坐标
							float alpha2D, beta2D, gamma2D;
							std::tie(alpha2D, beta2D, gamma2D) = Renderer::Barycentric((float)x + 0.5f, (float)y + 0.5f, t.vertex);

							float theZ = Renderer::Interpolate(alpha2D, beta2D, gamma2D, t.vertex[0].z(), t.vertex[1].z(), t.vertex[2].z());

							//判断深度值
							if (ShadowMap[r.GetPixelIndex(x, y)] > theZ)
								ShadowMap[r.GetPixelIndex(x, y)] = theZ;
						}
					}
				}
			}
		}
	}
};