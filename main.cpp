#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "camera.h"
#include "model.h"

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
unsigned int loadTexture(const char* path);
unsigned int loadCubemap(vector<std::string> faces);

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
float scale = 2.0f;

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
	Shader skyboxShader("skybox_vertex.glsl", "skybox_fragment.glsl");

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

	// ������պ�
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
	skyboxShader.use();
	skyboxShader.setInt("skybox", 0);

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
		ImGui::Checkbox("Lock Cursor(Shortcut: L)", &lockCursor);
		ImGui::ColorEdit3("background color", (float*)&clear_color);
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
			lightingShader.setVec3("lightColor", light_color.x, light_color.y, light_color.z);
			lightingShader.setVec3("lightPos", lightPos);
			lightingShader.setVec3("viewPos", camera.Position);

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
			lightingShader.setVec3("lightColor", light_color.x, light_color.y, light_color.z);
			lightingShader.setVec3("lightPos", lightPos);
			lightingShader.setVec3("viewPos", camera.Position);

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
			lightingShader.setVec3("lightColor", light_color.x, light_color.y, light_color.z);
			lightingShader.setVec3("lightPos", lightPos);
			lightingShader.setVec3("viewPos", camera.Position);

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
			lightingShader.setVec3("lightColor", light_color.x, light_color.y, light_color.z);
			lightingShader.setVec3("lightPos", lightPos);
			lightingShader.setVec3("viewPos", camera.Position);

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
			lightingShader.setVec3("lightColor", light_color.x, light_color.y, light_color.z);
			lightingShader.setVec3("lightPos", lightPos);
			lightingShader.setVec3("viewPos", camera.Position);

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
	glDeleteBuffers(1, &VBO1);
	glDeleteBuffers(1, &VBO2);
	glDeleteBuffers(1, &VBO3);
	glDeleteBuffers(1, &VBO4);
	glDeleteBuffers(1, &VBO5);
	glDeleteBuffers(1, &VBO6);
	glDeleteBuffers(1, &skyboxVBO);

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