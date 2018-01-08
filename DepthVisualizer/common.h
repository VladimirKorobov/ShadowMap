#ifndef __COMMON_H
#define __COMMON_H
#define _USE_MATH_DEFINES
#include <math.h>
#include <glad/glad.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <glfw/glfw3.h>

#include <glm/glm.hpp>

using namespace glm;

#define GL_CHECK_ERRORS ThrowExceptionOnGLError(__LINE__,__FILE__)
static void ThrowExceptionOnGLError(int line, const char *file)
{

	static char errMsg[512];

	//���������� ������� glGetError, ����������� �� ��������� �� ������
	//� �����-�� ������ opengl � ���� ���������, �� ����� ��� � ������
	GLenum gl_error = glGetError();

	if (gl_error == GL_NO_ERROR)
		return;

	switch (gl_error)
	{
	case GL_INVALID_ENUM:
		std::cerr << "GL_INVALID_ENUM file " << file << " line " << line << std::endl;
		break;

	case GL_INVALID_VALUE:
		std::cerr << "GL_INVALID_VALUE file " << file << " line " << line << std::endl;
		break;

	case GL_INVALID_OPERATION:
		std::cerr << "GL_INVALID_OPERATION file " << file << " line " << line << std::endl;
		break;

	case GL_STACK_OVERFLOW:
		std::cerr << "GL_STACK_OVERFLOW file " << file << " line " << line << std::endl;
		break;

	case GL_STACK_UNDERFLOW:
		std::cerr << "GL_STACK_UNDERFLOW file " << file << " line " << line << std::endl;
		break;

	case GL_OUT_OF_MEMORY:
		std::cerr << "GL_OUT_OF_MEMORY file " << file << " line " << line << std::endl;
		break;

	case GL_NO_ERROR:
		break;

	default:
		std::cerr << "Unknown error @ file " << file << " line " << line << std::endl;
		break;
	}

	if (gl_error != GL_NO_ERROR)
		throw std::runtime_error(errMsg);
}

#endif //#ifndef __COMMON_H