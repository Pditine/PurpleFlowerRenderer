#include <iostream>
#include "Renderer.h"
#include "OBJ_Loader.h"
#include "Shader.h"
#include "thread"
#include <glfw3.h>

constexpr int width = 700;
constexpr int height = 700;
std::vector<Object> objectList;
Light light;
Camera camera;
int frameCount = 0;
bool hasInput = false;

float inputFloat1;
float inputFloat2;

bool firstMouse = true;
float lastX = 800.0f / 2.0f;
float lastY = 600.0f / 2.0f;
float yaw = -90.0f;
float pitch = 0.0f;

Vector4f Cast(Vector3f vec,float w)
{
	return Vector4f(vec.x(), vec.y(), vec.z(), w);
}

void SetCamera()
{
	camera.Position = Vector4f(0, 0, 20,1);
	camera.Direction = Vector3f(0, 0, -1).normalized();
	camera.Up = Vector3f(0, 1, 0).normalized();
	camera.Fov = 60.f;
	camera.Near = 0.1f;
	camera.Far = 60.f;
	camera.AspectRatio = width / height;
}

void SetLight()
{
	light.Position = Vector4f(10, 20, 10, 1);
	light.Direction = Vector3f(-1, -2, -1).normalized();
	light.Intensity = 0.8;
	light.Color = Vector3f(1,1,1);
	light.ShadowMap.resize(height * width);
}

void InputObject(GLFWwindow* window)
{
	Vector3f rotation = { 0,0,0 };

	//if (glfwGetKey(window, 56) == GLFW_PRESS) {
	//	// 处理按键 56 的逻辑
	//}
	//if (glfwGetKey(window, 50) == GLFW_PRESS) {
	//	// 处理按键 50 的逻辑
	//}
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		rotation += Vector3f(0, 3, 0);
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		rotation -= Vector3f(0, 3, 0);
	}

	for (auto& obj : objectList) {
		obj.Rotation += rotation;
	}
}

void MouseCameraInput(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // 注意这里是反过来的，因为y坐标是从底部往顶部增大的
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.1f; // 鼠标灵敏度
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	// 确保当pitch超出范围时屏幕不会翻转
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	Vector3f front = Vector3f(cos(yaw*PI/180.0f) * cos(pitch * PI / 180.0f), sin(pitch * PI / 180.0f), sin(yaw * PI / 180.0f)) * cos(pitch * PI / 180.0f);

	camera.Direction = front.normalized();

	Vector3f worldUp(0.0f, 1.0f, 0.0f);
	Vector3f right = camera.Direction.cross(worldUp).normalized();

	// 计算上向量
	camera.Up = right.cross(camera.Direction).normalized();
}

