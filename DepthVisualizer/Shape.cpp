#define _USE_MATH_DEFINES
#include "shape.h"
#include <random>

static void CalcNormalsTriangle(
	float* vert0,
	float* vert1,
	float* vert2,
	float* norm0,
	float* norm1,
	float* norm2
	)
{
	glm::vec3 A(vert0[0], vert0[1], vert0[2]);
	glm::vec3 B(vert1[0], vert1[1], vert1[2]);
	glm::vec3 C(vert2[0], vert2[1], vert2[2]);

	glm::vec3 edgeab = (B - A);
	glm::vec3 edgeac = (C - A);
	glm::vec3 edgebc = (C - B);

	glm::vec3 n;

	if (length(edgeab) > 1.e-6)
	{
		if (length(edgeac) > 1.e-6)
		{
			n = cross(normalize(edgeab), normalize(edgeac));
		}
		else if (length(edgebc) > 1.e-6)
		{
			n = cross(normalize(edgebc), normalize(edgeab));
		}
	}
	else
	{
		if (length(edgeac) > 1.e-6 && length(edgebc) > 1.e-6)
		{
			n = cross(normalize(edgebc), normalize(edgeac));
		}
		else
		{
			n = glm::vec3(0, 0, 1);
		}
	}

	n = normalize(n);
	norm0[0] += n.x; norm1[0] += n.x; norm2[0] += n.x;
	norm0[1] += n.y; norm1[1] += n.y; norm2[1] += n.y;
	norm0[2] += n.z; norm1[2] += n.z; norm2[2] += n.z;
}

void Shape::CreateSphere(float r, float zoffset, int phisteps, int tetasteps,
	float textleft,
	float texttop,
	float textright,
	float textbottom,
	float scale)
{
	float dteta = float(M_PI / tetasteps);
	float dphi = float(M_PI * 2 / phisteps);

	float textwidth = textright - textleft;
	float textheight = textbottom - texttop;

	float dtextx = textwidth / phisteps;
	float dtexty = textheight / tetasteps;


	int startvert = verts.size();
	int startind = indices.size();
	int starttext = texcoords.size();
	int startnorm = normals.size();

	tetasteps++;
	phisteps++;

	for (int i = 0; i < tetasteps; i++)
	{
		float curZ = r * cos(i * dteta);
		float texty = i * dtexty;

		for (int j = 0; j < phisteps; j++)
		{
			float x = (float)(r * sin(i * dteta) * cos(j * dphi));
			float y = (float)(r * sin(i * dteta) * sin(j * dphi));
			float z = curZ * scale;

			verts.push_back(x);
			verts.push_back(y);
			verts.push_back(zoffset + z);

			texcoords.push_back(textleft + j * dtextx);
			texcoords.push_back(texty);
		}
	}

	for (int i = 0; i < tetasteps - 1; i++)
	{
		int curStrip = i * phisteps;

		for (int j = 0; j < phisteps - 1; j++)
		{
			if (i > 0)
			{
				indices.push_back(curStrip + j + startvert / 3);
				indices.push_back(curStrip + j + phisteps + startvert / 3);
				indices.push_back(curStrip + j + 1 + startvert / 3);
			}

			if (i < tetasteps - 2)
			{
				indices.push_back(curStrip + j + phisteps + startvert / 3);
				indices.push_back(curStrip + j + 1 + phisteps + startvert / 3);
				indices.push_back(curStrip + j + 1 + startvert / 3);
			}

			if (j == phisteps - 2)
			{
				dupnormals.push_back(curStrip + j + startvert / 3);
				dupnormals.push_back(curStrip + startvert / 3);
				dupnormals.push_back(curStrip + phisteps + j + startvert / 3);
				dupnormals.push_back(curStrip + phisteps + startvert / 3);
			}
		}
	}


	//normals.resize(verts.size());

	// Create chunk
	BufferChunk chunk;
	chunk.start = startind;
	chunk.count = indices.size() - startind;
	chunk.type = GL_TRIANGLES;
	chunks.push_back(chunk);
}

