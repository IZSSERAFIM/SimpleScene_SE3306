#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "camera.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void generateChalkboardVertices();
void generateFrameVertices();
void generateSnowflakes(int count);
void updateSnowflakes();
void updateBallPosition();
std::vector<float> generateSphereVertices(float radius, int sectorCount, int stackCount);

// 窗口设置
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 960;

// 摄像机设置
Camera camera(glm::vec3(0.0f, 0.3f, 3.3f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
bool lockCursor = false;

// 时间设置
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// 光照设置
glm::vec3 lightPos(0.0f, 0.75f, 1.65f);
glm::vec3 cubePos(0.0f, 0.3f, 2.0f);

// imgui设置
const char* glsl_version = "#version 330";
ImVec4 clear_color = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
ImVec4 light_color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
ImVec4 celling_color = ImVec4(0.5, 0.5f, 0.5f, 1.0f);
ImVec4 floor_color = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
ImVec4 left_color = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
ImVec4 front_color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
ImVec4 right_color = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
ImVec4 windmill_color = ImVec4(0.314f, 0.902f, 0.192f, 1.0f);
ImVec4 ball_color = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
float scale = 2.0f;

// 黑板设置
std::vector<float> chalkboardVertices;
glm::vec3 chalkboardSize(1.0f, 0.6f, 0.05f); // Width, height, depth
glm::vec3 chalkboardPosition(0.0f, 0.3f, 1.51f); // Slightly in front of the front wall
// 定义边框的顶点，包括纹理坐标
std::vector<float> frameVertices;
glm::vec3 frameSize(1.05f, 0.65f, 0.05f); // 比黑板稍大
glm::vec3 frameThickness(0.05f, 0.05f, 0.05f); // 边框的厚度

// 风车设置
// 风车图形的顶点数据
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

// Snowflake structure
struct Snowflake {
	float x, y, z; // Position of the snowflake
	float size; // Size of the snowflake
	float speed; // Falling speed of the snowflake
};
// Vector to hold snowflakes
std::vector<Snowflake> snowflakes;
bool drawSnow = true;

struct Ball {
	glm::vec3 position;
	glm::vec3 velocity;
	float radius;
};
Ball ball = {
		glm::vec3(0.0f, 0.3f, 2.0f),  // Initial position at the center
		glm::vec3(0.1f, 0.2f, 0.3f), // Initial velocity
		0.3f // Ball radius
};
bool drawBall = true;
void updateBallPosition(Ball& ball);

int main()
{
	// 初始化并配置glfw
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw创建窗口
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

	// 告诉 GLFW 捕获我们的鼠标
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	// glad：加载所有 OpenGL 函数指针
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

	// 配置全局 OpenGL 状态
	// -----------------------------
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_PROGRAM_POINT_SIZE);

	// 编译shader操作
	// ------------------------------------
	Shader lightingShader("lighting_vertex.glsl", "lighting_fragment.glsl");
	Shader lightCubeShader("lightcube_vertex.glsl", "lightcube_fragment.glsl");
	Shader textureShader("board_vertex.glsl", "board_fragment.glsl");
	Shader snowflakeShader("snowflake_vertex.glsl", "snowflake_fragment.glsl");
	Shader ballShader("ball_vertex.glsl", "ball_fragment.glsl");

	// 统一设置用到的坐标信息(每一行前三个数字为点的坐标，后三个为法向量)
	// ------------------------------------------------------------------
	float vertices[] = {
		// 立方体的后面
		-0.5f * scale, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f * scale, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f * scale,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f * scale,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f * scale,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f * scale, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		// 立方体的正面
		-0.5f * scale, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		 0.5f * scale, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		 0.5f * scale,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		 0.5f * scale,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		-0.5f * scale,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		-0.5f * scale, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		// 立方体的左面
		-0.5f * scale,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f * scale,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f * scale, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f * scale, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f * scale, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f * scale,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		// 立方体的右面
		 0.5f * scale,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f * scale,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f * scale, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f * scale, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f * scale, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f * scale,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		 // 立方体的底面
		-0.5f * scale, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f * scale, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f * scale, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f * scale, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f * scale, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f * scale, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		// 立方体的顶面
		-0.5f * scale,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f * scale,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f * scale,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f * scale,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f * scale,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f * scale,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
	};

	//获取各个平面的数据
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

	// 载入天花板的顶点信息
	// ------------------------------------------------------------------
	unsigned int VBO1, CeilingVAO;
	{
		glGenVertexArrays(1, &CeilingVAO);
		glGenBuffers(1, &VBO1);

		glBindBuffer(GL_ARRAY_BUFFER, VBO1);
		glBufferData(GL_ARRAY_BUFFER, sizeof(CeilingVertices), CeilingVertices, GL_STATIC_DRAW);

		glBindVertexArray(CeilingVAO);

		// 载入位置
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(0 * sizeof(float)));
		glEnableVertexAttribArray(0);
		// 载入法向量
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
	}

	//载入地板的顶点信息
	// ------------------------------------------------------------------
	unsigned int VBO2, FloorVAO;
	{
		glGenVertexArrays(1, &FloorVAO);
		glGenBuffers(1, &VBO2);

		glBindBuffer(GL_ARRAY_BUFFER, VBO2);
		glBufferData(GL_ARRAY_BUFFER, sizeof(FloorVertices), FloorVertices, GL_STATIC_DRAW);

		glBindVertexArray(FloorVAO);

		// 载入位置
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(0 * sizeof(float)));
		glEnableVertexAttribArray(0);
		// 载入法向量
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
	}

	//载入左墙的顶点信息
	// ------------------------------------------------------------------
	unsigned int VBO3, LWallVAO;
	{
		glGenVertexArrays(1, &LWallVAO);
		glGenBuffers(1, &VBO3);

		glBindBuffer(GL_ARRAY_BUFFER, VBO3);
		glBufferData(GL_ARRAY_BUFFER, sizeof(LWallVertices), LWallVertices, GL_STATIC_DRAW);

		glBindVertexArray(LWallVAO);

		// 载入位置
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(0 * sizeof(float)));
		glEnableVertexAttribArray(0);
		// 载入法向量
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
	}

	//载入右墙的顶点信息
	// ------------------------------------------------------------------
	unsigned int VBO4, RWallVAO;
	{
		glGenVertexArrays(1, &RWallVAO);
		glGenBuffers(1, &VBO4);

		glBindBuffer(GL_ARRAY_BUFFER, VBO4);
		glBufferData(GL_ARRAY_BUFFER, sizeof(RWallVertices), RWallVertices, GL_STATIC_DRAW);

		glBindVertexArray(RWallVAO);

		// 载入位置
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(0 * sizeof(float)));
		glEnableVertexAttribArray(0);
		// 载入法向量
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
	}

	//载入前墙的顶点信息
	// ------------------------------------------------------------------
	unsigned int VBO5, FWallVAO;
	{
		glGenVertexArrays(1, &FWallVAO);
		glGenBuffers(1, &VBO5);

		glBindBuffer(GL_ARRAY_BUFFER, VBO5);
		glBufferData(GL_ARRAY_BUFFER, sizeof(FWallVertices), FWallVertices, GL_STATIC_DRAW);

		glBindVertexArray(FWallVAO);

		// 载入位置
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(0 * sizeof(float)));
		glEnableVertexAttribArray(0);
		// 载入法向量
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
	}



	// 载入方块灯的顶点信息
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

	// 创建风车的VAO和VBO
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

	//设置黑板的VAO和VBO
	// ------------------------------------------------------------------
	unsigned int chalkboardVAO, chalkboardVBO;
	{
		generateChalkboardVertices();
		glGenVertexArrays(1, &chalkboardVAO);
		glGenBuffers(1, &chalkboardVBO);

		glBindVertexArray(chalkboardVAO);
		glBindBuffer(GL_ARRAY_BUFFER, chalkboardVBO);
		glBufferData(GL_ARRAY_BUFFER, chalkboardVertices.size() * sizeof(float), chalkboardVertices.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); // 位置
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); // 法线
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); // 纹理坐标
		glEnableVertexAttribArray(2);
	}

	// 为边框设置 VAO 和 VBO
	unsigned int frameVAO, frameVBO;
	{
		// 生成边框顶点
		generateFrameVertices();
		glGenVertexArrays(1, &frameVAO);
		glGenBuffers(1, &frameVBO);

		glBindVertexArray(frameVAO);
		glBindBuffer(GL_ARRAY_BUFFER, frameVBO);
		glBufferData(GL_ARRAY_BUFFER, frameVertices.size() * sizeof(float), frameVertices.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
	}

	// 加载纹理
	unsigned int frameTexture;
	glGenTextures(1, &frameTexture);
	glBindTexture(GL_TEXTURE_2D, frameTexture);
	// 设置纹理环绕和过滤方式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// 加载图像，创建纹理并生成mipmap
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true); // 翻转加载的纹理
	unsigned char* data = stbi_load("frame_texture.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	unsigned int chalkboardTexture;
	glGenTextures(1, &chalkboardTexture);
	glBindTexture(GL_TEXTURE_2D, chalkboardTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	unsigned char* chalkboardData = stbi_load("board_texture.jpg", &width, &height, &nrChannels, 0);
	if (chalkboardData)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, chalkboardData);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load chalkboard texture" << std::endl;
	}
	stbi_image_free(chalkboardData);

	generateSnowflakes(100); // 生成 100 个雪花

	unsigned int flakeVAO, flakeVBO;
	{
		glGenVertexArrays(1, &flakeVAO);
		glGenBuffers(1, &flakeVBO);

		glBindVertexArray(flakeVAO);
		glBindBuffer(GL_ARRAY_BUFFER, flakeVBO);
		glBufferData(GL_ARRAY_BUFFER, snowflakes.size() * sizeof(Snowflake), snowflakes.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(1 * sizeof(float)));
		glEnableVertexAttribArray(2);
	}

	unsigned int ballVAO, ballVBO;
	{
		std::vector<float> sphereVertices = generateSphereVertices(ball.radius, 36, 18); // 36 sectors and 18 stacks
		glGenVertexArrays(1, &ballVAO);
		glGenBuffers(1, &ballVBO);
		glBindVertexArray(ballVAO);
		glBindBuffer(GL_ARRAY_BUFFER, ballVBO);
		glBufferData(GL_ARRAY_BUFFER, sphereVertices.size() * sizeof(float), &sphereVertices[0], GL_STATIC_DRAW);
		// Position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glBindVertexArray(0);
	}



	// 渲染循环
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// 时间逻辑
		// --------------------
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// 输入
		// -----
		processInput(window);

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::Begin("panel");// Create a window called "panel" and append into it.
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
		ImGui::Text("Cornell bos is scaled by %f times", scale);
		ImGui::Checkbox("Lock Cursor(Shortcut: L)", &lockCursor);
		ImGui::Checkbox("Draw firefly", &drawSnow);
		ImGui::Checkbox("Draw Ball", &drawBall);
		ImGui::SliderFloat("rotate speed", &rotateSpeed, 0.0f, 10.0f);
		ImGui::ColorEdit3("windmill color", (float*)&windmill_color);
		ImGui::ColorEdit3("ball color", (float*)&ball_color);
		ImGui::ColorEdit3("background color", (float*)&clear_color);
		ImGui::ColorEdit3("light color", (float*)&light_color);
		ImGui::ColorEdit3("celling color", (float*)&celling_color);
		ImGui::ColorEdit3("floor color", (float*)&floor_color);
		ImGui::ColorEdit3("left color", (float*)&left_color);
		ImGui::ColorEdit3("front color", (float*)&front_color);
		ImGui::ColorEdit3("right color", (float*)&right_color);
		ImGui::End();

		// 更新雪花位置
		updateSnowflakes();

		updateBallPosition();

		// 开始渲染
		// ------
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// 确保在设置 Uniforms/Drawing 对象时激活 Shader
		//---------------------------------------------------------------------
		lightingShader.use();
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 model = glm::mat4(1.0f);

		//绘制天花板
		{
			//设置光照参数
			lightingShader.setVec3("objectColor", celling_color.x, celling_color.y, celling_color.z);
			lightingShader.setVec3("lightColor", light_color.x, light_color.y, light_color.z);
			lightingShader.setVec3("lightPos", lightPos);
			lightingShader.setVec3("viewPos", camera.Position);

			// view/projection 变换
			lightingShader.setMat4("projection", projection);
			lightingShader.setMat4("view", view);

			// 世界坐标变换
			model = glm::translate(model, cubePos);
			model = glm::scale(model, glm::vec3(1.0f));
			lightingShader.setMat4("model", model);

			// 渲染
			glBindVertexArray(CeilingVAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		// 绘制地板
		{
			//设置光照参数
			lightingShader.setVec3("objectColor", floor_color.x, floor_color.y, floor_color.z);
			lightingShader.setVec3("lightColor", light_color.x, light_color.y, light_color.z);
			lightingShader.setVec3("lightPos", lightPos);
			lightingShader.setVec3("viewPos", camera.Position);

			// view/projection 变换
			lightingShader.setMat4("projection", projection);
			lightingShader.setMat4("view", view);

			// 世界坐标变换
			model = glm::mat4(1.0f);
			model = glm::translate(model, cubePos);
			model = glm::scale(model, glm::vec3(1.0f));
			lightingShader.setMat4("model", model);

			// 渲染
			glBindVertexArray(FloorVAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		// 绘制左墙
		{
			//设置光照参数
			lightingShader.setVec3("objectColor", left_color.x, left_color.y, left_color.z);
			lightingShader.setVec3("lightColor", light_color.x, light_color.y, light_color.z);
			lightingShader.setVec3("lightPos", lightPos);
			lightingShader.setVec3("viewPos", camera.Position);

			// view/projection 变换
			lightingShader.setMat4("projection", projection);
			lightingShader.setMat4("view", view);

			// 世界坐标变换
			model = glm::mat4(1.0f);
			model = glm::translate(model, cubePos);
			model = glm::scale(model, glm::vec3(1.0f));
			lightingShader.setMat4("model", model);

			// 渲染
			glBindVertexArray(LWallVAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		// 绘制右墙
		{
			//设置光照参数
			lightingShader.setVec3("objectColor", right_color.x, right_color.y, right_color.z);
			lightingShader.setVec3("lightColor", light_color.x, light_color.y, light_color.z);
			lightingShader.setVec3("lightPos", lightPos);
			lightingShader.setVec3("viewPos", camera.Position);

			// view/projection 变换
			lightingShader.setMat4("projection", projection);
			lightingShader.setMat4("view", view);

			// 世界坐标变换
			model = glm::mat4(1.0f);
			model = glm::translate(model, cubePos);
			model = glm::scale(model, glm::vec3(1.0f));
			lightingShader.setMat4("model", model);

			// 渲染
			glBindVertexArray(RWallVAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		// 绘制前墙
		{
			//设置光照参数
			lightingShader.setVec3("objectColor", front_color.x, front_color.y, front_color.z);
			lightingShader.setVec3("lightColor", light_color.x, light_color.y, light_color.z);
			lightingShader.setVec3("lightPos", lightPos);
			lightingShader.setVec3("viewPos", camera.Position);

			// view/projection 变换
			lightingShader.setMat4("projection", projection);
			lightingShader.setMat4("view", view);

			// 世界坐标变换
			model = glm::mat4(1.0f);
			model = glm::translate(model, cubePos);
			model = glm::scale(model, glm::vec3(1.0f));
			lightingShader.setMat4("model", model);

			// 渲染
			glBindVertexArray(FWallVAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		// 绘制灯方块
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

		// 绘制黑板
		{
			// 激活纹理单元0并绑定黑板纹理
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, chalkboardTexture);
			textureShader.use();
			textureShader.setInt("texture1", 0); // 将纹理单元传递给着色器
			textureShader.setBool("useTexture1", true); // 使用黑板纹理
			textureShader.setVec3("lightColor", light_color.x, light_color.y, light_color.z);
			textureShader.setVec3("lightPos", lightPos);
			textureShader.setVec3("viewPos", camera.Position);

			textureShader.setMat4("projection", projection);
			textureShader.setMat4("view", view);

			model = glm::mat4(1.0f);
			model = glm::translate(model, chalkboardPosition);
			model = glm::scale(model, chalkboardSize);
			textureShader.setMat4("model", model);

			glBindTexture(GL_TEXTURE_2D, chalkboardTexture);
			glBindVertexArray(chalkboardVAO);
			glDrawArrays(GL_TRIANGLES, 0, chalkboardVertices.size() / 8);
		}

		// 渲染边框
		{
			// 激活纹理单元0并绑定黑板纹理
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, frameTexture);
			textureShader.use();
			textureShader.setInt("texture2", 1); // 将纹理单元传递给着色器
			textureShader.setBool("useTexture1", false); // 使用黑板纹理
			textureShader.setVec3("lightColor", light_color.x, light_color.y, light_color.z);
			textureShader.setVec3("lightPos", lightPos);
			textureShader.setVec3("viewPos", camera.Position);

			textureShader.setMat4("projection", projection);
			textureShader.setMat4("view", view);

			model = glm::mat4(1.0f);
			model = glm::translate(model, chalkboardPosition);
			model = glm::scale(model, frameSize);
			textureShader.setMat4("model", model);

			glBindTexture(GL_TEXTURE_2D, frameTexture);
			glBindVertexArray(frameVAO);
			glDrawArrays(GL_TRIANGLES, 0, frameVertices.size() / 8);
		}

		// 绘制风车
		if (drawWindmill)
		{
			lightingShader.use();
			lightingShader.setVec3("objectColor", 1.0f, 1.0f, 1.0f); // 白色
			lightingShader.setMat4("projection", projection);
			lightingShader.setMat4("view", view);

			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(chalkboardPosition.x, chalkboardPosition.y, chalkboardPosition.z + 0.01f));

			if (ifRotate) {
				currentAngle += 50.0f * deltaTime * rotateSpeed; // Update the angle only if rotating
			}
			model = glm::rotate(model, glm::radians(currentAngle), glm::vec3(0.0f, 0.0f, 1.0f));

			lightingShader.setMat4("model", model);

			glBindVertexArray(windmillVAO);
			glLineWidth(2.5f);
			if (drawColor) {
				lightingShader.setVec3("objectColor", windmill_color.x, windmill_color.y, windmill_color.z);
				glDrawArrays(GL_TRIANGLES, 0, windmillVertices.size() / 3);
				lightingShader.setVec3("objectColor", 1.0f, 1.0f, 1.0f); // 白色
				glDrawArrays(GL_LINE_LOOP, 0, windmillVertices.size() / 3);
			}
			else
			{
				glDrawArrays(GL_LINE_LOOP, 0, windmillVertices.size() / 3);
			}
		}

		// 绘制雪花
		if (drawSnow)
		{
			snowflakeShader.use();
			snowflakeShader.setMat4("projection", projection);
			snowflakeShader.setMat4("view", view);
			// 更新 VBO 中的雪花数据
			glBindBuffer(GL_ARRAY_BUFFER, flakeVBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, snowflakes.size() * sizeof(Snowflake), snowflakes.data());
			// 绘制每个雪花
			glBindVertexArray(flakeVAO);
			for (auto& snowflake : snowflakes) {
				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(snowflake.x, snowflake.y, snowflake.z));
				model = glm::scale(model, glm::vec3(snowflake.size));
				snowflakeShader.setMat4("model", model);
			}
			// 使用 GL_POINTS 一次性渲染所有雪花
			snowflakeShader.setVec4("objectColor", 0.0f, 0.0f, 0.0f, 1.0f);
			snowflakeShader.setFloat("pointSize", 5.0f);
			glDrawArrays(GL_POINTS, 0, snowflakes.size());
			snowflakeShader.setVec4("objectColor", 0.98f, 0.58f, 0.098f, 1.0f);
			snowflakeShader.setFloat("pointSize", 15.0f);
			glDrawArrays(GL_POINTS, 0, snowflakes.size());
			snowflakeShader.setVec4("objectColor", 0.969f, 0.89f, 0.455f, 0.5f);
			snowflakeShader.setFloat("pointSize", 30.0f);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glDrawArrays(GL_POINTS, 0, snowflakes.size());
			glDisable(GL_BLEND);
		}

		// Render the ball
		if (drawBall)
		{
			ballShader.use();
			ballShader.setMat4("projection", projection);
			ballShader.setMat4("view", view);
			// 计算模型矩阵
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, ball.position); // 移动到小球的位置
			model = glm::scale(model, glm::vec3(ball.radius)); // 缩放到小球的半径
			ballShader.setMat4("model", model);
			ballShader.setVec3("objectColor", glm::vec3(ball_color.x, ball_color.y, ball_color.z)); // 设置小球的颜色为红色
			// 绑定球体的 VAO 并绘制
			glBindVertexArray(ballVAO);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 36 * 18); // 绘制球体的顶点（根据栈和扇区数量）
			glBindVertexArray(0);
		}

		// 渲染 imgui		
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// glfw：交换缓冲区和轮询 IO 事件（按下/释放键、移动鼠标等）
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// （可选）一旦资源超出其用途，就取消分配所有资源：
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &CeilingVAO);
	glDeleteVertexArrays(1, &FloorVAO);
	glDeleteVertexArrays(1, &RWallVAO);
	glDeleteVertexArrays(1, &LWallVAO);
	glDeleteVertexArrays(1, &FWallVAO);
	glDeleteVertexArrays(1, &lightCubeVAO);
	glDeleteVertexArrays(1, &chalkboardVAO);
	glDeleteVertexArrays(1, &windmillVAO);
	glDeleteVertexArrays(1, &frameVAO);
	glDeleteVertexArrays(1, &flakeVAO);
	glDeleteBuffers(1, &VBO1);
	glDeleteBuffers(1, &VBO2);
	glDeleteBuffers(1, &VBO3);
	glDeleteBuffers(1, &VBO4);
	glDeleteBuffers(1, &VBO5);
	glDeleteBuffers(1, &VBO6);
	glDeleteBuffers(1, &chalkboardVBO);
	glDeleteBuffers(1, &windmillVBO);
	glDeleteBuffers(1, &frameVBO);
	glDeleteBuffers(1, &flakeVBO);
	glDeleteTextures(1, &frameTexture);
	glDeleteTextures(1, &chalkboardTexture);


	// glfw：终止，清除所有以前分配的 GLFW 资源。
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

//生成黑板的顶点信息
void generateChalkboardVertices()
{
	float width = chalkboardSize.x;
	float height = chalkboardSize.y;
	float depth = chalkboardSize.z;

	// Front face with texture coordinates
	chalkboardVertices = {
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

//查询 GLFW 是否按下/释放了该帧的相关键并做出相应的反应
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
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
}

//生成边框信息
void generateFrameVertices()
{
	float width = frameSize.x;
	float height = frameSize.y;
	float depth = frameSize.z;
	float thickness = frameThickness.x;

	// 边框的前面，包含纹理坐标
	frameVertices = {
		// 上边框
		-width / 2, height / 2, depth / 2, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
		 width / 2, height / 2, depth / 2, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
		 width / 2, height / 2 - thickness, depth / 2, 0.0f, 0.0f, 1.0f, 1.0f, 0.9f,
		 width / 2, height / 2 - thickness, depth / 2, 0.0f, 0.0f, 1.0f, 1.0f, 0.9f,
		-width / 2, height / 2 - thickness, depth / 2, 0.0f, 0.0f, 1.0f, 0.0f, 0.9f,
		-width / 2, height / 2, depth / 2, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,

		// 下边框
		-width / 2, -height / 2, depth / 2, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		 width / 2, -height / 2, depth / 2, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
		 width / 2, -height / 2 + thickness, depth / 2, 0.0f, 0.0f, 1.0f, 1.0f, 0.1f,
		 width / 2, -height / 2 + thickness, depth / 2, 0.0f, 0.0f, 1.0f, 1.0f, 0.1f,
		-width / 2, -height / 2 + thickness, depth / 2, 0.0f, 0.0f, 1.0f, 0.0f, 0.1f,
		-width / 2, -height / 2, depth / 2, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,

		// 左边框
		-width / 2, height / 2, depth / 2, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
		-width / 2 + thickness, height / 2, depth / 2, 0.0f, 0.0f, 1.0f, 0.1f, 1.0f,
		-width / 2 + thickness, -height / 2, depth / 2, 0.0f, 0.0f, 1.0f, 0.1f, 0.0f,
		-width / 2 + thickness, -height / 2, depth / 2, 0.0f, 0.0f, 1.0f, 0.1f, 0.0f,
		-width / 2, -height / 2, depth / 2, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		-width / 2, height / 2, depth / 2, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,

		// 右边框
		 width / 2, height / 2, depth / 2, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
		 width / 2 - thickness, height / 2, depth / 2, 0.0f, 0.0f, 1.0f, 0.9f, 1.0f,
		 width / 2 - thickness, -height / 2, depth / 2, 0.0f, 0.0f, 1.0f, 0.9f, 0.0f,
		 width / 2 - thickness, -height / 2, depth / 2, 0.0f, 0.0f, 1.0f, 0.9f, 0.0f,
		 width / 2, -height / 2, depth / 2, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
		 width / 2, height / 2, depth / 2, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
	};
}

// glfw：每当窗口大小发生变化（通过操作系统或用户调整大小）时，此回调函数都会执行
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// 确保视区与新的窗口尺寸匹配;请注意，width和height将明显大于 Retina 显示屏上指定的高度
	glViewport(0, 0, width, height);
}


// glfw: 每当鼠标移动时，该回调都会被调用
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
	float yoffset = lastY - ypos; // 反转，因为 y 坐标从下到上

	lastX = xpos;
	lastY = ypos;

	if (!lockCursor)
	{
		camera.ProcessMouseMovement(xoffset, yoffset);
	}
}

// glfw:每当鼠标滚轮滚动时，该回调都会被调用
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (!lockCursor)
	{
		camera.ProcessMouseScroll(static_cast<float>(yoffset));
	}
}

// 创建雪花粒子系统
void generateSnowflakes(int count) {
	srand(static_cast<unsigned>(time(0))); // 随机数种子
	for (int i = 0; i < count; ++i) {
		Snowflake flake;
		flake.x = cubePos.x + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) - 0.5f; // x 范围在 cube 内 [-0.5, 0.5]
		flake.y = cubePos.y + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) - 1.5f; // y 范围在 cube 内 [-0.5, 0.5]
		flake.z = static_cast<float>(rand()) / static_cast<float>(RAND_MAX); // z 范围在 cube 内 [-0.5, 0.5]
		flake.size = static_cast<float>(rand() % 5 + 1); // size 范围 1 到 5
		flake.speed = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 0.1f; // 随机的雪花下落速度
		snowflakes.push_back(flake);
	}
}