void InputCamera(GLFWwindow* window)
{
	float moveSpeed = 1.0f;
	float rotateSpeed = 0.05f;
	Vector3f right = camera.Direction.cross(camera.Up);

	if (glfwGetKey(window, 'W') == GLFW_PRESS) {
		camera.Position += Cast(camera.Direction * moveSpeed, 0.0f);
	}
	if (glfwGetKey(window, 'S') == GLFW_PRESS) {
		camera.Position -= Cast(camera.Direction * moveSpeed, 0.0f);
	}
	if (glfwGetKey(window, 'A') == GLFW_PRESS) {
		camera.Position -= Cast(right * moveSpeed, 0.0f);
	}
	if (glfwGetKey(window, 'D') == GLFW_PRESS) {
		camera.Position += Cast(right * moveSpeed, 0.0f);
	}
	if (glfwGetKey(window, 'Q') == GLFW_PRESS) {
		camera.Position += Cast(camera.Up * moveSpeed, 0.0f);
	}
	if (glfwGetKey(window, 'E') == GLFW_PRESS) {
		camera.Position -= Cast(camera.Up * moveSpeed, 0.0f);
	}
	if (glfwGetKey(window, 'J') == GLFW_PRESS) {
		camera.Direction = Vector3f(
			camera.Direction[0] * cos(rotateSpeed) - camera.Direction[2] * sin(rotateSpeed),
			camera.Direction[1],
			camera.Direction[0] * sin(rotateSpeed) + camera.Direction[2] * cos(rotateSpeed)
		);
		camera.Up = Vector3f(
			camera.Up[0] * cos(rotateSpeed) - camera.Up[2] * sin(rotateSpeed),
			camera.Up[1],
			camera.Up[0] * sin(rotateSpeed) + camera.Up[2] * cos(rotateSpeed)
		);
	}
	if (glfwGetKey(window, 'L') == GLFW_PRESS) {
		camera.Direction = Vector3f(
			camera.Direction[0] * cos(-rotateSpeed) - camera.Direction[2] * sin(-rotateSpeed),
			camera.Direction[1],
			camera.Direction[0] * sin(-rotateSpeed) + camera.Direction[2] * cos(-rotateSpeed)
		);
		camera.Up = Vector3f(
			camera.Up[0] * cos(-rotateSpeed) - camera.Up[2] * sin(-rotateSpeed),
			camera.Up[1],
			camera.Up[0] * sin(-rotateSpeed) + camera.Up[2] * cos(-rotateSpeed)
		);
	}
	if (glfwGetKey(window, 'I') == GLFW_PRESS) {
		camera.Direction = Vector3f(
			camera.Direction[0],
			camera.Direction[1] * cos(rotateSpeed) - camera.Direction[2] * sin(rotateSpeed),
			camera.Direction[1] * sin(rotateSpeed) + camera.Direction[2] * cos(rotateSpeed)
		);
		camera.Up = Vector3f(
			camera.Up[0],
			camera.Up[1] * cos(rotateSpeed) - camera.Up[2] * sin(rotateSpeed),
			camera.Up[1] * sin(rotateSpeed) + camera.Up[2] * cos(rotateSpeed)
		);
	}
	if (glfwGetKey(window, 'K') == GLFW_PRESS) {
		camera.Direction = Vector3f(
			camera.Direction[0],
			camera.Direction[1] * cos(-rotateSpeed) - camera.Direction[2] * sin(-rotateSpeed),
			camera.Direction[1] * sin(-rotateSpeed) + camera.Direction[2] * cos(-rotateSpeed)
		);
		camera.Up = Vector3f(
			camera.Up[0],
			camera.Up[1] * cos(-rotateSpeed) - camera.Up[2] * sin(-rotateSpeed),
			camera.Up[1] * sin(-rotateSpeed) + camera.Up[2] * cos(-rotateSpeed)
		);
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
					t->SetColor(j, Vector3f(0.3f, 0.3f, 0.3f));
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
	// 初始化GLFW
	if (!glfwInit()) {
		return -1;
	}

	// 创建窗口
	GLFWwindow* window = glfwCreateWindow(width, height, "PurpleFlowerRenderer", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	//初始化渲染器
	Renderer r(width, height);

	//SetModel("bunny", Vector3f(0, -3, 0), Vector3f(0, 0, 0), Vector3f(30, 30, 30),new NormalShader());

	/*SetModel("bunny", Vector3f(0, -3, 1), Vector3f(0, 0, 0), Vector3f(30, 30, 30),
		new BlinnPhongShader(&light, &camera));*/

	//SetModel("bunny", Vector3f(0, -3, 0), Vector3f(0, 0, 0), Vector3f(30, 30, 30),
	//	new CartoonShader(&light, &camera, Vector3f(0, 1, 1)));

	//SetModel("Knife", Vector3f(0, 0, 0), Vector3f(0, 0, 0), Vector3f(1, 1, 1),
	//	new TextureShader(&light, &camera, new Texture("Knife")));

	//SetModel("Gun", Vector3f(0, 0, 0), Vector3f(0, 0, 0), Vector3f(10, 10, 10),
	//	new TextureShader(&light, &camera, new Texture("Gun")));

	/*SetModel("11090_Cyclops_v2", Vector3f(0, 0, 0), Vector3f(0, 0, 0), Vector3f(0.2, 0.2, 0.2),
		new BlinnPhongShader(&light, &camera));*/

		/*SetModel("11090_Cyclops_v2", Vector3f(0, 0, 0), Vector3f(0, 0, 0), Vector3f(0.2, 0.2, 0.2),
			new CartoonShader(&light, &camera, Vector3f(0, 1, 1)));*/

	//SetModel("12140_Skull_v3_L2", Vector3f(0, 0, 0), Vector3f(-90, 0, 0), Vector3f(0.15, 0.15, 0.15),
	//	new TextureShader(&light, &camera, new Texture("Skull")));

				//SetModel("12140_Skull_v3_L2", Vector3f(0, 0, 0), Vector3f(0, 0, 0), Vector3f(0.15, 0.15, 0.15),
				//	new DissolveShader(&light, &camera, Vector3f(1, 0, 0),
				//		new Texture("noise"), &inputFloat1,&inputFloat2));

				/*SetModel("Alien Animal", Vector3f(0, 0, 0), Vector3f(0, 0, 0), Vector3f(0.25, 0.25, 0.25),
					new DissolveShader(&light, &camera, Vector3f(1, 0, 0), new Texture("ScanningNoise"),
						&inputFloat1, &inputFloat2));*/

						//SetModel("Spaceship", Vector3f(0, 0, 0), Vector3f(0, 0, 0), Vector3f(1, 1, 1),
						//	new BlinnPhongShader(&light, &camera));

	SetModel("Town", Vector3f(0, -4, 0), Vector3f(0, 0, 0), Vector3f(2, 2, 2),
		new ShadowTextureShader(&light, &camera, new Texture("Town")));

	//SetModel("Table", Vector3f(0, -6, 0), Vector3f(0, 0, 0), Vector3f(5, 3, 5),
	//	new ShadowShader(&light, &camera));

	SetLight(); // 设置光照

	SetCamera(); //设置相机

	glfwSetCursorPosCallback(window, MouseCameraInput);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	do
	{
		light.SetShadowMap(objectList, 700, 700);

		//r.Clear();
		r.Clear(new Texture("sky"));
		std::vector<Object> list = objectList;

		//光栅化
		r.VertexShader(list, camera);
		r.FragmentShader(list);

		glClear(GL_COLOR_BUFFER_BIT);

		// 设置投影矩阵
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0.0, width, 0.0, height, -1.0, 1.0);

		// 设置模型视图矩阵
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		// 渲染_frameBuffer中的颜色数据

		glBegin(GL_POINTS);
		glPointSize(1.0f);

		for (int y = 0; y < height; y++)
			for (int x = 0; x < width; x++)
			{
				auto color = r.GetFrameBuffer()[r.GetPixelIndex(x, y)];
				glColor3f(color.x(), color.y(), color.z());
				glVertex2f(x, y); // 仅作示例，实际根据需要绘制不同的图元
			}

		glEnd();

		glfwSwapBuffers(window);
		glfwPollEvents();

		std::cout << "1输入:" << inputFloat1 << "\n";
		std::cout << "2输入:" << inputFloat2 << "\n";
		std::cout << "第" << ++frameCount << "帧" << "\n";

		InputObject(window);
		InputCamera(window);

	} while (!glfwWindowShouldClose(window));

	glfwDestroyWindow(window);
	glfwTerminate();

}