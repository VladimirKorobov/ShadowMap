// ShadowMap.cpp : Defines the entry point for the console application.
#ifndef __SHAPE_H_
#define  __SHAPE_H_
#include "common.h"
#include <vector>

using namespace glm;

struct BufferChunk
{
	GLuint start;
	GLuint count;
	GLuint type;

};

class Shape
{
public:
	int ReadFromFile();
	void CreateSphere(float r, float zoffset, int phisteps, int tetasteps,
		float textleft,
		float texttop,
		float textright,
		float textbottom,
		float scale);
	void CreatePyramid();
	void CreateFlat(int width, int height);
	void Finalize();

	void Draw();

	GLuint vao;

private:
	void CalcNormalsTriangles(
		int startvert,
		int startind);

	void CreateVertexBuffer();

	std::vector<GLfloat> verts;
	std::vector<GLfloat> normals;
	std::vector<GLfloat> texcoords;
	std::vector<GLuint> indices;
	std::vector<BufferChunk> chunks;

	std::vector<GLuint> dupnormals;

};

#endif // __SHAPE_H_