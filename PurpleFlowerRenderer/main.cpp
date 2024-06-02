#include <iostream>
#include "Renderer.h"
#include "OBJ_Loader.h"
#include "Shader.h"
#include "thread"
#include "opencv2/opencv.hpp"

constexpr int width = 700;
constexpr int height = 700;
std::vector<Object> objectList;
Light light;
Camera camera;
int frameCount = 0;
bool hasInput = false;
float inputFloat1;
float inputFloat2;

void SetCamera()
{
	camera.Position = Vector3f(0, 0, 10);
	camera.Direction = Vector3f(0, 0, -1).normalized();
	camera.Up = Vector3f(0, 1, 0).normalized();
	camera.Fov = 60.f;
	camera.Near = 0.1f;
	camera.Far = 60.f;
	camera.AspectRatio = width / height;
}

void SetLight()
{
	light.Direction = Vector3f(-1, 1, -1).normalized();
	light.Intensity = 0.8f;
	light.Color = Vector3f(1,1,1);
	light.ShadowMap.resize(height * width);
}

void Input()
{
	while (true)
	{
		Vector3f rotation = { 0,0,0 };
		Vector3f position = { 0,0,0 };
		
		if (GetAsyncKeyState(VK_UP))
			position += Vector3f(0, 0, -1);
		if (GetAsyncKeyState(VK_DOWN))
			position += Vector3f(0, 0, 1);
		if (GetAsyncKeyState(VK_LEFT))
			rotation += Vector3f(0, 1, 0);
		if (GetAsyncKeyState(VK_RIGHT))
			rotation += Vector3f(0, -1, 0);
		if (GetAsyncKeyState(VK_ADD))
			inputFloat1 += 0.01f;
		if (GetAsyncKeyState(VK_SUBTRACT))
			inputFloat1 -= 0.01f;
		if (GetAsyncKeyState(VK_HOME))
			inputFloat2 += 0.1f;
		if (GetAsyncKeyState(VK_DELETE))
			inputFloat2 -= 0.1f;

		for (auto& obj : objectList)
		{
			obj.Rotation += rotation;
		}
		
		camera.Position += position;
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}
}

void SetModel(std::string objName, Vector3f pos, Vector3f rotation, Vector3f scale,Shader* shader)
{
	objl::Loader loader;
	std::string objPath = "Res/Models/";
	bool bLoad = loader.LoadFile(objPath + objName + ".obj");
	if (bLoad)
	{
		for (const auto& mesh : loader.LoadedMeshes)
		{
			Object* o = new Object();
			
			for (int i = 0; i < mesh.Indices.size()-2; i += 3)
			{
				Triangle* t = new Triangle();
				for (int j = 0; j < 3; j++)
				{
					t->SetVertex(j, Vector3f(mesh.Vertices[mesh.Indices[i+j]].Position.X, mesh.Vertices[mesh.Indices[i + j]].Position.Y, mesh.Vertices[mesh.Indices[i + j]].Position.Z));
					t->SetColor(j, Vector3f(0.5f, 0.5f, 0.5f));
					t->SetNormal(j, Vector4f(mesh.Vertices[mesh.Indices[i + j]].Normal.X, mesh.Vertices[mesh.Indices[i + j]].Normal.Y, mesh.Vertices[mesh.Indices[i + j]].Normal.Z,1));
					t->SetUV(j, Vector2f(mesh.Vertices[mesh.Indices[i + j]].TextureCoordinate.X, mesh.Vertices[mesh.Indices[i + j]].TextureCoordinate.Y));
				}
				o->Mesh.push_back(*t);
			}

			o->Position = pos; 
			o->Rotation = rotation; 
			o->Scale = scale;
			o->Shader = shader;
			objectList.push_back(*o);
		}
	}
	else std::cout << "模型加载失败" << std::endl;
}

int main()
{

	SetModel("table", Vector3f(0, -4, 0), Vector3f(0, 90, 0), Vector3f(0.05f, 0.03f, 0.05f),
		new BlinnPhongShader(&light, &camera));

	//SetModel("bunny", Vector3f(0, -3, 0), Vector3f(0, 0, 0), Vector3f(30, 30, 30),new NormalShader());

	//SetModel("bunny", Vector3f(0, -3, 1), Vector3f(0, 0, 0), Vector3f(30, 30, 30),
	//	new BlinnPhongShader(&light, &camera));

	SetModel("bunny", Vector3f(0, -3, 0), Vector3f(0, 0, 0), Vector3f(30, 30, 30),
		new CartoonShader(&light, &camera,Vector3f(0,1,1)));

	//SetModel("Knife", Vector3f(0, 0, 0), Vector3f(0, 0, 0), Vector3f(1, 1, 1),
	//	new TextureShader(&light, &camera, new Texture("Knife")));

	//SetModel("Gun", Vector3f(0, 0, 0), Vector3f(0, 0, 0), Vector3f(10, 10, 10),
	//	new TextureShader(&light, &camera, new Texture("Gun")));

	/*SetModel("11090_Cyclops_v2", Vector3f(0, 0, 0), Vector3f(0, 0, 0), Vector3f(0.2, 0.2, 0.2),
		new BlinnPhongShader(&light, &camera));*/

	/*SetModel("11090_Cyclops_v2", Vector3f(0, 0, 0), Vector3f(0, 0, 0), Vector3f(0.2, 0.2, 0.2),
		new CartoonShader(&light, &camera, Vector3f(0, 1, 1)));*/

	/*SetModel("12140_Skull_v3_L2", Vector3f(0, 0, 0), Vector3f(0, 0, 0), Vector3f(0.15, 0.15, 0.15),
		new TextureShader(&light, &camera,new Texture("Skull")));*/

	//SetModel("12140_Skull_v3_L2", Vector3f(0, 0, 0), Vector3f(0, 0, 0), Vector3f(0.15, 0.15, 0.15),
	//	new DissolveShader(&light, &camera, Vector3f(1, 0, 0),
	//		new Texture("noise"), &inputFloat1,&inputFloat2));

	/*SetModel("Alien Animal", Vector3f(0, 0, 0), Vector3f(0, 0, 0), Vector3f(0.25, 0.25, 0.25),
		new DissolveShader(&light, &camera, Vector3f(1, 0, 0), new Texture("ScanningNoise"), 
			&inputFloat1, &inputFloat2));*/

	//SetModel("Spaceship", Vector3f(0, 0, 0), Vector3f(0, 0, 0), Vector3f(1, 1, 1),
	//	new BlinnPhongShader(&light, &camera));

	SetLight(); // 设置光照

	SetCamera(); //设置相机

	std::thread input(Input);

	//初始化渲染器
	Renderer r(width, height);

	do
	{
		r.Clear();
		std::vector<Object> list = objectList;

		//光栅化
		r.VertexShader(list, camera);
		r.FragmentShader(list);

		//绘制
		cv::Mat image(height, width, CV_32FC3, r.GetFrameBuffer().data());
		image.convertTo(image, CV_8UC3, 1.0f);
		cv::cvtColor(image, image, cv::COLOR_RGB2BGR);
		cv::imshow("PurpleFlowerRender", image);
		cv::waitKey(20);

		std::cout << "1输入:" << inputFloat1 << "\n";
		std::cout << "2输入:" << inputFloat2 << "\n";
		std::cout << "第" << ++frameCount <<"帧" << "\n";

	} while (true);

}