void Shape::CalcNormalsTriangles(
	int startvert,
	int startind
	)
{
	// –асчет нормалей
	std::vector<glm::vec3> normals_tmp((verts.size() - startvert) / 3);

	for (size_t i = startind; i < indices.size(); i += 3)
	{
		int vertindex0 = indices[i];
		int vertindex1 = indices[i + 1];
		int vertindex2 = indices[i + 2];

		float* a = &verts[vertindex0 * 3];
		float* b = &verts[vertindex1 * 3];
		float* c = &verts[vertindex2 * 3];

		glm::vec3 A(a[0], a[1], a[2]);
		glm::vec3 B(b[0], b[1], b[2]);
		glm::vec3 C(c[0], c[1], c[2]);

		glm::vec3 edgeab = (B - A);
		glm::vec3 edgeac = (C - A);
		glm::vec3 edgebc = (C - B);

		glm::vec3 n;

		if (length(edgeab) > 1.e-6)
		{
			if (length(edgeac) > 1.e-6)
			{
				n = cross(normalize(edgeab), normalize(edgeac));
			}
			else if (length(edgebc) > 1.e-6)
			{
				n = cross(normalize(edgebc), normalize(edgeab));
			}
		}
		else
		{
			if (length(edgeac) > 1.e-6 && length(edgebc) > 1.e-6)
			{
				n = cross(normalize(edgebc), normalize(edgeac));
			}
			else
			{
				n = glm::vec3(0, 0, 1);
			}
		}

		n = normalize(n);

		normals_tmp[vertindex0 - startvert / 3] += n;
		normals_tmp[vertindex1 - startvert / 3] += n;
		normals_tmp[vertindex2 - startvert / 3] += n;
	}

	// Take into account duplicated normals
	for (size_t i = 0; i < dupnormals.size(); i += 2)
	{
		int normindex0 = dupnormals[i];
		int normindex1 = dupnormals[i + 1];
		vec3 v0 = normals_tmp[normindex0];
		vec3 v1 = normals_tmp[normindex1];
		normals_tmp[normindex0] += v1;
		normals_tmp[normindex1] += v0;
	}

	// Ќормализаци€ нормалей
	for (size_t i = 0; i < normals_tmp.size(); i++)
	{
		glm::vec3 n = normalize(normals_tmp[i]);
		normals.push_back(n.x);
		normals.push_back(n.y);
		normals.push_back(n.z);
	}
}


void Shape::CreateVertexBuffer()
{
	GLuint vboVertices, vboIndices, vboNormals, vboTexCoords;

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vboVertices);
	glGenBuffers(1, &vboIndices);
	glGenBuffers(1, &vboNormals);
	glGenBuffers(1, &vboTexCoords);


	glBindVertexArray(vao); GL_CHECK_ERRORS;
	{

		//передаем в шейдерную программу атрибут координат вершин
		glBindBuffer(GL_ARRAY_BUFFER, vboVertices); GL_CHECK_ERRORS;
		glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(GL_FLOAT), &verts[0], GL_STATIC_DRAW); GL_CHECK_ERRORS;
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (GLvoid*)0); GL_CHECK_ERRORS;
		glEnableVertexAttribArray(0); GL_CHECK_ERRORS;

		//передаем в шейдерную программу атрибут нормалей
		glBindBuffer(GL_ARRAY_BUFFER, vboNormals); GL_CHECK_ERRORS;
		glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(GL_FLOAT), &normals[0], GL_STATIC_DRAW); GL_CHECK_ERRORS;
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (GLvoid*)0); GL_CHECK_ERRORS;
		glEnableVertexAttribArray(1); GL_CHECK_ERRORS;

		//передаем в шейдерную программу атрибут текстурных координат
		glBindBuffer(GL_ARRAY_BUFFER, vboTexCoords); GL_CHECK_ERRORS;
		glBufferData(GL_ARRAY_BUFFER, texcoords.size() * sizeof(GL_FLOAT), &texcoords[0], GL_STATIC_DRAW); GL_CHECK_ERRORS;
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GL_FLOAT), (GLvoid*)0); GL_CHECK_ERRORS;
		glEnableVertexAttribArray(2); GL_CHECK_ERRORS;

		//передаем в шейдерную программу индексы
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndices); GL_CHECK_ERRORS;
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW); GL_CHECK_ERRORS;

		//glEnable(GL_PRIMITIVE_RESTART); GL_CHECK_ERRORS;
		//glPrimitiveRestartIndex(primRestart); GL_CHECK_ERRORS;
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}


int Shape::ReadFromFile()
{
	return 0;
}


