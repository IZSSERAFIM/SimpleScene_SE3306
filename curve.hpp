#pragma once
#ifndef CURVE_H
#define CURVE_H

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

float quad[] = {
	-1, -1, 2,
	1, -1, 2,
	-1, 1, 2,
	1, 1, 2,
	-1, 1, 2,
	1, -1, 2 };
class curve
{
private:
	int m_num;
public:
	int num;
	float* verts;
	int samplenum;
	float* samples;

	curve()
	{
		num = 0;
		verts = nullptr;
		samplenum = 100;
		samples = new float[3 * (samplenum + 1)];
	}

	~curve()
	{
		if (verts)
			delete[] verts;
		if (samples)
			delete[] samples;
	}

	void update()
	{
		if (num != m_num)
		{
			delete (verts);
			verts = new float[3 * num];
			for (int i = 0; i < num; ++i)
			{
				verts[3 * i] = -0.5 + 1.0 / (num - 1) * i;
				verts[3 * i + 1] = 0.3;
				verts[3 * i + 2] = 2.0;
			}
			m_num = num;
		}
	}
	float getparam(int n, int x, float t)
	{
		float res = pow(t, x) * pow(1.0 - t, n - x);
		long up = 1;
		long down = 1;
		for (int i = 0; i < x; ++i)
		{
			up *= (n - i);
			down *= i + 1;
		}
		return up / down * res;
	}
	void gencurve()
	{
		for (int i = 0; i <= samplenum; ++i)
		{
			float t = (float)i / samplenum;
			float x = 0, y = 0, z = 2.0f;
			float param;
			for (int j = 0; j < num; ++j)
			{
				param = getparam(num - 1, j, t);
				x += verts[j * 3] * param;
				y += verts[j * 3 + 1] * param;
			}
			samples[i * 3] = x;
			samples[i * 3 + 1] = y;
			samples[i * 3 + 2] = z;
		}
	}
	void updategl(unsigned int& linevao, unsigned int& linevbo,
		unsigned int& pointvao, unsigned int& pointvbo,
		unsigned int& instancevbo)
	{
		glBindVertexArray(linevao);
		glBindBuffer(GL_ARRAY_BUFFER, linevbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * (samplenum + 1), samples, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);

		float* instance = new float[num * 3];
		for (int i = 0; i < num; ++i)
		{
			instance[i * 3] = verts[i * 3];
			instance[i * 3 + 1] = verts[i * 3 + 1];
			instance[i * 3 + 2] = 2.0f;
		}

		glBindVertexArray(pointvao);

		float quadVertices[] = {
			-1.0f, -1.0f, 0.0f,
			1.0f, -1.0f, 0.0f,
			1.0f, 1.0f, 0.0f,
			-1.0f, -1.0f, 0.0f,
			1.0f, 1.0f, 0.0f,
			-1.0f, 1.0f, 0.0f };

		glBindBuffer(GL_ARRAY_BUFFER, pointvbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);

		glBindBuffer(GL_ARRAY_BUFFER, instancevbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num * 3, instance, GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glVertexAttribDivisor(1, 1);

		delete[] instance;
	}
	float* getsamples()
	{
		return samples;
	}
};

#endif // !CURVE_H
