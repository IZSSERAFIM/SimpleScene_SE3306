#pragma once
#ifndef SWEEP_H
#define SWEEP_H

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <glm/glm.hpp>

struct rectangle
{
	float width;
	float length;
};

struct ellipse
{
	float a;
	float b;
};

struct circle
{
	float radius;
};

class sweep
{
private:
	std::vector<glm::vec3> curve;
	glm::vec3 point_left;
	glm::vec3 point_middle;
	glm::vec3 point_right;
	rectangle rect;
	ellipse ell;
	circle cir;

	unsigned int VAO, VBO, EBO;
	std::vector<float> vertices;
	std::vector<unsigned int> indices;

	const int segments = 32;	 // Բ�κ���Բ�εķֶ���
	const float texScale = 2.0f; // ���������ظ�����

	void generate_profile(float t, std::vector<glm::vec3>& profile_points)
	{
		profile_points.clear();

		if (t <= 0.5f)
		{
			// �����Σ��Ӿ��ι��ɵ���Բ
			float blend = t * 2.0f;

			for (int i = 0; i <= segments; i++)
			{
				float angle = 2.0f * glm::pi<float>() * i / segments;

				// ���εĵ�
				glm::vec3 rect_point;
				if (angle <= glm::pi<float>() / 2 || angle >= 3 * glm::pi<float>() / 2)
				{
					rect_point.y = rect.length / 2.0f * glm::sin(angle);
					rect_point.z = rect.width / 2.0f * glm::sign(glm::cos(angle));
				}
				else
				{
					rect_point.y = rect.length / 2.0f * glm::sign(glm::sin(angle));
					rect_point.z = rect.width / 2.0f * glm::cos(angle);
				}

				// ��Բ�ĵ�
				glm::vec3 ellipse_point(
					0.0f,
					ell.b * glm::sin(angle),
					ell.a * glm::cos(angle));

				// ���������״
				glm::vec3 blended = glm::mix(rect_point, ellipse_point, blend);
				profile_points.push_back(blended);
			}
		}
		else
		{
			// ���Ұ�Σ�����Բ���ɵ�Բ
			float blend = (t - 0.5f) * 2.0f;

			for (int i = 0; i <= segments; i++)
			{
				float angle = 2.0f * glm::pi<float>() * i / segments;

				// ��Բ�ĵ�
				glm::vec3 ellipse_point(
					0.0f,
					ell.b * glm::sin(angle),
					ell.a * glm::cos(angle));

				// Բ�ĵ�
				glm::vec3 circle_point(
					0.0f,
					cir.radius * glm::sin(angle),
					cir.radius * glm::cos(angle));

				// ���������״
				glm::vec3 blended = glm::mix(ellipse_point, circle_point, blend);
				profile_points.push_back(blended);
			}
		}
	}

	float calculateArcLength(const glm::vec3& p1, const glm::vec3& p2)
	{
		return glm::length(p2 - p1);
	}

public:
	sweep() : VAO(0), VBO(0), EBO(0) {}

	void init_curve(float* samples, int size)
	{
		curve = std::vector<glm::vec3>(size + 1);
		for (int i = 0; i <= size; ++i)
		{
			curve[i].x = samples[3 * i];
			curve[i].y = samples[3 * i + 1];
			curve[i].z = samples[3 * i + 2];
		}
		point_left = curve[0];
		point_middle = curve[size / 2];
		point_right = curve[size];
	}
	void set3(float rectangle_width, float rectangle_length, float ellipse_a, float ellipse_b, float circle_radius)
	{
		rect.width = rectangle_width;
		rect.length = rectangle_length;
		ell.a = ellipse_a;
		ell.b = ellipse_b;
		cir.radius = circle_radius;

		generate_mesh();
	}

	void generate_mesh()
	{
		vertices.clear();
		indices.clear();

		const int path_segments = curve.size() - 1;
		std::vector<glm::vec3> profile_points;

		// ������·������
		float totalLength = 0.0f;
		std::vector<float> pathLengths(path_segments + 1, 0.0f);
		for (int i = 0; i < path_segments; i++)
		{
			totalLength += calculateArcLength(curve[i], curve[i + 1]);
			pathLengths[i + 1] = totalLength;
		}

		// ���ɶ���
		for (int i = 0; i <= path_segments; i++)
		{
			float t = static_cast<float>(i) / path_segments;
			float texV = pathLengths[i] / totalLength; // ʹ�ù�һ�����ۻ�·������

			// ��ȡ��ǰ·����
			glm::vec3 path_point = curve[i];

			// �������߷���
			glm::vec3 tangent;
			if (i < path_segments)
			{
				tangent = glm::normalize(curve[i + 1] - curve[i]);
			}
			else
			{
				tangent = glm::normalize(curve[i] - curve[i - 1]);
			}

			// ������ת����
			glm::vec3 up(0.0f, 1.0f, 0.0f);
			glm::vec3 right = glm::normalize(glm::cross(up, tangent));
			up = glm::normalize(glm::cross(tangent, right));

			// ���ɽ���������
			generate_profile(t, profile_points);

			// �任����Ӷ���
			for (int j = 0; j <= segments; j++)
			{
				float angle = 2.0f * glm::pi<float>() * j / segments;
				float texU = angle / (2.0f * glm::pi<float>()); // ��һ���ĽǶ�

				const auto& profile_point = profile_points[j];
				glm::vec3 transformed_point = path_point +
					right * profile_point.y +
					up * profile_point.z;

				// �������߷����ڽ����ϣ�
				glm::vec3 profile_tangent;
				if (j < segments)
				{
					profile_tangent = glm::normalize(profile_points[j + 1] - profile_point);
				}
				else
				{
					profile_tangent = glm::normalize(profile_points[0] - profile_point);
				}

				// ���㷨�ߣ����淨�ߺ�·�����ߵĲ��
				glm::vec3 profile_normal = glm::normalize(glm::vec3(0.0f, profile_point.y, profile_point.z));
				glm::vec3 normal = glm::normalize(
					right * profile_normal.y +
					up * profile_normal.z);

				// λ��
				vertices.push_back(transformed_point.x);
				vertices.push_back(transformed_point.y);
				vertices.push_back(transformed_point.z);

				// ����
				vertices.push_back(normal.x);
				vertices.push_back(normal.y);
				vertices.push_back(normal.z);

				// ��������
				vertices.push_back(texU * texScale);
				vertices.push_back(texV * texScale);
			}
		}

		// ��������
		for (int i = 0; i < path_segments; i++)
		{
			for (int j = 0; j < segments; j++)
			{
				int current = i * (segments + 1) + j;
				int next = current + segments + 1;

				indices.push_back(current);
				indices.push_back(next);
				indices.push_back(current + 1);

				indices.push_back(current + 1);
				indices.push_back(next);
				indices.push_back(next + 1);
			}
		}

		// ��������VAO��VBO��EBO
		if (VAO == 0)
		{
			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);
			glGenBuffers(1, &EBO);
		}

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

		// ���ö�������
		// λ��
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		// ����
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		// ��������
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
	}

	void draw()
	{
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	}

	~sweep()
	{
		if (VAO != 0)
		{
			glDeleteVertexArrays(1, &VAO);
			glDeleteBuffers(1, &VBO);
			glDeleteBuffers(1, &EBO);
		}
	}
};

#endif