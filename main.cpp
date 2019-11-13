#define GLEW_STATIC
#pragma comment(lib, "glew32s.lib")
#pragma comment(lib, "glfw3.lib")
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "Shader.h"
#include "imgui_impl_glfw_gl3.h"
#include <stdio.h>
#include <iostream>
#include "Camera.h"
#include <Windows.h>
#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <GL\GLU.h>
#include <istream>
#include <stdlib.h>
#include <vector>
#include "typeOperation.h"
using namespace std;
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
void MVP(const glm::mat4&, const glm::mat4&, const glm::mat4&, Shader&);
vec3f norm(const vec3f& vec);
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
float deltaTime = 0.0f;
float lastFrame = 0.0f;

void ComputeNormals(vec3i* q, vec4f* v, vec3f* n, const int& tri_num, const int& vert_num, const int& vert_offset) {
	memset(n, 0, sizeof(vec3f) * vert_num);
	vec3f a, b, c;
	int aid, bid, cid;
	for (int i = 0; i < tri_num; ++i) {
		aid = q[i].x - vert_offset;
		bid = q[i].y - vert_offset;
		cid = q[i].z - vert_offset;

		a = makeVec3f(v[aid]);
		b = makeVec3f(v[bid]);
		c = makeVec3f(v[cid]);
		n[aid] = n[aid] + norm(cross(b - a, c - a));
		n[bid] = n[bid] + norm(cross(c - b, a - b));
		n[cid] = n[cid] + norm(cross(a - c, b - c));
	}

	for (int i = 0; i < vert_num; ++i) {
		n[i] = norm(n[i]);
	}
}
void SetProjection(Shader& CubeShader, const glm::mat4& projection,float camera_x,float camera_y, float camera_z) {
	
	glm::mat4 model;
	model = glm::translate(model, glm::vec3(-1.5, 0.5f, -1.5f));
	glm::mat4 view = glm::lookAt(
		glm::vec3(camera_x, camera_y, camera_z),
		glm::vec3(-1.5, 0.5, -1.5),
		glm::vec3(0, 1, 0)
	);
	MVP(model, view, projection, CubeShader);
}
void MovingAround(Shader& CubeShader, float radius) {

	float camera_x = sin(glfwGetTime()) * radius, camera_y = 90, camera_z = cos(glfwGetTime()) * radius;
	glm::mat4 model;
	model = glm::mat4(1.0f);
	glm::mat4 view = glm::lookAt(
		glm::vec3(camera_x, camera_y, camera_z),
		glm::vec3(0, 0, 0), // and looks at the origin  
		glm::vec3(0, 1, 0) // Head is up (set to 0,-1,0 to look upside-down)        
	);
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 300.0f);
	MVP(model, view, projection, CubeShader);
}
void UsingCamera(Shader& CubeShader) {
	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
	glm::mat4 view = camera.GetViewMatrix();
	MVP(model, view, projection, CubeShader);
}
void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyBoard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyBoard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyBoard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyBoard(RIGHT, deltaTime);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Zack\'s OpenGL", NULL, NULL);


	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	if (GLEW_OK != glewInit()) {
		std::cout << "Failed to initialize GLEW" << std::endl;
		return EXIT_FAILURE;
	}
	glGetError();
	// Setup ImGui binding
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls
	ImGui_ImplGlfwGL3_Init(window, true);

	// Setup style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// Load Fonts
	// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them. 
	// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple. 
	// - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
	// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
	// - Read 'misc/fonts/README.txt' for more instructions and details.
	// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
	//io.Fonts->AddFontDefault();
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);0
	//ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
	//IM_ASSERT(font != NULL);


	Shader CubeShader("vshader.vs", "fshader.fs");

	float r = 1.0f;

	int segLen = 0;
	int maxPtNumForEachTrcLine = 0;
	int ptNum = 0;
	int maxTrcLineNum = 0;
	std::ifstream PointFile("..\\..\\cudaProjects\\Hultquist\\Points.bin", std::ios::binary);
	PointFile.seekg(0, std::ios::beg);
	PointFile.read((char*)(&segLen), sizeof(int) );
	PointFile.read((char*)(&maxPtNumForEachTrcLine), sizeof(int));
	PointFile.read((char*)(&ptNum), sizeof(int));
	PointFile.read((char*)(&maxTrcLineNum), sizeof(int));
	float floatNum =ptNum * 3;
	float* points = new float[floatNum];
	
	PointFile.read((char*)(points), sizeof(float)* floatNum);
	PointFile.close();

	std::cout << "segLen_DataInChunk:" << segLen << std::endl;
	std::cout << "maxPtNumForEachTrcLine:" << maxPtNumForEachTrcLine << std::endl;
	std::cout << "PtNum:" << floatNum/3 << std::endl;
	int i = 0;
	while (i < floatNum) {
			points[i] /= 51;
	/*	if (i % (128 * 3) == 0) {
			std::cout << i / (128*3) << std::endl;
		}
			if (i % 3 == 0)std::cout << std::endl;
			std::cout << points[i] << " ";*/
			i++;
	}

	/*int segLen_DataInAlternate = 0;
	int maxTrcLineNum = 0;
	std::ifstream PointFileInAlt("..\\..\\cudaProjects\\Hultquist\\Points_DataInAlternate.bin", std::ios::binary);

	PointFileInAlt.read((char*)(&segLen_DataInAlternate), sizeof(int));
	PointFileInAlt.read((char*)(&maxPtNumForEachTrcLine), sizeof(int));
	PointFileInAlt.read((char*)(&ptNum), sizeof(int));
	PointFileInAlt.read((char*)(&maxTrcLineNum), sizeof(int));
	std::cout << "segLen_DataInAlternate:" << segLen_DataInAlternate << std::endl;
	std::cout << "maxPtNumForEachTrcLine:" << maxPtNumForEachTrcLine << std::endl;
	std::cout << "PtNum:" << ptNum << std::endl;
	std::cout << "maxTrcLineNum:" << maxTrcLineNum << std::endl;
	int floatNumInAlt = ptNum * 3;
	float* pointInAlt = new float[floatNumInAlt];

	PointFileInAlt.read((char*)pointInAlt, sizeof(float)*floatNumInAlt);
	PointFileInAlt.close();*/

	std::ifstream PointIdxArrayFileInAlt("..\\..\\cudaProjects\\Hultquist\\PointsIndexArray.bin", std::ios::binary);
	int totalPtNum = 0;
	int seedingPtNum = 0;
	PointIdxArrayFileInAlt.read((char*)&seedingPtNum, sizeof(int));
	int* PointLenArray = new int[seedingPtNum];
	
	PointIdxArrayFileInAlt.read((char*)PointLenArray, sizeof(int)*seedingPtNum);
	
	PointIdxArrayFileInAlt.read((char*)& totalPtNum, sizeof(int));
	std::cout << totalPtNum << std::endl;
	std::cout << seedingPtNum << std::endl;
	int* PointIdxArray = new int[totalPtNum];
	PointIdxArrayFileInAlt.read((char*)PointIdxArray, sizeof(int)*totalPtNum);
	PointIdxArrayFileInAlt.close();
	
	
	
	
