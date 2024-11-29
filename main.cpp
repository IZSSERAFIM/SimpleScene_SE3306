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

#include "midpoint_disp_terrain.hpp"
#include "koch_snowflake.hpp"
#include "photon.hpp"

#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
unsigned int loadTexture(char const* path);
unsigned int loadCubemap(vector<std::string> faces); // ������������ͼ����
void processInput(GLFWwindow* window);
void generateblackboardVertices();
std::vector<float> generateLightStripVertices(glm::vec3 coneApex, glm::vec3 coneBase, float baseRadius, int numTurns, int pointsPerTurn);

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
glm::vec3 lightPos(0.0f, 0.75f, 1.65f);
glm::vec3 cubePos(0.0f, 0.3f, 2.0f);

// imgui����
const char* glsl_version = "#version 330";
ImVec4 clear_color = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
ImVec4 light_color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
ImVec4 celling_color = ImVec4(0.5, 0.5f, 0.5f, 1.0f);
ImVec4 floor_color = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
ImVec4 left_color = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
ImVec4 front_color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
ImVec4 right_color = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
ImVec4 windmill_color = ImVec4(0.314f, 0.902f, 0.192f, 1.0f);
float scale = 2.0f;

// �ڰ�����
std::vector<float> blackboardVertices;
glm::vec3 blackboardSize(1.0f, 0.6f, 0.1f); // Width, height, depth
glm::vec3 blackboardPosition(0.0f, 0.3f, 1.51f); // Slightly in front of the front wall

// ��������
glm::vec3 tablePosition(-0.2f, -0.19f, 2.2f);

// ʥ��������
glm::vec3 christmas_treePosition(0.0f, 0.13f, 2.1f);

// �糵����
// �糵ͼ�εĶ�������
std::vector<float> windmillVertices = {
	//1
	0.0f, 0.0f, 0.0f,
	0.0f, 0.1f, 0.0f,
	0.05f, 0.05f, 0.0f,
	//2
	0.0f, 0.0f, 0.0f,
	0.1f, 0.1f, 0.0f,
	0.1f, 0.0f, 0.0f,
	//3
	0.0f, 0.0f, 0.0f,
	0.05f, -0.05f, 0.0f,
	0.1f, 0.0f, 0.0f,
	//4
	0.0f, 0.0f, 0.0f,
	0.1f, -0.1f, 0.0f,
	0.0f, -0.1f, 0.0f,
	//5
	0.0f, 0.0f, 0.0f,
	0.0f, -0.1f, 0.0f,
	-0.05f, -0.05f, 0.0f,
	//6
	0.0f, 0.0f, 0.0f,
	-0.1f, -0.1f, 0.0f,
	-0.1f, 0.0f, 0.0f,
	//7
	0.0f, 0.0f, 0.0f,
	-0.05f, 0.05f, 0.0f,
	-0.1f, 0.0f, 0.0f,
	//8
	0.0f, 0.0f, 0.0f,
	-0.1f, 0.1f, 0.0f,
	0.0f, 0.1f, 0.0f,
};
float rotateSpeed = 1.0f;
bool ifRotate = false;
bool drawWindmill = false;
float currentAngle = 0.0f;
bool drawColor = false;

float skyboxVertices[] = {
	// positions          
	-1.0f,  1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	-1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f
};

bool drawSnowflake = false;

