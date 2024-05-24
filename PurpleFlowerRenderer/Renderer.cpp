#include "Renderer.h"
#include "Macro.h"
#include "Shader.h"


Renderer::Renderer(int w, int h) :_width(w), _height(h)
{
	_frameBuffer.resize(w * h);
	_zBuffer.resize(w * h);

	_viewport << _width / 2, 0, 0, _width / 2,
		0, _height / 2, 0, _height / 2,
		0, 0, 1, 0,
		0, 0, 0, 1;
}

void Renderer::Clear()
{
	std::fill(_frameBuffer.begin(), _frameBuffer.end(), Vector3f(0, 0, 0));
	std::fill(_zBuffer.begin(), _zBuffer.end(), std::numeric_limits<float>::infinity());
}

float Renderer::ToRadian(float angle)
{
	return (angle / 180) * PI;
}

void Renderer::SetPixelColor(int x,int y, const Vector3f color)
{
	int ind = GetPixelIndex(x,y);
	_frameBuffer[ind] = color;
}

Eigen::Vector3f Renderer::GetPixelColor(int x, int y)
{
	return _frameBuffer[GetPixelIndex(x, y)];
}


int Renderer::GetPixelIndex(int x, int y)
{
	return x+y*_width;
}

void Renderer::SetModelMatrix(const Object& o)
{
	Matrix4f rX, rY, rZ;
	float radX, radY, radZ;
	Matrix4f scale;
	Matrix4f move; 

	radX = ToRadian(o.Rotation.x());
	radY = ToRadian(o.Rotation.y());
	radZ = ToRadian(o.Rotation.z());

	rX << 1, 0, 0, 0,
		0, cos(radX), -sin(radX), 0,
		0, sin(radX), cos(radX), 0,
		0, 0, 0, 1;

	rY << cos(radY), 0, sin(radY), 0,
		0, 1, 0, 0,
		-sin(radY), 0, cos(radY), 0,
		0, 0, 0, 1;

	rZ << cos(radZ), -sin(radZ), 0, 0,
		sin(radZ), cos(radZ), 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1;

	scale << o.Scale.x(), 0, 0, 0,
		0, o.Scale.y(), 0, 0,
		0, 0, o.Scale.z(), 0,
		0, 0, 0, 1;

	move << 1, 0, 0, o.Position.x(),
		0, 1, 0, o.Position.y(),
		0, 0, 1, o.Position.z(),
		0, 0, 0, 1;

	_modelMatrix = move * rZ * rX * rY * scale;
}

void Renderer::SetViewMatrix(const Camera& c)
{
	//将摄像机移动到原点，然后使用旋转矩阵的正交性让摄像机摆正
	Matrix4f move; //移动矩阵
	Vector3f right; //摄像机的x轴
	Matrix4f rotateT; //旋转矩阵的转置矩阵

	move << 1, 0, 0, -c.Position.x(),
		0, 1, 0, -c.Position.y(),
		0, 0, 1, -c.Position.z(),
		0, 0, 0, 1;

	right = c.Direction.cross(c.Up);

	rotateT << right.x(), right.y(), right.z(), 0,
		c.Up.x(), c.Up.y(), c.Up.z(), 0,
		-c.Direction.x(), -c.Direction.y(), -c.Direction.z(), 0,
		0, 0, 0, 1;

	_viewMatrix = rotateT * move;
}

void Renderer::SetClipMatrix(const Camera& c)
{
	//透视投影矩阵
	Matrix4f p2o; //将梯台状透视视锥挤成长方体正交投影
	Matrix4f orthoTrans, orthoScale, ortho; //正交投影矩阵的平移和缩放分解
	float nearClipPlaneHeight, nearClipPlaneWidth;
	float radFov; //视野的弧度制

	radFov = ToRadian(c.Fov);
	nearClipPlaneHeight = tan(radFov / 2) * c.Near;
	nearClipPlaneWidth = c.AspectRatio * nearClipPlaneHeight;

	p2o << c.Near, 0, 0, 0,
		0, c.Near, 0, 0,
		0, 0, c.Far + c.Near, c.Near* c.Far,
		0, 0, -1, 0;

	orthoTrans << 1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, (c.Near + c.Far) / 2,
		0, 0, 0, 1;

	orthoScale << 1 / nearClipPlaneWidth, 0, 0, 0,
		0, 1 / nearClipPlaneHeight, 0, 0,
		0, 0, 2 / (c.Far - c.Near), 0,
		0, 0, 0, 1;

	ortho = orthoScale * orthoTrans;

	//矩阵左乘计算出透视投影矩阵
	_clipMatrix = ortho * p2o;
}

void Renderer::VertexShader(std::vector<Object>& objectList, Camera c)
{
	//对于每个物体
	for (Object& object : objectList)
	{
		//计算MVP矩阵
		SetModelMatrix(object);
		SetViewMatrix(c);
		SetClipMatrix(c);
		_mvp = _clipMatrix * _viewMatrix * _modelMatrix;

		//对于物体中的每个三角形
		for (Triangle& t : object.Mesh)
		{
			//对于三角形的每个顶点
			for (auto& vec : t.vertex)
			{
				//变换
				vec = _mvp * vec; // 转换到裁剪空间
				vec = _viewport * vec; // 转换到屏幕空间

				//齐次坐标归一化
				vec.x() /= vec.w();
				vec.y() /= vec.w();
				vec.z() /= vec.w();
				vec.w() /= vec.w();
			}
		}
	}
	return;
}

