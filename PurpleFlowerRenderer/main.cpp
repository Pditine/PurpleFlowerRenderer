#include <iostream>
#include <easyx.h>
#include "Renderer.h"
#include "OBJ_Loader.h"
#include "Shader.h"
#include "thread"

constexpr int width = 700;
constexpr int height = 700;
std::vector<Object> objectList;
Light light;
Camera camera;
int frameCount = 0;
bool hasInput = false;

void SetObjects()
{
	Object o1;
	Triangle t1;
	t1.SetVertex(Vector3f(0, 0.2f, -2), Vector3f(-2, 0, -2), Vector3f(1, 0, -2));
	t1.SetColor(Vector3f(255, 0, 0), Vector3f(0, 255, 0), Vector3f(0, 0, 255));
	o1.Mesh.push_back(t1);
	o1.Position = Vector3f(0, 0, 0);
	o1.Rotation = Vector3f(0, 0, 0);
	o1.Scale = Vector3f(2, 2, 1);
	objectList.push_back(o1);

	Object o2;
	Triangle t2;
	t2.SetVertex(Vector3f(2, 1, 0), Vector3f(-1, 4, -5), Vector3f(-1, -3, -5));
	t2.SetColor(Vector3f(100, 100, 200), Vector3f(100, 200, 100), Vector3f(100, 100, 100));
	o2.Mesh.push_back(t2);
	o2.Position = Vector3f(0, 0, 0);
	o2.Rotation = Vector3f(0, 0, 0);
	o2.Scale = Vector3f(1, 1, 1);
	objectList.push_back(o2);

}

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
	light.Direction = Vector3f(1, 1, 1).normalized();
	light.Intensity = 0.8f;
	light.Color = Vector3f(0.8f,0.8f,0.8f);
}

void Input()
{
	while(true)
	{
		Vector3f rotation = { 0,0,0 };

		if (GetAsyncKeyState(VK_UP))
			rotation += Vector3f(1, 0, 0);
		if (GetAsyncKeyState(VK_DOWN))
			rotation += Vector3f(-1, 0, 0);
		if (GetAsyncKeyState(VK_LEFT))
			rotation += Vector3f(0, 1, 0);
		if (GetAsyncKeyState(VK_RIGHT))
			rotation += Vector3f(0, -1, 0);

		for (auto& obj : objectList)
		{
			obj.Rotation += rotation;
		}

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
			
			for (int i = 0; i < mesh.Vertices.size()-3; i += 3)
			{
				Triangle* t = new Triangle();
				for (int j = 0; j < 3; j++)
				{
					t->SetVertex(j, Vector3f(mesh.Vertices[i + j].Position.X, mesh.Vertices[i + j].Position.Y, mesh.Vertices[i + j].Position.Z));
					t->SetColor(j, Vector3f(0.5f, 0.5f, 0.5f));
					t->SetNormal(j, Vector4f(mesh.Vertices[i + j].Normal.X, mesh.Vertices[i + j].Normal.Y, mesh.Vertices[i + j].Normal.Z,1));
					t->SetUV(j, Vector2f(mesh.Vertices[i + j].TextureCoordinate.X, mesh.Vertices[i + j].TextureCoordinate.Y));
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

	//SetModel("bunny", Vector3f(0, -3, 0), Vector3f(0, 0, 0), Vector3f(30, 30, 30),new NormalShader());

	//SetModel("bunny", Vector3f(0, -3, 0), Vector3f(0, 0, 0), Vector3f(30, 30, 30),
	//	new BlinnPhongShader(&light,&camera));

	/*SetModel("bunny", Vector3f(0, -3, 0), Vector3f(0, 0, 0), Vector3f(30, 30, 30),
		new TextureShader(&light, &camera, new Texture("Skull")));*/

	SetModel("Knife", Vector3f(0, 0, 0), Vector3f(0, 0, 0), Vector3f(1, 1, 1),
		new BlinnPhongShader(&light, &camera));

	//SetModel("Gun", Vector3f(0, 0, 0), Vector3f(0, 0, 0), Vector3f(10, 10, 10),
	//	new BlinnPhongShader(&light, &camera));

	/*SetModel("11090_Cyclops_v2", Vector3f(0, 0, 0), Vector3f(0, 0, 0), Vector3f(0.2, 0.2, 0.2),
		new BlinnPhongShader(&light, &camera));*/

	//SetModel("12140_Skull_v3_L2", Vector3f(0, 0, 0), Vector3f(0, 0, 0), Vector3f(0.15, 0.15, 0.15),
	//	new TextureShader(&light, &camera,new Texture("Skull")));

	//&light, &camera,new Texture("head-a0")

	SetLight(); // 设置光照

	SetCamera(); //设置相机

	initgraph(width, height);

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
		for (int x = 0; x < width; x++)
		{
			for (int y = 0; y < height; y++)
			{
				// 由于窗口y轴正方向向下,此处将y轴反转
				int realY = (height - y - 1);
				putpixel(x, y, RGB(r.GetPixelColor(x, realY).x(),r.GetPixelColor(x, realY).y(),r.GetPixelColor(x, realY).z()));
			}
		}

		std::cout << "第" << ++frameCount <<"帧" << "\n";

	} while (true);
}