bool isChristmasTreeLighted = false;

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

	//imgui init
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
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
	Shader blackBoardShader("board_vertex.glsl", "board_fragment.glsl");
	Shader windmillShader("windmill_vertex.glsl", "windmill_fragment.glsl");
	Shader table_modelShader("table_model_vertex.glsl", "table_model_fragment.glsl");
	Shader christmas_tree_modelShader("christmas_tree_model_vertex.glsl", "christmas_tree_model_fragment.glsl");
	Shader skyboxShader("skybox_vertex.glsl", "skybox_fragment.glsl");
	Shader heightMapShader("terrain_vertex.glsl", "terrain_fragment.glsl");
	Shader lightStripShader("light_strip_vertex.glsl", "light_strip_fragment.glsl");
	Shader photonShader("photon_vertex.glsl", "photon_fragment.glsl");

	// load models
	// -----------
	Model tableModel("./table/table.obj");
	Model christmas_treeModel("./christmas_tree/christmas_tree.obj");

	// ͳһ�����õ���������Ϣ(ÿһ��ǰ��������Ϊ������꣬������Ϊ������)
	// ------------------------------------------------------------------
	float vertices[] = {
		// ������ĺ���
		-0.5f * scale, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f * scale, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f * scale,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f * scale,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f * scale,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f * scale, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		// �����������
		-0.5f * scale, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		 0.5f * scale, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		 0.5f * scale,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		 0.5f * scale,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		-0.5f * scale,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		-0.5f * scale, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		// �����������
		-0.5f * scale,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f * scale,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f * scale, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f * scale, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f * scale, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f * scale,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		// �����������
		 0.5f * scale,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f * scale,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f * scale, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f * scale, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f * scale, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f * scale,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		 // ������ĵ���
		-0.5f * scale, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f * scale, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f * scale, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f * scale, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f * scale, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f * scale, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		// ������Ķ���
		-0.5f * scale,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f * scale,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f * scale,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f * scale,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f * scale,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f * scale,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
	};

	//��ȡ����ƽ�������
	// ------------------------------------------------------------------
	float CeilingVertices[36];
	std::copy(vertices + 180, vertices + 216, CeilingVertices);
	float FloorVertices[36];
	std::copy(vertices + 144, vertices + 180, FloorVertices);
	float LWallVertices[36];
	std::copy(vertices + 72, vertices + 108, LWallVertices);
	float RWallVertices[36];
	std::copy(vertices + 108, vertices + 144, RWallVertices);
	float FWallVertices[36];
	std::copy(vertices + 0, vertices + 36, FWallVertices);

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
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(0 * sizeof(float)));
		glEnableVertexAttribArray(0);
		// ���뷨����
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
	}

	//����ذ�Ķ�����Ϣ
	// ------------------------------------------------------------------
	unsigned int VBO2, FloorVAO;
	{
		glGenVertexArrays(1, &FloorVAO);
		glGenBuffers(1, &VBO2);

		glBindBuffer(GL_ARRAY_BUFFER, VBO2);
		glBufferData(GL_ARRAY_BUFFER, sizeof(FloorVertices), FloorVertices, GL_STATIC_DRAW);

		glBindVertexArray(FloorVAO);

		// ����λ��
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(0 * sizeof(float)));
		glEnableVertexAttribArray(0);
		// ���뷨����
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
	}

	//������ǽ�Ķ�����Ϣ
	// ------------------------------------------------------------------
	unsigned int VBO3, LWallVAO;
	{
		glGenVertexArrays(1, &LWallVAO);
		glGenBuffers(1, &VBO3);

		glBindBuffer(GL_ARRAY_BUFFER, VBO3);
		glBufferData(GL_ARRAY_BUFFER, sizeof(LWallVertices), LWallVertices, GL_STATIC_DRAW);

		glBindVertexArray(LWallVAO);

		// ����λ��
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(0 * sizeof(float)));
		glEnableVertexAttribArray(0);
		// ���뷨����
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
	}

	//������ǽ�Ķ�����Ϣ
	// ------------------------------------------------------------------
	unsigned int VBO4, RWallVAO;
	{
		glGenVertexArrays(1, &RWallVAO);
		glGenBuffers(1, &VBO4);

		glBindBuffer(GL_ARRAY_BUFFER, VBO4);
		glBufferData(GL_ARRAY_BUFFER, sizeof(RWallVertices), RWallVertices, GL_STATIC_DRAW);

		glBindVertexArray(RWallVAO);

		// ����λ��
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(0 * sizeof(float)));
		glEnableVertexAttribArray(0);
		// ���뷨����
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
	}

	//����ǰǽ�Ķ�����Ϣ
	// ------------------------------------------------------------------
	unsigned int VBO5, FWallVAO;
	{
		glGenVertexArrays(1, &FWallVAO);
		glGenBuffers(1, &VBO5);

		glBindBuffer(GL_ARRAY_BUFFER, VBO5);
		glBufferData(GL_ARRAY_BUFFER, sizeof(FWallVertices), FWallVertices, GL_STATIC_DRAW);

		glBindVertexArray(FWallVAO);

		// ����λ��
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(0 * sizeof(float)));
		glEnableVertexAttribArray(0);
		// ���뷨����
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
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
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
	}

	// �����糵��VAO��VBO
	unsigned int windmillVAO, windmillVBO;
	{
		glGenVertexArrays(1, &windmillVAO);
		glGenBuffers(1, &windmillVBO);

		glBindVertexArray(windmillVAO);
		glBindBuffer(GL_ARRAY_BUFFER, windmillVBO);
		glBufferData(GL_ARRAY_BUFFER, windmillVertices.size() * sizeof(float), windmillVertices.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
	}

	//���úڰ��VAO��VBO
	// ------------------------------------------------------------------
	unsigned int blackboardVAO, blackboardVBO;
	{
		generateblackboardVertices();
		glGenVertexArrays(1, &blackboardVAO);
		glGenBuffers(1, &blackboardVBO);

		glBindVertexArray(blackboardVAO);
		glBindBuffer(GL_ARRAY_BUFFER, blackboardVBO);
		glBufferData(GL_ARRAY_BUFFER, blackboardVertices.size() * sizeof(float), blackboardVertices.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); // λ��
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); // ����
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); // ��������
		glEnableVertexAttribArray(2);
	}

	// skybox VAO
	unsigned int skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);

	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	// ��������
	unsigned int diffuseMap = loadTexture("./textures/blackboard.jpg");
	unsigned int specularMap = loadTexture("./textures/blackboard_specular.png");
	vector<std::string> faces
	{
		"./skybox/right.jpg",
		"./skybox/left.jpg",
		"./skybox/top.jpg",
		"./skybox/bottom.jpg",
		"./skybox/front.jpg",
		"./skybox/back.jpg"
	};
	unsigned int cubemapTexture = loadCubemap(faces);

	// shader configuration
	// --------------------
	blackBoardShader.use();
	blackBoardShader.setInt("material.diffuse", 0);
	blackBoardShader.setInt("material.specular", 1);
	skyboxShader.use();
	skyboxShader.setInt("skybox", 0);

	// terrain
	// ------------------------------------------------------------------
	int width = 32;
	int height = 32;
	float minHeight = 0.0f;
	float maxHeight = 4.0f;
	float roughness = 1.5f;

	std::vector<float> heightmap;
	CreateMidpointDisplacementHeightmap(width, height, minHeight, maxHeight, roughness, heightmap);

	// ��������ĸ߶ȺͲ���
	int numLayers = 10; // �������ɸ�����Ҫ����
	float cylinderHeight = 5.0f; // ������ܸ߶�
	float layerHeightIncrement = (cylinderHeight - maxHeight) / (numLayers - 1); // ÿ��ĸ߶�����

	// Բ�β���
	float cx = -0.5f, cz = -0.5f;  // Բ������
	float r = std::min(width, height) / 2.0f - 1;  // Բ�İ뾶

	// ���ɰ����߶ȵĶ�������
	std::vector<float> cylinder_vertices;
	for (int layer = 0; layer < numLayers; layer++) {
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				// ����λ��
				float x = -height / 2.0f + height * i / (float)(height - 1);
				float z = -width / 2.0f + width * j / (float)(width - 1);

				// Ӧ��Բ��ͶӰ���� x �� z
				float dx = x - cx;
				float dz = z - cz;
				float d = std::sqrt(dx * dx + dz * dz);
				if (d > r) {
					float scale = r / d;
					x = cx + dx * scale;
					z = cz + dz * scale;
				}

				float y;
				if (layer == numLayers - 1) {
					// ���㣬ʹ�ø߶�ͼ�ĸ߶�
					y = cylinderHeight - maxHeight + heightmap[i * width + j]; // ����߶ȼ��ϸ߶�ͼ��ֵ
				}
				else {
					// �����㣬ʹ�þ��ȵĸ߶�
					y = layer * layerHeightIncrement;
				}

				cylinder_vertices.push_back(x);
				cylinder_vertices.push_back(y); // ʹ�õ������ y ֵ
				cylinder_vertices.push_back(z);

				// ��������
				cylinder_vertices.push_back((float)j / (float)(width - 1));
				cylinder_vertices.push_back((float)i / (float)(height - 1));
			}
		}
	}

	// �������Ӹ������������
	std::vector<unsigned int> cylinder_indices;
	int verticesPerLayer = width * height;
	for (int layer = 0; layer < numLayers - 1; layer++) {
		int currentLayerStart = layer * verticesPerLayer;
		int nextLayerStart = (layer + 1) * verticesPerLayer;
		for (int i = 0; i < height - 1; i++) {
			for (int j = 0; j < width - 1; j++) {
				// ��ǰ����ĸ���������
				int current = currentLayerStart + i * width + j;
				int currentRight = current + 1;
				int currentBottom = current + width;
				int currentBottomRight = currentBottom + 1;

				// ��һ��Ķ�Ӧ��������
				int next = nextLayerStart + i * width + j;
				int nextRight = next + 1;
				int nextBottom = next + width;
				int nextBottomRight = nextBottom + 1;

				// ���������ı��Σ����������������
				// ��һ��������
				cylinder_indices.push_back(current);
				cylinder_indices.push_back(next);
				cylinder_indices.push_back(currentRight);
				// �ڶ���������
				cylinder_indices.push_back(currentRight);
				cylinder_indices.push_back(next);
				cylinder_indices.push_back(nextRight);

				// �ڶ��������Σ����ӵ�ǰ�����һ��
				// ������������
				cylinder_indices.push_back(currentRight);
				cylinder_indices.push_back(next);
				cylinder_indices.push_back(currentBottomRight);
				// ���ĸ�������
				cylinder_indices.push_back(currentBottomRight);
				cylinder_indices.push_back(next);
				cylinder_indices.push_back(nextBottomRight);

				// �����ײ������ڵ�һ�㴦��
				if (layer == 0) {
					cylinder_indices.push_back(current);
					cylinder_indices.push_back(currentRight);
					cylinder_indices.push_back(currentBottom);

					cylinder_indices.push_back(currentRight);
					cylinder_indices.push_back(currentBottomRight);
					cylinder_indices.push_back(currentBottom);
				}

				// �����������������һ�㴦��
				if (layer == numLayers - 2) {
					cylinder_indices.push_back(next);
					cylinder_indices.push_back(nextBottom);
					cylinder_indices.push_back(nextRight);

					cylinder_indices.push_back(nextRight);
					cylinder_indices.push_back(nextBottom);
					cylinder_indices.push_back(nextBottomRight);
				}
			}
		}
	}

	std::cout << "Generated " << cylinder_indices.size() << " indices" << std::endl;

	// ���� VAO��VBO �� IBO
	unsigned int terrainVAO, terrainVBO, terrainIBO;
	glGenVertexArrays(1, &terrainVAO);
	glBindVertexArray(terrainVAO);

	glGenBuffers(1, &terrainVBO);
	glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);
	glBufferData(GL_ARRAY_BUFFER, cylinder_vertices.size() * sizeof(float), cylinder_vertices.data(), GL_STATIC_DRAW);

	// ����λ������
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// ������������
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &terrainIBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrainIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, cylinder_indices.size() * sizeof(unsigned int), cylinder_indices.data(), GL_STATIC_DRAW);

	// ��������
	unsigned int gTextureHeight0 = loadTexture("./textures/rare.jpg");
	unsigned int gTextureHeight1 = loadTexture("./textures/medium_rare.jpg");
	unsigned int gTextureHeight2 = loadTexture("./textures/medium_well.png");
	unsigned int gTextureHeight3 = loadTexture("./textures/well_done.png");

	heightMapShader.use();
	heightMapShader.setInt("gTextureHeight0", 0);
	heightMapShader.setInt("gTextureHeight1", 1);
	heightMapShader.setInt("gTextureHeight2", 2);
	heightMapShader.setInt("gTextureHeight3", 3);

	// ѩ��
	// ����ѩ��ϵͳ
	Shader snowflakeShader("koch_snowflake_vertex.glsl", "koch_snowflake_fragment.glsl");
	KochSnowflake snowflakeSystem;
	snowflakeSystem.init();

	// �ʵƴ�
	// ���ɵƴ�����
	// Բ׶����
	glm::vec3 coneApex(0.0f, 0.34f, 2.1f);
	// Բ׶����Բ��
	glm::vec3 coneBase(0.0f, 0.13f, 2.1f);
	// Բ׶����뾶
	float baseRadius = 0.08f;
	int numTurns = 20;
	int pointsPerTurn = 100;
	std::vector<float> lightStripVertices = generateLightStripVertices(coneApex, coneBase, baseRadius, numTurns, pointsPerTurn);
	unsigned int lightStripVBO, lightStripVAO;
	glGenVertexArrays(1, &lightStripVAO);
	glGenBuffers(1, &lightStripVBO);
	glBindVertexArray(lightStripVAO);
	glBindBuffer(GL_ARRAY_BUFFER, lightStripVBO);
	glBufferData(GL_ARRAY_BUFFER, lightStripVertices.size() * sizeof(float), &lightStripVertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// ������
	PhotonSystem photonSystem(10000, glm::vec3(0.0f, 0.32f, 2.1f));

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
		ImGui::Begin("panel");// Create a window called "panel" and append into it.
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
		ImGui::Text("Cornell bos is scaled by %f times", scale);
		ImGui::Checkbox("Lock Cursor(Shortcut: SPACE)", &lockCursor);
		ImGui::SliderFloat("wind mill rotate speed", &rotateSpeed, 0.0f, 10.0f);
		ImGui::ColorEdit3("windmill color", (float*)&windmill_color);
		ImGui::ColorEdit3("light color", (float*)&light_color);
		ImGui::ColorEdit3("celling color", (float*)&celling_color);
		ImGui::ColorEdit3("floor color", (float*)&floor_color);
		ImGui::ColorEdit3("left color", (float*)&left_color);
		ImGui::ColorEdit3("front color", (float*)&front_color);
		ImGui::ColorEdit3("right color", (float*)&right_color);
		ImGui::End();

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

		//�����컨��
		{
			//���ù��ղ���
			lightingShader.setVec3("objectColor", celling_color.x, celling_color.y, celling_color.z);
			lightingShader.setVec3("viewPos", camera.Position);
			lightingShader.setFloat("material.shininess", 32.0f);
			// directional light
			lightingShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
			lightingShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
			lightingShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
			lightingShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
			// point light 1
			lightingShader.setVec3("pointLights[0].position", lightPos);
			lightingShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
			lightingShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
			lightingShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
			lightingShader.setFloat("pointLights[0].constant", 1.0f);
			lightingShader.setFloat("pointLights[0].linear", 0.09f);
			lightingShader.setFloat("pointLights[0].quadratic", 0.032f);
			// spotLight
			lightingShader.setVec3("spotLight.position", camera.Position);
			lightingShader.setVec3("spotLight.direction", camera.Front);
			lightingShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
			lightingShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
			lightingShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
			lightingShader.setFloat("spotLight.constant", 1.0f);
			lightingShader.setFloat("spotLight.linear", 0.09f);
			lightingShader.setFloat("spotLight.quadratic", 0.032f);
			lightingShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
			lightingShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

			// view/projection �任
			lightingShader.setMat4("projection", projection);
			lightingShader.setMat4("view", view);

			// ��������任
			model = glm::translate(model, cubePos);
			model = glm::scale(model, glm::vec3(1.0f));
			lightingShader.setMat4("model", model);

			// ��Ⱦ
			glBindVertexArray(CeilingVAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		// ���Ƶذ�
		{
			//���ù��ղ���
			lightingShader.setVec3("objectColor", floor_color.x, floor_color.y, floor_color.z);
			lightingShader.setVec3("viewPos", camera.Position);
			lightingShader.setFloat("material.shininess", 32.0f);
			// directional light
			lightingShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
			lightingShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
			lightingShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
			lightingShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
			// point light 1
			lightingShader.setVec3("pointLights[0].position", lightPos);
			lightingShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
			lightingShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
			lightingShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
			lightingShader.setFloat("pointLights[0].constant", 1.0f);
			lightingShader.setFloat("pointLights[0].linear", 0.09f);
			lightingShader.setFloat("pointLights[0].quadratic", 0.032f);
			// spotLight
			lightingShader.setVec3("spotLight.position", camera.Position);
			lightingShader.setVec3("spotLight.direction", camera.Front);
			lightingShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
			lightingShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
			lightingShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
			lightingShader.setFloat("spotLight.constant", 1.0f);
			lightingShader.setFloat("spotLight.linear", 0.09f);
			lightingShader.setFloat("spotLight.quadratic", 0.032f);
			lightingShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
			lightingShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

			// view/projection �任
			lightingShader.setMat4("projection", projection);
			lightingShader.setMat4("view", view);

			// ��������任
			model = glm::mat4(1.0f);
			model = glm::translate(model, cubePos);
			model = glm::scale(model, glm::vec3(1.0f));
			lightingShader.setMat4("model", model);

			// ��Ⱦ
			glBindVertexArray(FloorVAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		// ������ǽ
		{
			//���ù��ղ���
			lightingShader.setVec3("objectColor", left_color.x, left_color.y, left_color.z);
			lightingShader.setVec3("viewPos", camera.Position);
			lightingShader.setFloat("material.shininess", 32.0f);
			// directional light
			lightingShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
			lightingShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
			lightingShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
			lightingShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
			// point light 1
			lightingShader.setVec3("pointLights[0].position", lightPos);
			lightingShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
			lightingShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
			lightingShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
			lightingShader.setFloat("pointLights[0].constant", 1.0f);
			lightingShader.setFloat("pointLights[0].linear", 0.09f);
			lightingShader.setFloat("pointLights[0].quadratic", 0.032f);
			// spotLight
			lightingShader.setVec3("spotLight.position", camera.Position);
			lightingShader.setVec3("spotLight.direction", camera.Front);
			lightingShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
			lightingShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
			lightingShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
			lightingShader.setFloat("spotLight.constant", 1.0f);
			lightingShader.setFloat("spotLight.linear", 0.09f);
			lightingShader.setFloat("spotLight.quadratic", 0.032f);
			lightingShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
			lightingShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

			// view/projection �任
			lightingShader.setMat4("projection", projection);
			lightingShader.setMat4("view", view);

			// ��������任
			model = glm::mat4(1.0f);
			model = glm::translate(model, cubePos);
			model = glm::scale(model, glm::vec3(1.0f));
			lightingShader.setMat4("model", model);

			// ��Ⱦ
			glBindVertexArray(LWallVAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		// ������ǽ
		{
			//���ù��ղ���
			lightingShader.setVec3("objectColor", right_color.x, right_color.y, right_color.z);
			lightingShader.setVec3("viewPos", camera.Position);
			lightingShader.setFloat("material.shininess", 32.0f);
			// directional light
			lightingShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
			lightingShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
			lightingShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
			lightingShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
			// point light 1
			lightingShader.setVec3("pointLights[0].position", lightPos);
			lightingShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
			lightingShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
			lightingShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
			lightingShader.setFloat("pointLights[0].constant", 1.0f);
			lightingShader.setFloat("pointLights[0].linear", 0.09f);
			lightingShader.setFloat("pointLights[0].quadratic", 0.032f);
			// spotLight
			lightingShader.setVec3("spotLight.position", camera.Position);
			lightingShader.setVec3("spotLight.direction", camera.Front);
			lightingShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
			lightingShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
			lightingShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
			lightingShader.setFloat("spotLight.constant", 1.0f);
			lightingShader.setFloat("spotLight.linear", 0.09f);
			lightingShader.setFloat("spotLight.quadratic", 0.032f);
			lightingShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
			lightingShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

			// view/projection �任
			lightingShader.setMat4("projection", projection);
			lightingShader.setMat4("view", view);

			// ��������任
			model = glm::mat4(1.0f);
			model = glm::translate(model, cubePos);
			model = glm::scale(model, glm::vec3(1.0f));
			lightingShader.setMat4("model", model);

			// ��Ⱦ
			glBindVertexArray(RWallVAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		// ����ǰǽ
		{
			//���ù��ղ���
			lightingShader.setVec3("objectColor", front_color.x, front_color.y, front_color.z);
			lightingShader.setVec3("viewPos", camera.Position);
			lightingShader.setFloat("material.shininess", 32.0f);
			// directional light
			lightingShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
			lightingShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
			lightingShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
			lightingShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
			// point light 1
			lightingShader.setVec3("pointLights[0].position", lightPos);
			lightingShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
			lightingShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
			lightingShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
			lightingShader.setFloat("pointLights[0].constant", 1.0f);
			lightingShader.setFloat("pointLights[0].linear", 0.09f);
			lightingShader.setFloat("pointLights[0].quadratic", 0.032f);
			// spotLight
			lightingShader.setVec3("spotLight.position", camera.Position);
			lightingShader.setVec3("spotLight.direction", camera.Front);
			lightingShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
			lightingShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
			lightingShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
			lightingShader.setFloat("spotLight.constant", 1.0f);
			lightingShader.setFloat("spotLight.linear", 0.09f);
			lightingShader.setFloat("spotLight.quadratic", 0.032f);
			lightingShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
			lightingShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

			// view/projection �任
			lightingShader.setMat4("projection", projection);
			lightingShader.setMat4("view", view);

			// ��������任
			model = glm::mat4(1.0f);
			model = glm::translate(model, cubePos);
			model = glm::scale(model, glm::vec3(1.0f));
			lightingShader.setMat4("model", model);

			// ��Ⱦ
			glBindVertexArray(FWallVAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		// ���ƵƷ���
		{
			lightCubeShader.use();
			lightCubeShader.setMat4("projection", projection);
			lightCubeShader.setMat4("view", view);
			model = glm::mat4(1.0f);
			model = glm::translate(model, lightPos);
			model = glm::scale(model, glm::vec3(0.1f)); // a smaller cube
			lightCubeShader.setMat4("model", model);
			lightCubeShader.setVec4("ourColor", glm::vec4(light_color.x, light_color.y, light_color.z, 1.0f));


			glBindVertexArray(lightCubeVAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		// ���ƺڰ�
		{
			// ��������Ԫ0���󶨺ڰ�����
			blackBoardShader.use();
			blackBoardShader.setVec3("viewPos", camera.Position);
			blackBoardShader.setFloat("material.shininess", 32.0f);
			// directional light
			blackBoardShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
			blackBoardShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
			blackBoardShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
			blackBoardShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
			// point light 1
			blackBoardShader.setVec3("pointLights[0].position", lightPos);
			blackBoardShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
			blackBoardShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
			blackBoardShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
			blackBoardShader.setFloat("pointLights[0].constant", 1.0f);
			blackBoardShader.setFloat("pointLights[0].linear", 0.09f);
			blackBoardShader.setFloat("pointLights[0].quadratic", 0.032f);
			// spotLight
			blackBoardShader.setVec3("spotLight.position", camera.Position);
			blackBoardShader.setVec3("spotLight.direction", camera.Front);
			blackBoardShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
			blackBoardShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
			blackBoardShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
			blackBoardShader.setFloat("spotLight.constant", 1.0f);
			blackBoardShader.setFloat("spotLight.linear", 0.09f);
			blackBoardShader.setFloat("spotLight.quadratic", 0.032f);
			blackBoardShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
			blackBoardShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));
			// view/projection transformations
			glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
			glm::mat4 view = camera.GetViewMatrix();
			blackBoardShader.setMat4("projection", projection);
			blackBoardShader.setMat4("view", view);

			// world transformation
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, blackboardPosition);
			model = glm::scale(model, blackboardSize);
			blackBoardShader.setMat4("model", model);

			// bind diffuse map
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, diffuseMap);
			// bind specular map
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, specularMap);

			glBindVertexArray(blackboardVAO);
			glDrawArrays(GL_TRIANGLES, 0, blackboardVertices.size() / 8);
		}

		// ���Ʒ糵
		if (drawWindmill)
		{
			windmillShader.use();
			windmillShader.setVec3("objectColor", 1.0f, 1.0f, 1.0f); // ��ɫ
			windmillShader.setMat4("projection", projection);
			windmillShader.setMat4("view", view);
			windmillShader.setVec3("lightColor", light_color.x, light_color.y, light_color.z);
			windmillShader.setVec3("lightPos", lightPos);
			windmillShader.setVec3("viewPos", camera.Position);

			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(blackboardPosition.x, blackboardPosition.y, blackboardPosition.z + 0.01f));

			if (ifRotate) {
				currentAngle += 50.0f * deltaTime * rotateSpeed; // Update the angle only if rotating
			}
			model = glm::rotate(model, glm::radians(currentAngle), glm::vec3(0.0f, 0.0f, 1.0f));

			windmillShader.setMat4("model", model);

			glBindVertexArray(windmillVAO);
			glLineWidth(2.5f);
			if (drawColor) {
				windmillShader.setVec3("objectColor", windmill_color.x, windmill_color.y, windmill_color.z);
				glDrawArrays(GL_TRIANGLES, 0, windmillVertices.size() / 3);
				windmillShader.setVec3("objectColor", 1.0f, 1.0f, 1.0f); // ��ɫ
				glDrawArrays(GL_LINE_LOOP, 0, windmillVertices.size() / 3);
			}
			else
			{
				glDrawArrays(GL_LINE_LOOP, 0, windmillVertices.size() / 3);
			}
		}

		// ��Ⱦ����ģ��
		{
			table_modelShader.use();
			table_modelShader.setVec3("viewPos", camera.Position);

			// view/projection transformations
			glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
			glm::mat4 view = camera.GetViewMatrix();
			table_modelShader.setMat4("projection", projection);
			table_modelShader.setMat4("view", view);

			// render the loaded model
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, tablePosition);
			model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));	// it's a bit too big for our scene, so scale it down
			model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			table_modelShader.setMat4("model", model);

			//
			// directional light
			table_modelShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
			table_modelShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
			table_modelShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
			table_modelShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
			// point light 1
			table_modelShader.setVec3("pointLights[0].position", lightPos);
			table_modelShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
			table_modelShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
			table_modelShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
			table_modelShader.setFloat("pointLights[0].constant", 1.0f);
			table_modelShader.setFloat("pointLights[0].linear", 0.09f);
			table_modelShader.setFloat("pointLights[0].quadratic", 0.032f);
			// spotLight
			table_modelShader.setVec3("spotLight.position", camera.Position);
			table_modelShader.setVec3("spotLight.direction", camera.Front);
			table_modelShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
			table_modelShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
			table_modelShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
			table_modelShader.setFloat("spotLight.constant", 1.0f);
			table_modelShader.setFloat("spotLight.linear", 0.09f);
			table_modelShader.setFloat("spotLight.quadratic", 0.032f);
			table_modelShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
			table_modelShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

			tableModel.Draw(table_modelShader);
		}

		float factor = 15.0f * (float)glfwGetTime();
		// ��Ⱦʥ����ģ��
		{
			christmas_tree_modelShader.use();

			christmas_tree_modelShader.setVec3("lightAmbient", 0.5f * glm::vec3(1.0f, 1.0f, 1.0f));
			christmas_tree_modelShader.setVec3("lightDiffuse", 0.2f * glm::vec3(1.0f, 1.0f, 1.0f));
			christmas_tree_modelShader.setVec3("lightSpecular", glm::vec3(1.0f, 1.0f, 1.0f));
			christmas_tree_modelShader.setVec3("lightPos", glm::vec3(0.0f, 10.0f, 0.0f));
			christmas_tree_modelShader.setVec3("viewPos", camera.Position);

			//// view/projection transformations
			glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
			glm::mat4 view = camera.GetViewMatrix();
			christmas_tree_modelShader.setMat4("projection", projection);
			christmas_tree_modelShader.setMat4("view", view);

			//// render the loaded model
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, christmas_treePosition);
			model = glm::rotate(model, glm::radians(factor), glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));	// it's a bit too big for our scene, so scale it down
			christmas_tree_modelShader.setMat4("model", model);

			christmas_treeModel.Draw(christmas_tree_modelShader);
		}

		// ������պ�
		{
			glDepthFunc(GL_LEQUAL);  // ������Ҫ����Ⱥ�������ΪGL_LEQUAL����Ϊ���ֵ����Զƽ������ֵ��Ƭ�βŻ�����Ȼ�����ͨ��
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

		// ���Ƶ���
		{
			// ������ɫ�������� uniform
			heightMapShader.use();
			heightMapShader.setFloat("gMinHeight", minHeight);
			heightMapShader.setFloat("gMaxHeight", maxHeight + cylinderHeight - maxHeight);

			// ��ͼ��ͶӰ����
			glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100000.0f);
			glm::mat4 view = camera.GetViewMatrix();
			heightMapShader.setMat4("projection", projection);
			heightMapShader.setMat4("view", view);

			// ģ�;���
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(christmas_treePosition.x, christmas_treePosition.y - 0.02f, christmas_treePosition.z));
			model = glm::rotate(model, glm::radians(factor), glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
			heightMapShader.setMat4("model", model);

			// ������
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, gTextureHeight0);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, gTextureHeight1);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, gTextureHeight2);
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, gTextureHeight3);

			// ���Ƶ���
			glBindVertexArray(terrainVAO);
			glDrawElements(GL_TRIANGLES, cylinder_indices.size(), GL_UNSIGNED_INT, 0);
		}

		// ��ȾKochѩ��
		if (drawSnowflake)
		{
			snowflakeShader.use();
			glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
			glm::mat4 view = camera.GetViewMatrix();
			snowflakeShader.setMat4("projection", projection);
			snowflakeShader.setMat4("view", view);
			glm::mat4 model = glm::mat4(1.0f);
			snowflakeShader.setMat4("model", model);
			snowflakeSystem.update(deltaTime);
			snowflakeSystem.render(snowflakeShader);
		}

		if (isChristmasTreeLighted)
		{
			lightStripShader.use();
			glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
			glm::mat4 view = camera.GetViewMatrix();
			lightStripShader.setMat4("projection", projection);
			lightStripShader.setMat4("view", view);
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(christmas_treePosition.x, christmas_treePosition.y - 0.13f, christmas_treePosition.z - 2.1f));
			lightStripShader.setMat4("model", model);
			lightStripShader.setFloat("factor", factor);
			glBindVertexArray(lightStripVAO);
			glLineWidth(3.0f);
			glDrawArrays(GL_LINE_STRIP, 0, lightStripVertices.size() / 3);
		}

		if (isChristmasTreeLighted)
		{
			photonShader.use();
			glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
			glm::mat4 view = camera.GetViewMatrix();
			photonShader.setMat4("projection", projection);
			photonShader.setMat4("view", view);
			glm::mat4 model = glm::mat4(1.0f);
			photonShader.setMat4("model", model);
			photonSystem.update(deltaTime);
			photonSystem.render(photonShader);
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
	glDeleteVertexArrays(1, &blackboardVAO);
	glDeleteVertexArrays(1, &windmillVAO);
	glDeleteVertexArrays(1, &lightStripVAO);
	glDeleteBuffers(1, &VBO1);
	glDeleteBuffers(1, &VBO2);
	glDeleteBuffers(1, &VBO3);
	glDeleteBuffers(1, &VBO4);
	glDeleteBuffers(1, &VBO5);
	glDeleteBuffers(1, &VBO6);
	glDeleteBuffers(1, &blackboardVBO);
	glDeleteBuffers(1, &windmillVBO);
	glDeleteBuffers(1, &lightStripVBO);
	glDeleteTextures(1, &diffuseMap);
	glDeleteTextures(1, &specularMap);

	// glfw����ֹ�����������ǰ����� GLFW ��Դ��
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

//��ѯ GLFW �Ƿ���/�ͷ��˸�֡����ؼ���������Ӧ�ķ�Ӧ
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		lockCursor = !lockCursor;
	}

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);

	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
	{
		drawWindmill = !drawWindmill;
	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		ifRotate = !ifRotate;
	}

	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
	{
		if (!drawColor) {
			drawColor = true;
		}
		else
		{
			// Generate random color
			windmill_color.x = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
			windmill_color.y = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
			windmill_color.z = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		}
	}

	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	{
		drawSnowflake = !drawSnowflake;
	}

	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
	{
		isChristmasTreeLighted = !isChristmasTreeLighted;
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
	unsigned int textureID; // ����ID
	glGenTextures(1, &textureID); // ��������
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
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // ����������˷�ʽ
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // ����������˷�ʽ
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // ���������Ʒ�ʽ
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // ���������Ʒ�ʽ
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE); // ���������Ʒ�ʽ

	return textureID; // ��������ID
}