bool Renderer::InsideTriangle(const float x, const float y, const Triangle& t)
{
	Vector3f v[3] =
	{
		Vector3f(t.vertex[0].x(),t.vertex[0].y(),1.f),
		Vector3f(t.vertex[1].x(),t.vertex[1].y(),1.f),
		Vector3f(t.vertex[2].x(),t.vertex[2].y(),1.f)
	};

	Vector3f p(x, y, 1);

	//三条边的向量
	Vector3f side1, side2, side3;
	side1 = v[1] - v[0];
	side2 = v[2] - v[1];
	side3 = v[0] - v[2];

	//顶点到点的向量
	Vector3f p1, p2, p3;
	p1 = p - v[0];
	p2 = p - v[1];
	p3 = p - v[2];

	//叉乘
	Vector3f cross1, cross2, cross3;
	cross1 = p1.cross(side1);
	cross2 = p2.cross(side2);
	cross3 = p3.cross(side3);

	//判断是否同号
	if ((cross1.z() > 0 && cross2.z() > 0 && cross3.z() > 0) || (cross1.z() < 0 && cross2.z() < 0 && cross3.z() < 0))
	{
		return true;
	}
	else return false;
}

std::tuple<float, float, float> Renderer::Barycentric(float x, float y, const Vector4f* v)
{

	// 三角形顶点
	Vector4f A = v[0];
	Vector4f B = v[1];
	Vector4f C = v[2];

	float i = (-(x - B.x()) * (C.y() - B.y()) + (y - B.y()) * (C.x() - B.x())) /
		(-(A.x() - B.x()) * (C.y() - B.y()) + (A.y() - B.y()) * (C.x() - B.x()));
	float j = (-(x - C.x()) * (A.y() - C.y()) + (y - C.y()) * (A.x() - C.x())) /
		(-(B.x() - C.x()) * (A.y() - C.y()) + (B.y() - C.y()) * (A.x() - C.x()));
	float k = 1 - i - j;

	return { i,j,k };
}

Vector2f Renderer::Interpolate(float alpha, float beta, float gamma, const Vector2f& vert1, const Vector2f& vert2, const Vector2f& vert3)
{
	return (alpha * vert1 + beta * vert2 + gamma * vert3);
}

Vector3f Renderer::Interpolate(float alpha, float beta, float gamma, const Vector3f& vert1, const Vector3f& vert2, const Vector3f& vert3)
{
	return (alpha * vert1 + beta * vert2 + gamma * vert3);
}

Vector4f Renderer::Interpolate(float alpha, float beta, float gamma, const Vector4f& vert1, const Vector4f& vert2, const Vector4f& vert3)
{
	return (alpha * vert1 + beta * vert2 + gamma * vert3);
}

float Renderer::Interpolate(float alpha, float beta, float gamma, const float& vert1, const float& vert2, const float& vert3)
{
	return (alpha * vert1 + beta * vert2 + gamma * vert3);
}

void Renderer::FragmentShader(std::vector<Object>& objects)
{
	for (Object& object : objects)
	{
		for (Triangle& t : object.Mesh)
		{
			//包围盒
			float minXf, maxXf, minYf, maxYf;
			minXf = _width;
			maxXf = 0;
			minYf = _height;
			maxYf = 0;
			for (const auto& ver : t.vertex)
			{
				if (ver.x() < minXf) minXf = ver.x();
				if (ver.x() > maxXf) maxXf = ver.x();
				if (ver.y() < minYf) minYf = ver.y();
				if (ver.y() > maxYf) maxYf = ver.y();
			}

			if (minXf < 0) minXf = 0;
			if (maxXf > _width) maxXf = _width;
			if (minYf < 0) minYf = 0;
			if (maxYf > _height) maxYf = _height;

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
					if (InsideTriangle((float)x + 0.5, (float)y + 0.5, t))
					{
						//在的话计算2D重心坐标
						float alpha2D, beta2D, gamma2D;
						std::tie(alpha2D, beta2D, gamma2D) = Barycentric((float)x + 0.5f, (float)y + 0.5f, t.vertex);

						float theZ = Interpolate(alpha2D, beta2D, gamma2D, t.vertex[0].z(), t.vertex[1].z(), t.vertex[2].z());

						//判断深度值
						if (_zBuffer[GetPixelIndex(x, y)] > theZ)
						{
							//插值颜色，更新深度信息
							Vector3f interpolateColor = Interpolate(alpha2D, beta2D, gamma2D, t.color[0], t.color[1], t.color[2]);

							Vector4f interpolateNormal = Interpolate(alpha2D, beta2D, gamma2D, t.normal[0], t.normal[1], t.normal[2]);

							Vector2f interpolateUV = Interpolate(alpha2D, beta2D, gamma2D, t.uv[0], t.uv[1], t.uv[2]);

							//使用shader处理着色
							Vector3f pixelColor = object.Shader->GetColor({interpolateColor,(_modelMatrix*interpolateNormal).normalized(),interpolateUV });
							SetPixelColor(x, y, pixelColor);

							_zBuffer[GetPixelIndex(x, y)] = theZ;
						}
					}
				}
			}
		}
	}
}