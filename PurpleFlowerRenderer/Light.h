#pragma once
#include <Eigen>
#include <iostream>
#include "opencv2/opencv.hpp"
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

	int ShadowMapWidth;
	int ShadowMapHeight;
	Camera VirtualCamera;
	
	std::vector<float> ShadowMap;

	void SetShadowMap(std::vector<Object>& theObjList,int width,int height)
	{
		ShadowMap.reserve(width * height);
		std::fill(ShadowMap.begin(), ShadowMap.end(), std::numeric_limits<float>::infinity());

		ShadowMapWidth = width;
		ShadowMapHeight = height;

		std::vector<Object> objList = theObjList;

		Renderer r = Renderer(width, height);

		VirtualCamera.Position = Position;
		VirtualCamera.Direction = Direction;

		Matrix3f rotation;
		rotation<< 0, 1, 0,
			0, 0, -1,
			1, 0, 0;

		VirtualCamera.Up = rotation *Direction;
		VirtualCamera.Fov = 60.f;
		VirtualCamera.Near = 0.1f;
		VirtualCamera.Far = 60.f;
		VirtualCamera.AspectRatio = width/height;

		r.VertexShader(objList, VirtualCamera);

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
							if (ShadowMap[x + (ShadowMapHeight - y - 1) * ShadowMapWidth] > theZ)
							{
								ShadowMap[x + (ShadowMapHeight - y - 1) * ShadowMapWidth] = theZ;

								float num = pow(theZ,40);
								r.GetFrameBuffer()[x + (ShadowMapHeight - y - 1) * ShadowMapWidth] = Vector3f(num, num, num) * 255.0f;
							}
						}
					}
				}
			}
		}

		cv::Mat image(height, width, CV_32FC3, r.GetFrameBuffer().data());
		image.convertTo(image, CV_8UC3, 1.0f);
		cv::cvtColor(image, image, cv::COLOR_RGB2BGR);
		cv::imshow("light_z", image);
		int key = cv::waitKey(1);
	}
};