//���ɺڰ�Ķ�����Ϣ
void generateblackboardVertices()
{
	float width = blackboardSize.x;
	float height = blackboardSize.y;
	float depth = blackboardSize.z;

	// Front face with texture coordinates
	blackboardVertices = {
		// Positions          // Normals         // Texture Coords
		// Front face
		-width / 2, -height / 2,  depth / 2,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
		 width / 2, -height / 2,  depth / 2,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
		 width / 2,  height / 2,  depth / 2,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
		 width / 2,  height / 2,  depth / 2,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
		-width / 2,  height / 2,  depth / 2,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,
		-width / 2, -height / 2,  depth / 2,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,

		// Back face
		-width / 2, -height / 2, -depth / 2,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
		 width / 2, -height / 2, -depth / 2,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
		 width / 2,  height / 2, -depth / 2,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		 width / 2,  height / 2, -depth / 2,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		-width / 2,  height / 2, -depth / 2,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
		-width / 2, -height / 2, -depth / 2,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

		// Left face
		-width / 2,  height / 2,  depth / 2, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-width / 2,  height / 2, -depth / 2, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		-width / 2, -height / 2, -depth / 2, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		-width / 2, -height / 2, -depth / 2, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		-width / 2, -height / 2,  depth / 2, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		-width / 2,  height / 2,  depth / 2, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,

		// Right face
		 width / 2,  height / 2,  depth / 2,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		 width / 2,  height / 2, -depth / 2,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 width / 2, -height / 2, -depth / 2,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		 width / 2, -height / 2, -depth / 2,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		 width / 2, -height / 2,  depth / 2,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		 width / 2,  height / 2,  depth / 2,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,

		 // Bottom face
		 -width / 2, -height / 2, -depth / 2,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
		  width / 2, -height / 2, -depth / 2,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		  width / 2, -height / 2,  depth / 2,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
		  width / 2, -height / 2,  depth / 2,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
		 -width / 2, -height / 2,  depth / 2,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
		 -width / 2, -height / 2, -depth / 2,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,

		 // Top face
		 -width / 2,  height / 2, -depth / 2,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
		  width / 2,  height / 2, -depth / 2,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
		  width / 2,  height / 2,  depth / 2,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		  width / 2,  height / 2,  depth / 2,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		 -width / 2,  height / 2,  depth / 2,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
		 -width / 2,  height / 2, -depth / 2,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
	};
}