//	i = 0;
	/*while (i < floatNumInAlt) {
		std::cout<<pointInAlt[i]<<std::endl;
		i++;
	}*/
	int triNum = 0;
	int MaxTriangleNumForEachTrkLine = 0;
	int trackNum = 0;

	std::ifstream TriangleFile("..\\..\\cudaProjects\\Hultquist\\TriangleAll.bin", std::ios::binary);
//	std::ifstream TriangleFile(".\\Triangle.bin", std::ios::binary);

	TriangleFile.seekg(0, std::ios::beg);
	TriangleFile.read((char*)(&trackNum), sizeof(int));
	TriangleFile.read((char*)(&MaxTriangleNumForEachTrkLine), sizeof(int));
	TriangleFile.read((char*)(&triNum), sizeof(int));
	std::cout <<"triNum:"<< triNum << std::endl;
	std::cout << "trackNum:" << trackNum << std::endl;
	int* triangles_a = new int[triNum*3];

	TriangleFile.read((char*)triangles_a, sizeof(int)*triNum*3);
	TriangleFile.close();
	int intense_maxTrkNum = 32 * 64;
	vector<int> triangles16;
	vector<int> triangles8;
	vector<int> triangles4;
	vector<int> triangles2;
	vector<int> triangles1;
	for (int i = 0; i < trackNum/32; i++) {
		for (int j = 0; j < 32; j++) {
			if (j == 0) {
				int len = triangles_a[(i * 32 + j)*3] ;
				
				for (int h =1 ; h <= len; h++) {
					int index =( i * 32 + j + h * intense_maxTrkNum)*3;
					triangles16.push_back(triangles_a[index+0]);
					triangles16.push_back(triangles_a[index + 1]);
					triangles16.push_back(triangles_a[index + 2]);
				}
				continue;
			}
			else if (j <= 2 && j >= 1) {
				int len = triangles_a[(i * 32 + j) * 3];

				for (int h = 1; h <= len; h++) {
					int index = (i * 32 + j + h * intense_maxTrkNum) * 3;
					triangles8.push_back(triangles_a[index + 0]);
					triangles8.push_back(triangles_a[index + 1]);
					triangles8.push_back(triangles_a[index + 2]);
				}
				continue;
			}
			else if (j <= 6 && j >= 3) {
				int len = triangles_a[(i * 32 + j) * 3];

				for (int h = 1; h <= len; h++) {
					int index = (i * 32 + j + h * intense_maxTrkNum) * 3;
					triangles4.push_back(triangles_a[index + 0]);
					triangles4.push_back(triangles_a[index + 1]);
					triangles4.push_back(triangles_a[index + 2]);
				}
				continue;
			}
			else if (j <= 14 && j >= 7) {
				int len = triangles_a[(i * 32 + j) * 3];

				for (int h = 1; h <= len; h++) {
					int index = (i * 32 + j + h * intense_maxTrkNum) * 3;
					triangles2.push_back(triangles_a[index + 0]);
					triangles2.push_back(triangles_a[index + 1]);
					triangles2.push_back(triangles_a[index + 2]);
					
				}
				continue;
			}
			else if (j <= 30 && j >= 15) {
				int len = triangles_a[(i * 32 + j) * 3];

				for (int h = 1; h <= len; h++) {
					int index = (i * 32 + j + h * intense_maxTrkNum) * 3;
					triangles1.push_back(triangles_a[index + 0]);
					triangles1.push_back(triangles_a[index + 1]);
					triangles1.push_back(triangles_a[index + 2]);
				}
				continue;
			}
		}
	}
	delete[]triangles_a;
	std::ifstream TriangleFileAll("..\\..\\cudaProjects\\Hultquist\\Triangle.bin", std::ios::binary);
	//	std::ifstream TriangleFile(".\\Triangle.bin", std::ios::binary);

	TriangleFileAll.seekg(0, std::ios::beg);
	TriangleFileAll.read((char*)(&trackNum), sizeof(int));
	TriangleFileAll.read((char*)(&MaxTriangleNumForEachTrkLine), sizeof(int));
	TriangleFileAll.read((char*)(&triNum), sizeof(int));
	std::cout << "triNum:" << triNum << std::endl;
	std::cout << "trackNum:" << trackNum << std::endl;
	triangles_a = new int[triNum * 3];

	TriangleFileAll.read((char*)triangles_a, sizeof(int) * triNum * 3);
	TriangleFileAll.close();

	vector<int> triangles(triangles_a,triangles_a+triNum*3);
	/*for (int i = 0; i < triangles.size(); i++) {
		cout << triangles[i] << endl;
	}*/
	

	unsigned int VBO, VAO, VAOT, VBOT,VAOT16,VAOT8,VAOT4,VAOT2,VAOT1;
	unsigned EBO_Pt;
	unsigned int EBO_Tri, EBO_Tri16, EBO_Tri8, EBO_Tri4, EBO_Tri2, EBO_Tri1;
	glGenBuffers(1, &EBO_Tri16);
	glGenBuffers(1, &EBO_Tri8);
	glGenBuffers(1, &EBO_Tri4);
	glGenBuffers(1, &EBO_Tri2);
	glGenBuffers(1, &EBO_Tri1);
	glGenBuffers(1, &EBO_Tri);
	glGenBuffers(1, &EBO_Pt);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &VBOT);
	glGenVertexArrays(1, &VAO);
	glGenVertexArrays(1, &VAOT);
	glGenVertexArrays(1, &VAOT16);
	glGenVertexArrays(1, &VAOT8);
	glGenVertexArrays(1, &VAOT4);
	glGenVertexArrays(1, &VAOT2);
	glGenVertexArrays(1, &VAOT1);
	
	
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*floatNum, points, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_Pt);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int)* totalPtNum, PointIdxArray, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(VAOT);
	glBindBuffer(GL_ARRAY_BUFFER, VBOT);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)* floatNum, points, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_Tri);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * triangles.size(), &triangles.front(),GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(VAOT16);
	glBindBuffer(GL_ARRAY_BUFFER, VBOT);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * floatNum, points, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_Tri16);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * triangles16.size(), &triangles16[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(VAOT8);
	glBindBuffer(GL_ARRAY_BUFFER, VBOT);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * floatNum, points, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_Tri8);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * triangles8.size(), &triangles8[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(VAOT4);
	glBindBuffer(GL_ARRAY_BUFFER, VBOT);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * floatNum, points, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_Tri4);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * triangles4.size(), &triangles4[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(VAOT2);
	glBindBuffer(GL_ARRAY_BUFFER, VBOT);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)* floatNum, points, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_Tri2);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int)* triangles2.size(), &triangles2[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(VAOT1);
	glBindBuffer(GL_ARRAY_BUFFER, VBOT);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)* floatNum, points, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_Tri1);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int)* triangles1.size(), &triangles1[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	/*glBindVertexArray(VAOinAlt);
	glBindBuffer(GL_ARRAY_BUFFER, VBOinAlt);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)* floatNumInAlt, pointInAlt, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOinAlt);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int)* totalPtNum,PointIdxArray,GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);*/
	
	glEnableVertexAttribArray(0);

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int)*  4*prim_num, mQuads, GL_STATIC_DRAW);


	//glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(3 * sizeof(float)));
	//glEnableVertexAttribArray(1);

	glEnable(GL_DEPTH_TEST);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	bool projectionEnabled = false;
	bool persp = false;
	bool ortho = false;
	bool circling = false;
	bool showAll = true;
	bool show16 = false;
	bool show8 = false;
	bool show4 = false;
	bool show2 = false;
	bool show1 = false;
	bool showLine = true;
	bool FPS = true;
	float f = 300.0f;
	float radius = 10;
	CubeShader.use();

	float radians = 1.573f;
	float n = 0.1f;
	float left = -(float)SCR_WIDTH / 200;
	float right = (float)SCR_WIDTH / 200;
	float bottom = -(float)SCR_HEIGHT / 200;
	float top = (float)SCR_HEIGHT / 200;

	float camera_x = 50.562f, camera_y = 89.888f, camera_z = 129.213f;


	/*vec4f* vertices = new vec4f[ptNum];
	vec4f* colors = new vec4f[ptNum];
	int* indices = triangles_a;
	int num_vertices = ptNum;
	int num_primitives = triNum;
	int num_components = 3;
	vec3i* q = new vec3i[triNum];
	for (int i = 0; i < triNum; i++) {
		q[i].x = triangles[i * 3 + 0];
		q[i].y = triangles[i * 3 + 1];
		q[i].z = triangles[i * 3 + 2];
	}
	for (int i = 0; i < ptNum; i++) {
		vertices[i].x = points[i * 3 + 0];
		vertices[i].y = points[i * 3 + 1];
		vertices[i].z = points[i * 3 + 2];
		vertices[i].w = 1.0f;
		colors[i].r = 255.0f;
		colors[i].g = 255.0f;
		colors[i].b = 255.0f;
	}
	vec3f* normals=new vec3f[ptNum];
	ComputeNormals(q, vertices, normals, triNum, ptNum, 0);


	SurfaceRenderer mSurfaceRenderer(vertices, normals, colors, indices, num_vertices, num_primitives, num_components);*/
	while (!glfwWindowShouldClose(window)) {

		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glfwPollEvents();
		ImGui_ImplGlfwGL3_NewFrame();
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);

		glViewport(0, 0, display_w, display_h);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		processInput(window);

		glm::mat4 projection;

		ImGui::Checkbox("Projection", &projectionEnabled);
		ImGui::Checkbox("MovingAround", &circling);
		ImGui::Checkbox("FPS", &FPS);
		ImGui::Checkbox("showLine", &showLine);
		ImGui::Checkbox("showAll", &showAll);
		ImGui::Checkbox("show16", &show16);
		ImGui::Checkbox("show8", &show8);
		ImGui::Checkbox("show4", &show4);
		ImGui::Checkbox("show2", &show2);
		ImGui::Checkbox("show1", &show1);
		if (projectionEnabled) {
			ImGui::Begin("Choose projection", &projectionEnabled);

			ImGui::Checkbox("Perspective", &persp);
			ImGui::Checkbox("Orthographic", &ortho);
			ImGui::SliderFloat("far", &f, 20.0f, 2000.0f);
			ImGui::SliderFloat("near", &n, 0.1f, 20.0f);
			ImGui::SliderFloat("radians", &radians, 0.0f, 20.0f);

			if (persp) {
				projection = glm::perspective(glm::radians(radians), (float)SCR_WIDTH / (float)SCR_HEIGHT, n, f);

			}
			else if (ortho) {
				ImGui::SliderFloat("left", &left, -(float)SCR_WIDTH / 200, 0);
				ImGui::SliderFloat("right", &right, 0, (float)SCR_WIDTH / 200);
				ImGui::SliderFloat("bottom", &bottom, -(float)SCR_HEIGHT / 200, 0);
				ImGui::SliderFloat("top", &top, 0, (float)SCR_HEIGHT / 200);
				projection = glm::ortho(left, right, bottom, top, n, f);
			}
			ImGui::SliderFloat("x", &camera_x, -100.0f, 100.0f);
			ImGui::SliderFloat("y", &camera_y, -50.0f, 150.0f);
			ImGui::SliderFloat("z", &camera_z, -50.0f, 200.0f);
			SetProjection(CubeShader, projection,camera_x,camera_y,camera_z);
			if (ImGui::Button("Close Me"))
				projectionEnabled = false;
			ImGui::End();
		}
		else if (circling) {
			ImGui::Begin("Moving Around", &circling);
			ImGui::SliderFloat("Radius", &radius, 5.0f, 50.0f);
			MovingAround(CubeShader, radius);
					
			if (ImGui::Button("Close Me"))
				circling = false;
			ImGui::End();
		}
		else if (FPS) {
			UsingCamera(CubeShader);
			//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
		}
		if (showLine) {
			int lastLen = 0;
			glBindVertexArray(VAO);
			for (int i = 0; i < segLen; i++) {
				int len = PointLenArray[i];
				//		std::cout << len << std::endl;
				glDrawElements(GL_LINE_STRIP, len, GL_UNSIGNED_INT, (void*)((lastLen) * sizeof(GLuint)));
				lastLen += len;
				//	std::cout << lastLen << std::endl;
			}
		}
		if (showAll) {
			
			glBindVertexArray(VAOT);
			glDrawElements(GL_TRIANGLES, triangles.size(), GL_UNSIGNED_INT, nullptr);
			//	glDrawElements(GL_TRIANGLES,prim_num*4 , GL_UNSIGNED_INT, nullptr);
		}
		else if (show16) {
			glBindVertexArray(VAOT16);
			glDrawElements(GL_TRIANGLES, triangles16.size(), GL_UNSIGNED_INT, nullptr);
			//	glDrawElements(GL_TRIANGLES,prim_num*4 , GL_UNSIGNED_INT, nullptr);
		}
		else if (show8) {
			glBindVertexArray(VAOT8);
			glDrawElements(GL_TRIANGLES, triangles8.size(), GL_UNSIGNED_INT, nullptr);
		}
		else if (show4) {
			glBindVertexArray(VAOT4);
			glDrawElements(GL_TRIANGLES, triangles4.size(), GL_UNSIGNED_INT, nullptr);
		}
		else if (show2) {
			glBindVertexArray(VAOT2);
			glDrawElements(GL_TRIANGLES, triangles2.size(), GL_UNSIGNED_INT, nullptr);
		}
		else if (show1) {
			glBindVertexArray(VAOT1);
			glDrawElements(GL_TRIANGLES, triangles1.size(), GL_UNSIGNED_INT, nullptr);
		}
		glGetError();
		ImGui::Render();
		ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
		/*SRColorMode srccolormode = SRC_SURFACE_MODE;
		mSurfaceRenderer.render(SRC_MESH_MODE);*/
		glfwSwapBuffers(window);
	}
	/*delete[]mVertices;
	delete[]mColors;
	delete[]mNormals;
	delete[]mQuads;*/

	delete[] PointIdxArray;
	
	glDeleteBuffers(1, &EBO_Tri);
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glfwTerminate();
	return 0;
}
void MVP(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection, Shader& CubeShader) {
	// retrieve the matrix uniform locations
	unsigned int modelLoc = glGetUniformLocation(CubeShader.ID, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	CubeShader.setMat4("view", view);
	CubeShader.setMat4("projection", projection);
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

vec3f norm(const vec3f& vec) {
	vec3f ret = vec;
	float len = sqrtf(vec * vec);
	if (len < 1e-10) {
		return makeVec3f(0.0f);
	}
	return (ret / len);
}