// 渲染和更新雪花位置
void updateSnowflakes() {
	srand(static_cast<unsigned>(time(0)));
	for (auto& snowflake : snowflakes) {
		if (static_cast<float>(rand()) / static_cast<float>(RAND_MAX) >= 0.5f) {
			snowflake.y -= snowflake.speed * deltaTime; // 雪花下落
		}
		else {
			snowflake.y += snowflake.speed * deltaTime; // 雪花下落
		}
		if (static_cast<float>(rand()) / static_cast<float>(RAND_MAX) >= 0.5f) {
			snowflake.x += snowflake.speed * deltaTime * 0.5f;
		}
		else {
			snowflake.x -= snowflake.speed * deltaTime * 0.5f;
		}
		// 当雪花超出立方体底部时，重置到立方体顶部
		if (snowflake.y < cubePos.y - 0.5f) { // 超出立方体底部
			snowflake.y += 0.5f; // 重置到立方体顶部
			snowflake.x = cubePos.x + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) - 0.5f; // 随机 x 位置
		}
	}
}

// 更新小球位置
void updateBallPosition() {
	ball.position += ball.velocity * deltaTime;
	if (ball.position.x - ball.radius <= -1.2f) {
		ball.velocity.x = fabs(ball.velocity.x); // Reverse X direction
	}
	if (ball.position.x + ball.radius >= 1.2f) {
		ball.velocity.x = -fabs(ball.velocity.x); // Reverse X direction
	}
	if (ball.position.y - ball.radius <= -0.4f) {
		ball.velocity.y = fabs(ball.velocity.y); // Reverse Y direction
	}
	if (ball.position.y + ball.radius >= 1.0f) {
		ball.velocity.y = -fabs(ball.velocity.y); // Reverse Y direction
	}
	if (ball.position.z + ball.radius >= 2.7f) {
		ball.velocity.z = -fabs(ball.velocity.z); // Reverse Z direction
	}
	if (ball.position.z - ball.radius <= 1.3f) {
		ball.velocity.z = fabs(ball.velocity.z); // Reverse Z direction
	}
}

std::vector<float> generateSphereVertices(float radius, int sectorCount, int stackCount) {
	std::vector<float> vertices;
	float x, y, z, xy; // Position
	float sectorStep = 2 * acos(-1.0) / sectorCount;
	float stackStep = acos(-1.0) / stackCount;
	float sectorAngle, stackAngle;

	for (int i = 0; i <= stackCount; ++i) {
		stackAngle = acos(-1.0) / 2 - i * stackStep; // Starting from pi/2 to -pi/2
		xy = radius * cosf(stackAngle);        // r * cos(u)
		y = radius * sinf(stackAngle);         // r * sin(u)

		// Add (sectorCount+1) vertices per stack
		for (int j = 0; j <= sectorCount; ++j) {
			sectorAngle = j * sectorStep;

			// Vertex position
			x = xy * cosf(sectorAngle); // r * cos(u) * cos(v)
			z = xy * sinf(sectorAngle); // r * cos(u) * sin(v)
			vertices.push_back(x);
			vertices.push_back(y);
			vertices.push_back(z);
		}
	}
	return vertices;
}