void Shape::CreateFlat(int width, int height)
{
	int startvert = verts.size();
	int startind = indices.size();
	int starttext = texcoords.size();
	int startnorm = normals.size();


	std::vector<std::vector<float>> v(height);
	for (size_t i = 0; i < v.size(); i++)
		v[i].resize(width);

	v[0][0] = 0;
	v[0][width - 1] = 0;
	v[height - 1][0] = 0;
	v[height - 1][width - 1] = 0;


	v[height / 2][width / 2] = 20;

	/*
	float randfactor = width * 5.0e-4;

	int zstep = height - 1;
	int xstep = width - 1;

	while (zstep > 1 && xstep > 1)
	{
		for (int top = 0; top < height - 1; top += zstep)
		{
			for (int left = 0; left < width - 1; left += xstep)
			{
				v[top + zstep / 2][left + xstep / 2] +=
					(v[top][left] + v[top][left + xstep] + v[top + zstep][left] + v[top + zstep][left + xstep]) / 4 +
					(float(rand()) / RAND_MAX - 0.5) * xstep * randfactor;
			}
		}

		
		for (int top = 0; top < height - 1; top += zstep)
		{
			for (int left = 0; left < width - 1; left += xstep)
			{
				float nul = 0;
				float& center = v[top + zstep / 2][left + xstep / 2];
				float& outtop = top - zstep / 2 < 0 ? center : v[top - zstep / 2][left + xstep / 2];
				float& outleft = left - xstep / 2 < 0 ? center : v[top + zstep / 2][left - xstep / 2];
				float& outbottom = top + 3 * zstep / 2 >= height ? center : v[top + 3 * zstep / 2][left + xstep / 2];
				float& outright = left + 3 * xstep / 2 >= width ? center : v[top + zstep / 2][left + 3 * xstep / 2];

				v[top][left + xstep / 2] =
					(v[top][left] + v[top][left + xstep] + center + outtop) / 4 +
					(float(rand()) / RAND_MAX - 0.5) * xstep * randfactor;

				v[top + zstep / 2][left] =
					(v[top][left] + v[top + zstep][left] + center + outleft) / 4 +
					(float(rand()) / RAND_MAX - 0.5) * xstep * randfactor;

				v[top + zstep][left + xstep / 2] =
					(v[top + zstep][left] + v[top + zstep][left + xstep] + center + outbottom) / 4 +
					(float(rand()) / RAND_MAX - 0.5) * xstep * randfactor;

				v[top + zstep / 2][left + xstep] =
					(v[top][left + xstep] + v[top + zstep][left + xstep] + center + outright) / 4 +
					(float(rand()) / RAND_MAX - 0.5) * xstep * randfactor;
			}
		}

		zstep /= 2;
		xstep /= 2;
	}
	*/
	// Make crater
	/*
	int size = 9;
	for (int z = -size; z <= size; z ++)
	for (int x = -size; x <= size; x++)
		v[height/2 + z][width/2 + x] /= 2;
		*/

	for (int z = 0; z < height; z++)
	{
		float texty = float(z) / (height - 1);
		float vertz = z - (float)height / 2;
		int zIndex = startvert + z * width * 3;

		for (int x = 0; x < width; x++)
		{
			verts.push_back(x - (float)width / 2);
			verts.push_back(v[z][x]);
			verts.push_back(vertz);

			texcoords.push_back(texty);
			texcoords.push_back(float(x) / (width - 1));
			
		}
	}


	for (int y = 0; y < height - 1; y++)
	{
		int curstrip = y * width;
		int nextstrip = (y + 1) * width;
		for (int x = 0; x < width - 1; x++)
		{
			indices.push_back(startvert + nextstrip + x);
			indices.push_back(startvert + curstrip + x + 1);
			indices.push_back(startvert + curstrip + x);

			indices.push_back(startvert + nextstrip + x);
			indices.push_back(startvert + nextstrip + x + 1);
			indices.push_back(startvert + curstrip + x + 1);
		}
	}

	// Create chunk
	BufferChunk chunk;
	chunk.start = startind;
	chunk.count = indices.size() - startind;
	chunk.type = GL_TRIANGLES;
	chunks.push_back(chunk);
}

void Shape::CreatePyramid()
{
}

void Shape::Finalize()
{
	CalcNormalsTriangles(0, 0);
	CreateVertexBuffer();
}

void Shape::Draw()
{
	for (size_t i = 0; i < chunks.size(); i++)
	{
		glDrawElements(chunks[i].type, chunks[i].count, GL_UNSIGNED_INT, (GLvoid*)(sizeof(GLuint)* chunks[i].start)); GL_CHECK_ERRORS;
	}
}
