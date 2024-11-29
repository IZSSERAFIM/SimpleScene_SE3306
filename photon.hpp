#ifndef PHOTON_HPP
#define PHOTON_HPP

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <cmath>
#include <random>
#include "shader_m.h"

#define M_PI 3.14159265358979323846

class PhotonSystem {
public:
	struct Particle {
		glm::vec3 position;
		glm::vec3 velocity;
		glm::vec4 color;
		float life;
	};

	std::vector<Particle> particles;
	unsigned int VAO, VBO;
	glm::vec3 coneApex;

	PhotonSystem(unsigned int numParticles, const glm::vec3& coneApex);
	void update(float deltaTime);
	void render(Shader& shader);

private:
	void initializeParticle(Particle& particle);
};

PhotonSystem::PhotonSystem(unsigned int numParticles, const glm::vec3& coneApex)
	: coneApex(coneApex) {
	particles.resize(numParticles);
	for (auto& particle : particles) {
		initializeParticle(particle);
	}

	// OpenGL buffer setup
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, numParticles * sizeof(Particle), &particles[0], GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, position));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, color));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);
}

void PhotonSystem::initializeParticle(Particle& particle) {
	static std::default_random_engine generator;
	static std::uniform_real_distribution<float> radiusDist(0.5f, 1.5f);
	static std::uniform_real_distribution<float> lifeDist(0.05f, 0.23f);
	static std::uniform_real_distribution<float> cosThetaDist(std::cos(glm::radians(30.0f)), 1.0f); // ɢ��Ƕ�Ϊ60�ȣ�30�ȵ�0�ȵ�����ֵ��
	static std::uniform_real_distribution<float> azimuthDist(0.0f, 2.0f * glm::pi<float>());

	const std::vector<glm::vec3> starDirections = {
		glm::normalize(glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)),    // ��
		glm::normalize(glm::rotate(glm::mat4(1.0f), glm::radians(72.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)),   // ����
		glm::normalize(glm::rotate(glm::mat4(1.0f), glm::radians(144.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)),  // ����
		glm::normalize(glm::rotate(glm::mat4(1.0f), glm::radians(216.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)),  // ����
		glm::normalize(glm::rotate(glm::mat4(1.0f), glm::radians(288.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)),  // ����
	};

	// ���ѡ��һ������Ƿ���
	int directionIndex = rand() % starDirections.size();
	glm::vec3 coneAxis = glm::normalize(starDirections[directionIndex]);

	// ��׶����������ɷ���
	float cosTheta = cosThetaDist(generator);
	float sinTheta = std::sqrt(1.0f - cosTheta * cosTheta);
	float phi = azimuthDist(generator);

	float x = sinTheta * std::cos(phi);
	float y = sinTheta * std::sin(phi);
	float z = cosTheta;

	// ������coneAxisΪ�������ϵ
	glm::vec3 w = coneAxis;
	glm::vec3 u = glm::normalize(glm::cross((std::abs(w.x) > 0.1f ? glm::vec3(0.0f, 1.0f, 0.0f) : glm::vec3(1.0f, 0.0f, 0.0f)), w));
	glm::vec3 v = glm::cross(w, u);

	// �������ӷ���
	glm::vec3 direction = x * u + y * v + z * w;

	// �����������ֲ���
	float radius = radiusDist(generator);
	float randomFactor = radiusDist(generator);
	glm::vec3 offset = direction * randomFactor;
	particle.position = coneApex; // ��ȷ��coneApex����ȷ����
	particle.velocity = glm::normalize(offset);
	particle.color = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
	particle.life = lifeDist(generator);
}

void PhotonSystem::update(float deltaTime) {
	for (auto& particle : particles) {
		if (particle.life > 0.0f) {
			particle.position += particle.velocity * deltaTime * 2.0f;
			particle.life -= deltaTime;
			particle.color.a = particle.life * 0.5f; // �����������ڵ���͸����
		}
		else {
			initializeParticle(particle); // �������ӣ����������������������������
		}
	}
}


void PhotonSystem::render(Shader& shader) {
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, particles.size() * sizeof(Particle), particles.data());
	glDrawArrays(GL_POINTS, 0, particles.size());
	glBindVertexArray(0);
}

#endif // !PHOTON_HPP
