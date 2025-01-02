#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "shader.h"
#include "camera.h"
#include "model.h"

#include "ltc_matrix.hpp"
#include "curve.hpp"
#include "sweep.hpp"

#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int loadTexture(const char* path);
unsigned int loadCubemap(vector<std::string> faces);
unsigned int loadMTexture();
unsigned int loadLUTTexture();

// ��������
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 960;

// ���������
Camera camera(glm::vec3(0.0f, 0.3f, 3.3f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
bool lockCursor = false;

// ʱ������
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// ��������
glm::vec3 lightPos(0.0f, 0.745f, 2.0f);
glm::vec3 cubePos(0.0f, 0.3f, 2.0f);
glm::vec3 areaLightTranslate;
static float intensity = 50.0f;
static glm::vec3 color = glm::vec3(0.184314f, 0.309804f, 0.309804f);
static float roughness = 0.5f;
static bool twoSided = true;

// �����ؼ������������
float rectangle_width = 0.05f;
float rectangle_length = 0.1f;
float ellipse_a = 0.1f;
float ellipse_b = 0.05f;
float circle_radius = 0.05f;
bool ifDrawSweep = false;
bool ifDrawCurve = true;

// imgui����
const char* glsl_version = "#version 330";
ImVec4 clear_color = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
ImVec4 light_color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

struct LTC_matrices
{
	unsigned int mat1;
	unsigned int mat2;
};

int main()
{
	// ��ʼ��������glfw
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw��������
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
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

	// ���� GLFW �������ǵ����
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	// glad���������� OpenGL ����ָ��
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// imgui init
	//  Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;
	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	// ����ȫ�� OpenGL ״̬
	// -----------------------------
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_PROGRAM_POINT_SIZE);

	// ����shader����
	// ------------------------------------
	Shader lightingShader("lighting_vertex.glsl", "lighting_fragment.glsl");
	Shader lightCubeShader("lightcube_vertex.glsl", "lightcube_fragment.glsl");
	Shader areaLightShader("area_light_vertex.glsl", "area_light_fragment.glsl");
	Shader skyboxShader("skybox_vertex.glsl", "skybox_fragment.glsl");

	// ͳһ�����õ���������Ϣ(ÿһ��ǰ��������Ϊ������꣬������Ϊ������)
	// ------------------------------------------------------------------
	float vertices[] = {
		/*
		 * �������꣬					  ��������              ��������
		 */
		 // ������ĺ���
		 -1.0f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		 1.0f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
		 1.0f, 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
		 1.0f, 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
		 -1.0f, 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
		 -1.0f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		 // �����������
		 -1.0f, -0.5f, 0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
		 1.0f, -0.5f, 0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
		 1.0f, 0.5f, 0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
		 1.0f, 0.5f, 0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
		 -1.0f, 0.5f, 0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
		 -1.0f, -0.5f, 0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
		 // �����������
		 -1.0f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		 -1.0f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		 -1.0f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		 -1.0f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		 -1.0f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		 -1.0f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		 // �����������
		 1.0f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		 1.0f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		 1.0f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		 1.0f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		 1.0f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		 1.0f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		 // ������ĵ���
		 -1.0f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
		 -1.0f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		 1.0f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		 -1.0f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
		 1.0f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		 1.0f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,

		 // ������Ķ���
		 -1.0f, 0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
		 1.0f, 0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
		 1.0f, 0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		 1.0f, 0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		 -1.0f, 0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		 -1.0f, 0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f };

	// ��ȡ����ƽ�������
	//  ------------------------------------------------------------------
	float FWallVertices[48];
	std::copy(vertices + 0, vertices + 48, FWallVertices);
	float LWallVertices[48];
	std::copy(vertices + 96, vertices + 144, LWallVertices);
	float RWallVertices[48];
	std::copy(vertices + 144, vertices + 192, RWallVertices);
	float FloorVertices[48];
	std::copy(vertices + 192, vertices + 240, FloorVertices);
	float CeilingVertices[48];
	std::copy(vertices + 240, vertices + 288, CeilingVertices);

	// �����컨��Ķ�����Ϣ
	// ------------------------------------------------------------------
	unsigned int VBO1, CeilingVAO;
	{
		glGenVertexArrays(1, &CeilingVAO);
		glGenBuffers(1, &VBO1);

		glBindBuffer(GL_ARRAY_BUFFER, VBO1);
		glBufferData(GL_ARRAY_BUFFER, sizeof(CeilingVertices), CeilingVertices, GL_STATIC_DRAW);

		glBindVertexArray(CeilingVAO);

		// ����λ��
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(0 * sizeof(float)));
		glEnableVertexAttribArray(0);
		// ���뷨����
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		// ������������
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
	}

	// ����ذ�Ķ�����Ϣ
	//  ------------------------------------------------------------------
	unsigned int VBO2, FloorVAO;
	{
		glGenVertexArrays(1, &FloorVAO);
		glGenBuffers(1, &VBO2);

		glBindBuffer(GL_ARRAY_BUFFER, VBO2);
		glBufferData(GL_ARRAY_BUFFER, sizeof(FloorVertices), FloorVertices, GL_STATIC_DRAW);

		glBindVertexArray(FloorVAO);

		// ����λ��
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(0 * sizeof(float)));
		glEnableVertexAttribArray(0);
		// ���뷨����
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		// ������������
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
	}

	// ������ǽ�Ķ�����Ϣ
	//  ------------------------------------------------------------------
	unsigned int VBO3, LWallVAO;
	{
		glGenVertexArrays(1, &LWallVAO);
		glGenBuffers(1, &VBO3);

		glBindBuffer(GL_ARRAY_BUFFER, VBO3);
		glBufferData(GL_ARRAY_BUFFER, sizeof(LWallVertices), LWallVertices, GL_STATIC_DRAW);

		glBindVertexArray(LWallVAO);

		// ����λ��
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(0 * sizeof(float)));
		glEnableVertexAttribArray(0);
		// ���뷨����
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		// ������������
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
	}

	// ������ǽ�Ķ�����Ϣ
	//  ------------------------------------------------------------------
	unsigned int VBO4, RWallVAO;
	{
		glGenVertexArrays(1, &RWallVAO);
		glGenBuffers(1, &VBO4);

		glBindBuffer(GL_ARRAY_BUFFER, VBO4);
		glBufferData(GL_ARRAY_BUFFER, sizeof(RWallVertices), RWallVertices, GL_STATIC_DRAW);

		glBindVertexArray(RWallVAO);

		// ����λ��
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(0 * sizeof(float)));
		glEnableVertexAttribArray(0);
		// ���뷨����
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		// ������������
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
	}

	// ����ǰǽ�Ķ�����Ϣ
	//  ------------------------------------------------------------------
	unsigned int VBO5, FWallVAO;
	{
		glGenVertexArrays(1, &FWallVAO);
		glGenBuffers(1, &VBO5);

		glBindBuffer(GL_ARRAY_BUFFER, VBO5);
		glBufferData(GL_ARRAY_BUFFER, sizeof(FWallVertices), FWallVertices, GL_STATIC_DRAW);

		glBindVertexArray(FWallVAO);

		// ����λ��
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(0 * sizeof(float)));
		glEnableVertexAttribArray(0);
		// ���뷨����
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		// ������������
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
	}

	// ���뷽��ƵĶ�����Ϣ
	unsigned int VBO6, lightCubeVAO;
	{
		glGenVertexArrays(1, &lightCubeVAO);
		glGenBuffers(1, &VBO6);

		glBindBuffer(GL_ARRAY_BUFFER, VBO6);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glBindVertexArray(lightCubeVAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO6);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
	}

	// �����Դ
	float areaLightVertices[] = {
		-1.0f * 0.1f, 0.5f * 0.1f, 0.5f * 0.1f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
		-1.0f * 0.1f, 0.5f * 0.1f, -0.5f * 0.1f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		1.0f * 0.1f, 0.5f * 0.1f, -0.5f * 0.1f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		-1.0f * 0.1f, 0.5f * 0.1f, 0.5f * 0.1f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
		1.0f * 0.1f, 0.5f * 0.1f, -0.5f * 0.1f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		1.0f * 0.1f, 0.5f * 0.1f, 0.5f * 0.1f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f };
	unsigned int VBO7, areaLightVAO;
	{
		glGenVertexArrays(1, &areaLightVAO);
		glGenBuffers(1, &VBO7);

		glBindBuffer(GL_ARRAY_BUFFER, VBO7);
		glBufferData(GL_ARRAY_BUFFER, sizeof(areaLightVertices), areaLightVertices, GL_STATIC_DRAW);

		glBindVertexArray(areaLightVAO);

		// ����λ��
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(0 * sizeof(float)));
		glEnableVertexAttribArray(0);
		// ���뷨����
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		// ������������
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
	}

	// ������պ�
	float skyboxVertices[] = {
		// positions
		-1.0f, 1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,

		-1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,

		-1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 1.0f };

	// skybox VAO
	unsigned int skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	// load textures
	// -------------
	vector<std::string> faces{
		"./skybox/px.png",
		"./skybox/nx.png",
		"./skybox/py.png",
		"./skybox/ny.png",
		"./skybox/pz.png",
		"./skybox/nz.png" };
	unsigned int cubemapTexture = loadCubemap(faces);
	skyboxShader.use();
	skyboxShader.setInt("skybox", 0);

	unsigned int concreteTexture = loadTexture("./textures/concreteTexture.png");

	// LUT textures
	LTC_matrices mLTC;
	mLTC.mat1 = loadMTexture();
	mLTC.mat2 = loadLUTTexture();

	areaLightTranslate = lightPos;

	// ����������
	int curcontrolpoint = 0;
	curve* mycurve = new curve();
	mycurve->num = 3;
	mycurve->verts = new float[mycurve->num * 3]();
	for (int i = 0; i < 3; ++i)
	{
		mycurve->verts[i * 3] = 0.5 * (i - 1);
		mycurve->verts[i * 3 + 1] = 0;
		mycurve->verts[i * 3 + 2] = 2.0f;
	}
	unsigned int curve_lineVAO;
	glGenVertexArrays(1, &curve_lineVAO);
	unsigned int curve_lineVBO;
	glGenBuffers(1, &curve_lineVBO);
	unsigned int curve_pointVAO;
	glGenVertexArrays(1, &curve_pointVAO);
	unsigned int curve_pointVBO;
	glGenBuffers(1, &curve_pointVBO);
	unsigned int instanceVBO;
	glGenBuffers(1, &instanceVBO);
	Shader curve_line_shader("curve_line_vertex.glsl", "curve_line_fragment.glsl");
	Shader curve_point_shader("curve_point_vertex.glsl", "curve_point_fragment.glsl");

	// ɨ��
	sweep* mysweep = new sweep();
	Shader sweep_shader("sweep_vertex.glsl", "sweep_fragment.glsl");

	// ��Ⱦѭ��
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// ʱ���߼�
		// --------------------
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// ����
		// -----
		processInput(window);

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::Begin("panel"); // Create a window called "panel" and append into it.
		ImGui::SetWindowPos(ImVec2(0, 0));
		ImGui::SetWindowSize(ImVec2(500, 170));
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
		ImGui::Checkbox("Lock Cursor(Shortcut: SPACE)", &lockCursor);
		ImGui::Checkbox("double sided lighting", &twoSided);
		ImGui::ColorEdit3("light color", (float*)&light_color);
		ImGui::SliderFloat("light intensity", &intensity, 0.0f, 200.0f);
		ImGui::SliderFloat("material roghness", &roughness, 0.0f, 1.0f);
		ImGui::End();

		if (ifDrawCurve)
		{
			ImGui::Begin("dashboard of Bezier Curve");
			ImGui::SetWindowPos(ImVec2(0, 170));
			ImGui::SetWindowSize(ImVec2(400, 140));
			ImGui::InputInt("control point num", &mycurve->num);
			ImGui::SliderInt("cur control point", &curcontrolpoint, 0, mycurve->num - 1);
			ImGui::SliderFloat("controlpoint x", (mycurve->verts + 3 * curcontrolpoint), -1.0, 1.0);
			ImGui::SliderFloat("controlpoint y", (mycurve->verts + 3 * curcontrolpoint + 1), -1.0, 1.0);
			ImGui::End();
		}

		if (ifDrawSweep)
		{
			ImGui::Begin("dashboard of 3 section");
			ImGui::SetWindowPos(ImVec2(0, 310));
			ImGui::SetWindowSize(ImVec2(400, 170));
			ImGui::SliderFloat("rectangle with", &rectangle_width, 0.01, 1.0);
			ImGui::SliderFloat("rectangle length", &rectangle_length, 0.01, 1.0);
			ImGui::SliderFloat("ellipse a", &ellipse_a, 0.01, 1.0);
			ImGui::SliderFloat("ellipse b", &ellipse_b, 0.01, 1.0);
			ImGui::SliderFloat("cirle radius", &circle_radius, 0.01, 1.0);
			ImGui::End();
		}

		// ��ʼ��Ⱦ
		// ------
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// ȷ�������� Uniforms/Drawing ����ʱ���� Shader
		//---------------------------------------------------------------------
		lightingShader.use();
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat3 normalMatrix = glm::mat3(model);
		model = glm::translate(model, cubePos);
		model = glm::scale(model, glm::vec3(1.0f));
		lightingShader.setMat4("model", model);
		lightingShader.setMat3("normalMatrix", normalMatrix);
		lightingShader.setMat4("view", view);
		lightingShader.setMat4("projection", projection);
		lightingShader.setVec3("areaLight.points[0]", glm::vec3(-1.0f * 0.1f, 0.5f * 0.1f - 0.1f, 0.5f * 0.1f));
		lightingShader.setVec3("areaLight.points[1]", glm::vec3(-1.0f * 0.1f, 0.5f * 0.1f - 0.1f, -0.5f * 0.1f));
		lightingShader.setVec3("areaLight.points[2]", glm::vec3(1.0f * 0.1f, 0.5f * 0.1f - 0.1f, -0.5f * 0.1f));
		lightingShader.setVec3("areaLight.points[3]", glm::vec3(1.0f * 0.1f, 0.5f * 0.1f - 0.1f, 0.5f * 0.1f));
		lightingShader.setVec3("areaLight.color", light_color.x, light_color.y, light_color.z);
		lightingShader.setFloat("areaLight.intensity", intensity);
		lightingShader.setVec4("material.albedoRoughness", glm::vec4(color, roughness));
		lightingShader.setFloat("areaLight.twoSided", twoSided);
		lightingShader.setInt("LTC1", 0);
		lightingShader.setInt("LTC2", 1);
		lightingShader.setInt("material.diffuse", 2);
		lightingShader.setVec3("viewPosition", camera.Position);
		lightingShader.setVec3("areaLightTranslate", areaLightTranslate);

		// �����컨��
		{
			// ��Ⱦ
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, mLTC.mat1);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, mLTC.mat2);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, concreteTexture);
			glBindVertexArray(CeilingVAO);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}

		// ���Ƶذ�
		{
			// ��Ⱦ
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, mLTC.mat1);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, mLTC.mat2);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, concreteTexture);
			glBindVertexArray(FloorVAO);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}

		// ������ǽ
		{
			// ��Ⱦ
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, mLTC.mat1);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, mLTC.mat2);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, concreteTexture);
			glBindVertexArray(LWallVAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		// ������ǽ
		{
			// ��Ⱦ
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, mLTC.mat1);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, mLTC.mat2);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, concreteTexture);
			glBindVertexArray(RWallVAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		// ����ǰǽ
		{
			// ��Ⱦ
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, mLTC.mat1);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, mLTC.mat2);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, concreteTexture);
			glBindVertexArray(FWallVAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		// ���Ƶذ�
		{
			// ��Ⱦ
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, mLTC.mat1);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, mLTC.mat2);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, concreteTexture);
			glBindVertexArray(FloorVAO);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
		// ���������Դ
		{
			areaLightShader.use();
			glm::mat4 model(1.0f);
			areaLightShader.setVec3("lightColor", light_color.x, light_color.y, light_color.z);
			glm::vec3 areaLightCubeTranslate(areaLightTranslate.x, areaLightTranslate.y - 0.1f, areaLightTranslate.z);
			model = glm::translate(model, areaLightCubeTranslate);
			model = glm::scale(model, glm::vec3(1.0f));
			areaLightShader.setMat4("model", model);
			areaLightShader.setMat4("view", view);
			areaLightShader.setMat4("projection", projection);
			glBindVertexArray(areaLightVAO);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}

		// ���ƵƷ���
		{
			lightCubeShader.use();
			lightCubeShader.setMat4("projection", projection);
			lightCubeShader.setMat4("view", view);
			model = glm::mat4(1.0f);
			model = glm::translate(model, areaLightTranslate);
			model = glm::scale(model, glm::vec3(0.1f)); // a smaller cube
			lightCubeShader.setMat4("model", model);
			lightCubeShader.setVec4("ourColor", glm::vec4(light_color.x - 0.3, light_color.y - 0.3, light_color.z - 0.3, 1.0f));
			glBindVertexArray(lightCubeVAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		// ���Ʊ���������
		if (ifDrawCurve)
		{
			mycurve->gencurve();
			mycurve->update();
			mycurve->updategl(curve_lineVAO, curve_lineVBO, curve_pointVAO, curve_pointVBO, instanceVBO);

			glm::mat4 model = glm::mat4(1.0f);

			curve_line_shader.use();
			curve_line_shader.setVec3("color", 1.0, 1.0, 1.0);
			curve_line_shader.setMat4("projection", projection);
			curve_line_shader.setMat4("view", view);
			curve_line_shader.setMat4("model", model);

			glBindVertexArray(curve_lineVAO);
			glDrawArrays(GL_LINE_STRIP, 0, mycurve->samplenum + 1);

			curve_point_shader.use();
			curve_point_shader.setVec3("color", 1.0, 0.0, 0.0);
			curve_point_shader.setFloat("scale", 0.01f);
			curve_point_shader.setMat4("projection", projection);
			curve_point_shader.setMat4("view", view);
			curve_point_shader.setMat4("model", model);

			glBindVertexArray(curve_pointVAO);
			glDrawArraysInstanced(GL_TRIANGLES, 0, 6, mycurve->num);
		}

		// ɨ������
		if (ifDrawSweep)
		{
			float* samples = mycurve->getsamples();
			int size = mycurve->samplenum;
			mysweep->init_curve(samples, size);
			mysweep->set3(rectangle_width, rectangle_length, ellipse_a, ellipse_b, circle_radius);

			sweep_shader.use();
			sweep_shader.setMat4("projection", projection);
			sweep_shader.setMat4("view", view);
			sweep_shader.setMat4("model", glm::mat4(1.0f));
			sweep_shader.setVec3("viewPos", camera.Position);
			sweep_shader.setVec3("lightPos", lightPos);
			sweep_shader.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));

			// ������
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, concreteTexture); // ʹ��֮ǰ���صĻ������������������
			sweep_shader.setInt("diffuseTexture", 0);

			mysweep->draw();
		}

		// ������պ�
		{
			glDepthFunc(GL_LEQUAL); // ������Ҫ����Ⱥ�������ΪGL_LEQUAL����Ϊ���ֵ����Զƽ������ֵ��Ƭ�βŻ�����Ȼ�����ͨ��
			skyboxShader.use();
			view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
			skyboxShader.setMat4("view", view);
			skyboxShader.setMat4("projection", projection);
			// skybox cube
			glBindVertexArray(skyboxVAO);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glBindVertexArray(0);
			glDepthFunc(GL_LESS); // set depth function back to default
		}

		// ��Ⱦ imgui
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// glfw����������������ѯ IO �¼�������/�ͷż����ƶ����ȣ�
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// ����ѡ��һ����Դ��������;����ȡ������������Դ��
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &CeilingVAO);
	glDeleteVertexArrays(1, &FloorVAO);
	glDeleteVertexArrays(1, &RWallVAO);
	glDeleteVertexArrays(1, &LWallVAO);
	glDeleteVertexArrays(1, &FWallVAO);
	glDeleteVertexArrays(1, &lightCubeVAO);
	glDeleteVertexArrays(1, &skyboxVAO);
	glDeleteVertexArrays(1, &areaLightVAO);
	glDeleteVertexArrays(1, &curve_lineVAO);
	glDeleteVertexArrays(1, &curve_pointVAO);
	glDeleteBuffers(1, &VBO1);
	glDeleteBuffers(1, &VBO2);
	glDeleteBuffers(1, &VBO3);
	glDeleteBuffers(1, &VBO4);
	glDeleteBuffers(1, &VBO5);
	glDeleteBuffers(1, &VBO6);
	glDeleteBuffers(1, &skyboxVBO);
	glDeleteBuffers(1, &VBO7);
	glDeleteBuffers(1, &curve_lineVBO);
	glDeleteBuffers(1, &curve_pointVBO);
	glDeleteBuffers(1, &instanceVBO);

	// glfw����ֹ�����������ǰ����� GLFW ��Դ��
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// ��ѯ GLFW �Ƿ���/�ͷ��˸�֡����ؼ���������Ӧ�ķ�Ӧ
//  ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		lockCursor = !lockCursor;
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		areaLightTranslate.z -= 0.001f;
		areaLightTranslate.z = glm::clamp(areaLightTranslate.z, 1.6f, 2.4f);
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		areaLightTranslate.z += 0.001f;
		areaLightTranslate.z = glm::clamp(areaLightTranslate.z, 1.6f, 2.4f);
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		areaLightTranslate.x -= 0.001f;
		areaLightTranslate.x = glm::clamp(areaLightTranslate.x, -0.8f, 0.8f);
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		areaLightTranslate.x += 0.001f;
		areaLightTranslate.x = glm::clamp(areaLightTranslate.x, -0.8f, 0.8f);
	}
	if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS)
	{
		areaLightTranslate.y += 0.001f;
		areaLightTranslate.y = glm::clamp(areaLightTranslate.y, -0.1f, 0.745f);
	}
	if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS)
	{
		areaLightTranslate.y -= 0.001f;
		areaLightTranslate.y = glm::clamp(areaLightTranslate.y, -0.1f, 0.745f);
	}

	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
	{
		twoSided = !twoSided;
	}

	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
	{
		ifDrawCurve = !ifDrawCurve;
	}

	if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)
	{
		ifDrawSweep = !ifDrawSweep;
	}
}