std::vector<float> generateLightStripVertices(glm::vec3 coneApex, glm::vec3 coneBase, float baseRadius, int numTurns, int pointsPerTurn) {
	std::vector<float> vertices;
	float height = glm::distance(coneApex, coneBase); // Բ׶�߶�
	glm::vec3 coneAxis = glm::normalize(coneApex - coneBase); // Բ׶�᷽���������ӻ���ָ�򶥵�

	// ����һ�����ⲻƽ���� coneAxis ������
	glm::vec3 arbitraryVec = glm::vec3(0.0f, 1.0f, 0.0f);
	if (glm::abs(glm::dot(coneAxis, arbitraryVec)) > 0.99f) {
		arbitraryVec = glm::vec3(1.0f, 0.0f, 0.0f);
	}

	// ���㴹ֱ�� coneAxis ������������λ���� u �� v
	glm::vec3 u = glm::normalize(glm::cross(coneAxis, arbitraryVec));
	glm::vec3 v = glm::cross(coneAxis, u);

	int totalPoints = numTurns * pointsPerTurn;
	for (int i = 0; i < totalPoints; ++i) {
		// ��ǰ�Ƕ�
		float theta = 2.0f * glm::pi<float>() * i / pointsPerTurn;
		// ��ǰ�߶�
		float h = height * i / totalPoints;
		// ��ǰ�뾶
		float radius = (1.0f - h / height) * baseRadius;
		// ��ǰ���Բ��ƫ�ƣ��ڴ�ֱ�� coneAxis ��ƽ����
		glm::vec3 circularOffset = radius * (glm::cos(theta) * u + glm::sin(theta) * v);
		// ���㶥��λ��
		glm::vec3 position = coneBase + h * coneAxis + circularOffset;
		// ������λ����ӵ�����������
		vertices.push_back(position.x);
		vertices.push_back(position.y);
		vertices.push_back(position.z);
	}
	return vertices;
}