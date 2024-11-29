#ifndef KOCH_SNOWFLAKE_HPP
#define KOCH_SNOWFLAKE_HPP

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <cmath>
#include <random>
#include "shader_m.h"

#define M_PI 3.14159265358979323846

struct Snowflake {
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 acceleration;
	float drag;
	std::vector<glm::vec3> flakeVertices; // Store Koch snowflake vertices
	float rotation; // Added rotation for each snowflake
};

class KochSnowflake {
public:
	void init();
	void update(float dt);
	void render(Shader& shader);

private:
	std::vector<Snowflake> particles;
	unsigned int VAO, VBO;
	const int maxParticles = 500;

	// Koch snowflake generation methods
	std::vector<glm::vec3> generateKochSnowflake(float size, int iterations);
	glm::vec3 rotatePoint(const glm::vec3& point, float angle);

	void respawnParticle(Snowflake& particle);
	bool isInsideCylinder(glm::vec3 position);
	bool isInsideBall(glm::vec3 position);
	bool isInsideCone(glm::vec3 position);
};

std::vector<glm::vec3> KochSnowflake::generateKochSnowflake(float size, int iterations) {
	// 初始六边形的顶点
	std::vector<glm::vec3> vertices = {
		glm::vec3(size, 0.0f, 0.0f),
		glm::vec3(size / 2.0f, size * std::sqrt(3.0f) / 2.0f, 0.0f),
		glm::vec3(-size / 2.0f, size * std::sqrt(3.0f) / 2.0f, 0.0f),
		glm::vec3(-size, 0.0f, 0.0f),
		glm::vec3(-size / 2.0f, -size * std::sqrt(3.0f) / 2.0f, 0.0f),
		glm::vec3(size / 2.0f, -size * std::sqrt(3.0f) / 2.0f, 0.0f)
	};

	for (int iter = 0; iter < iterations; ++iter) {
		std::vector<glm::vec3> newVertices;

		for (size_t i = 0; i < vertices.size(); ++i) {
			glm::vec3 start = vertices[i];
			glm::vec3 end = vertices[(i + 1) % vertices.size()];

			// 将线段分为三等分
			glm::vec3 oneThird = start + (end - start) * (1.0f / 3.0f);
			glm::vec3 twoThirds = start + (end - start) * (2.0f / 3.0f);

			// 计算峰点（等边三角形的“突起”）
			glm::vec3 direction = glm::normalize(glm::vec3(-(end.y - start.y), end.x - start.x, 0.0f));  // 垂直方向
			float length = glm::length(end - start) / 3.0f;
			glm::vec3 peak = oneThird + direction * length;

			// 为 Koch 雪花添加新的顶点
			newVertices.push_back(start);
			newVertices.push_back(oneThird);
			newVertices.push_back(peak);
			newVertices.push_back(twoThirds);
		}

		vertices = newVertices;
	}

	return vertices;
}

glm::vec3 KochSnowflake::rotatePoint(const glm::vec3& point, float angle) {
	glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 1.0f, 0.0f));
	return glm::vec3(rotationMatrix * glm::vec4(point, 1.0f));
}

void KochSnowflake::init() {
	particles.resize(maxParticles);

	for (auto& particle : particles) {
		respawnParticle(particle);
	}

	// Generate vertices for all particles
	std::vector<float> allVertices;
	for (const auto& particle : particles) {
		for (const auto& vertex : particle.flakeVertices) {
			allVertices.push_back(vertex.x);
			allVertices.push_back(vertex.y);
			allVertices.push_back(vertex.z);
		}
	}

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, allVertices.size() * sizeof(float), allVertices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void KochSnowflake::update(float dt) {
	for (auto& particle : particles) {
		if (isInsideCone(particle.position) || !isInsideBall(particle.position)) {
			respawnParticle(particle);
		}
		particle.acceleration = glm::vec3(0.0f, -0.1f, 0.0f);
		particle.velocity += particle.acceleration * dt;
		particle.velocity *= particle.drag;
		particle.position += particle.velocity * dt;

		// Add rotation to snowflake
		particle.rotation += 0.5f * dt; // Adjust rotation speed as needed
	}
}

void KochSnowflake::render(Shader& shader) {
	glBindVertexArray(VAO);
	for (const auto& particle : particles) {
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, particle.position);

		// Add rotation around Y-axis
		model = glm::rotate(model, particle.rotation, glm::vec3(0.0f, 1.0f, 0.0f));

		// Scale down the snowflake
		model = glm::scale(model, glm::vec3(0.002f, 0.002f, 0.002f));

		shader.setMat4("model", model);

		// Draw the snowflake as a line loop
		glDrawArrays(GL_LINE_LOOP, 0, particle.flakeVertices.size());
	}
	glBindVertexArray(0);
}

void KochSnowflake::respawnParticle(Snowflake& particle) {
	// Previous position generation logic remains the same
	float centerX = 0.0f;
	float centerZ = 2.1f;
	float angle = (rand() % 360) * (M_PI / 180.0f);
	float radius = static_cast<float>(rand()) / RAND_MAX * 0.2f;

	particle.position = glm::vec3(
		centerX + radius * cos(angle),
		((rand() % 80) / 100.0f + 0.0f),
		centerZ + radius * sin(angle)
	);

	particle.velocity = glm::vec3(
		(static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.1f,
		-0.05f,
		(static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.1f
	);

	particle.acceleration = glm::vec3(0.0f);
	particle.drag = 0.98f + static_cast<float>(rand()) / RAND_MAX * 0.02f;

	// Generate Koch snowflake for this particle
	particle.flakeVertices = generateKochSnowflake(1.0f, 3); // 3 iterations
	particle.rotation = 0.0f; // Initialize rotation
}

// Existing boundary check methods remain the same
bool KochSnowflake::isInsideCylinder(glm::vec3 position) {
	return (glm::distance(glm::vec3(0.0f, 0.13f, 2.1f), glm::vec3(position.x, 0.13f, position.z)) <= 0.2f &&
		position.y >= 0.13f && position.y <= 0.9f);
}

bool KochSnowflake::isInsideBall(glm::vec3 position) {
	return (glm::distance(glm::vec3(0.0f, (0.13f + 0.34f) / 2.0f, 2.1f), glm::vec3(position.x, position.y, position.z)) <= 0.2f &&
		position.y >= 0.13f && position.y <= 0.9f);
}

bool KochSnowflake::isInsideCone(glm::vec3 position) {
	// Existing implementation remains the same
	if (position.y < 0.13f || position.y > 0.34f) {
		return false;
	}
	glm::vec3 coneApex(0.0f, 0.34f, 2.1f);
	glm::vec3 coneBase(0.0f, 0.13f, 2.1f);
	float coneHeight = coneApex.y - coneBase.y;
	float baseRadius = 0.08f;
	float distanceToAxis = glm::distance(glm::vec2(position.x, position.z), glm::vec2(coneBase.x, coneBase.z));
	float distanceToApex = coneApex.y - position.y;
	float coneRadius = baseRadius * distanceToApex / coneHeight;
	return distanceToAxis <= coneRadius;
}

#endif // KOCH_SNOWFLAKE_HPP