// glfw��ÿ�����ڴ�С�����仯��ͨ������ϵͳ���û�������С��ʱ���˻ص���������ִ��
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// ȷ���������µĴ��ڳߴ�ƥ��;��ע�⣬width��height�����Դ��� Retina ��ʾ����ָ���ĸ߶�
	glViewport(0, 0, width, height);
}

// glfw: ÿ������ƶ�ʱ���ûص����ᱻ����
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // ��ת����Ϊ y ������µ���

	lastX = xpos;
	lastY = ypos;

	if (!lockCursor)
	{
		camera.ProcessMouseMovement(xoffset, yoffset);
	}
}

// glfw:ÿ�������ֹ���ʱ���ûص����ᱻ����
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (!lockCursor)
	{
		camera.ProcessMouseScroll(static_cast<float>(yoffset));
	}
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const* path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

// loads a cubemap texture from 6 individual texture faces
// order:
// +X (right)
// -X (left)
// +Y (top)
// -Y (bottom)
// +Z (front)
// -Z (back)
// -------------------------------------------------------
unsigned int loadCubemap(vector<std::string> faces) // ������������ͼ����
{
	unsigned int textureID;						   // ����ID
	glGenTextures(1, &textureID);				   // ��������
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID); // ������

	int width, height, nrChannels; // ͼƬ��ȡ��߶ȡ���ɫͨ����
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0); // ����ͼƬ
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data); // ��������
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	   // ����������˷�ʽ
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	   // ����������˷�ʽ
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // ���������Ʒ�ʽ
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // ���������Ʒ�ʽ
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE); // ���������Ʒ�ʽ

	return textureID; // ��������ID
}

unsigned int loadMTexture()
{
	unsigned int texture = 0;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 64, 64,
		0, GL_RGBA, GL_FLOAT, LTC1);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);
	return texture;
}

unsigned int loadLUTTexture()
{
	unsigned int texture = 0;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 64, 64,
		0, GL_RGBA, GL_FLOAT, LTC2);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);
	return texture;
}