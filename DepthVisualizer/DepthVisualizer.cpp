// DepthVisualizer.cpp : Defines the entry point for the console application.
#include "ShaderProgram.h"
#include "Camera.h"
#include <vector>
#include <algorithm>

#include <glad/glad.h>
#include <glfw/glfw3.h>
#include "Shape.h"
#include "shadowmap.h"

#include <glm/gtx/transform.hpp>

GLFWwindow* window;
using namespace glm;

static bool bWrite = false;
static int filling = 0;
static bool keys[1024]; //массив состояний кнопок - нажата/не нажата
static GLfloat lastX = 400, lastY = 300; //исходное положение мыши
static bool firstMouse = true;
static bool g_captureMouse = true;  // Мышка захвачена нашим приложением или нет?
static bool g_capturedMouseJustNow = false;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

glm::mat4 bias(
	0.5, 0.0, 0.0, 0.0,
	0.0, 0.5, 0.0, 0.0,
	0.0, 0.0, 0.5, 0.0,
	0.5, 0.5, 0.5, 1.0
	);


Camera camera(glm::vec3(0, 50.1, 0));// , vec3(0, 0, -1), normalize(vec3(0, -1, 0)));

std::string dirnameOf(const std::string& fname)
{
	size_t pos = fname.find_last_of("\\/");
	return (std::string::npos == pos)
		? ""
		: fname.substr(0, pos);
}

//функция для обработки нажатий на кнопки клавиатуры
void OnKeyboardPressed(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	//std::cout << key << std::endl;
	switch (key)
	{
	case GLFW_KEY_ESCAPE: //на Esc выходим из программы
		if (action == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GL_TRUE);
		break;
	case GLFW_KEY_SPACE: //на пробел переключение в каркасный режим и обратно
		if (action == GLFW_PRESS)
		{
			if (filling == 0)
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				filling = 1;
			}
			else
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				filling = 0;
			}
		}
		break;
	case GLFW_KEY_3:
		if (action == GLFW_PRESS)
			bWrite = true;
		break;
	case GLFW_KEY_1:
		//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		break;
	case GLFW_KEY_2:
		//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		break;
	default:
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}
}

//функция для обработки клавиш мыши
void OnMouseButtonClicked(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
		g_captureMouse = !g_captureMouse;


	if (g_captureMouse)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		g_capturedMouseJustNow = true;
	}
	else
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

}

//функция для обработки перемещения мыши
void OnMouseMove(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = float(xpos);
		lastY = float(ypos);
		firstMouse = false;
	}

	GLfloat xoffset = float(xpos) - lastX;
	GLfloat yoffset = lastY - float(ypos);

	lastX = float(xpos);
	lastY = float(ypos);

	if (g_captureMouse)
		camera.ProcessMouseMove(xoffset, yoffset);
}


void OnMouseScroll(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(GLfloat(yoffset));
}

void doCameraMovement(Camera &camera, GLfloat deltaTime)
{
	if (keys[GLFW_KEY_W])
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (keys[GLFW_KEY_A])
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (keys[GLFW_KEY_S])
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (keys[GLFW_KEY_D])
		camera.ProcessKeyboard(RIGHT, deltaTime);
}


int CreateShadowMatrix(int window, vec3& lightPos, vec3& lightDir, vec3& lightUp, mat4& depthMat)
{
	glm::mat4 depthProjections = glm::ortho<float>(-window / 2, window / 2, -window / 2, window / 2, -window / 2,  window);

	glm::mat4 depthViewMatrix = glm::lookAt(lightPos, lightDir, lightUp);
	depthMat = depthProjections * depthViewMatrix;// *depthModelMatrix;

	return 0;
}


int main(int argc, char* argv[])
{
	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return -1;
	}

	//glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	// We would expect width and height to be 1024 and 768
	int windowWidth = 1024;
	int windowHeight = 768;

	window = glfwCreateWindow(windowWidth, windowHeight, "Tutorial 16 - Shadows", NULL, NULL);
	if (window == NULL){
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	//регистрируем коллбеки для обработки сообщений от пользователя - клавиатура, мышь..
	glfwSetKeyCallback(window, OnKeyboardPressed);
	glfwSetCursorPosCallback(window, OnMouseMove);
	glfwSetMouseButtonCallback(window, OnMouseButtonClicked);
	glfwSetScrollCallback(window, OnMouseScroll);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize OpenGL context" << std::endl;
		return -1;
	}

	// But on MacOS X with a retina screen it'll be 1024*2 and 768*2, so we get the actual framebuffer size:
	glfwGetFramebufferSize(window, &windowWidth, &windowHeight);

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	// Hide the mouse and enable unlimited mouvement
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	std::string exeDir = dirnameOf(argv[0]);
	if (!exeDir.empty())
	{
		exeDir += "/";
	}

	std::unordered_map<GLenum, std::string> shaders;
	shaders[GL_VERTEX_SHADER] = exeDir + "main.vsh";
	shaders[GL_FRAGMENT_SHADER] = exeDir + "main.fsh";
	ShaderProgram programMain(shaders); GL_CHECK_ERRORS;

	shaders[GL_VERTEX_SHADER] = exeDir + "depthcreate.vsh";
	shaders[GL_FRAGMENT_SHADER] = exeDir + "depthcreate.fsh";
	ShaderProgram programDepthCreate(shaders); GL_CHECK_ERRORS;

	shaders[GL_VERTEX_SHADER] = exeDir + "depthShow.vsh";
	shaders[GL_FRAGMENT_SHADER] = exeDir + "depthShow.fsh";
	ShaderProgram programDepthShow(shaders); GL_CHECK_ERRORS;

	shaders[GL_VERTEX_SHADER] = exeDir + "mainshadow.vsh";
	shaders[GL_FRAGMENT_SHADER] = exeDir + "mainshadow.fsh";
	ShaderProgram programMainShow(shaders); GL_CHECK_ERRORS;

	glm::vec3 lightPos(100, 100, 0);
	glm::vec3 lightdir(-1, -1, 0);

	Shape shape;
	shape.CreateFlat(513, 513);
	shape.Finalize();


	ShadowMap shadowmap(windowWidth * 4, windowHeight * 4);
	shadowmap.Prepare();

	glm::mat4 modelMat({ 1.0 });
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f); GL_CHECK_ERRORS;
	glEnable(GL_DEPTH_TEST);  GL_CHECK_ERRORS;

	do
	{
		//считаем сколько времени прошло за кадр
		GLfloat currentFrame = GLfloat(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glfwPollEvents();
		doCameraMovement(camera, deltaTime);

		// Create marices
		// Matrices
		glm::mat4 viewMat = camera.GetViewMatrix();
		//glm::mat4 viewMat = glm::lookAt(vec3(-0.5, 21, -0.5), vec3(-0.5, 0, -0.5), vec3(0, 0, 1));

		glm::mat4 proectionMat = glm::perspective(float(camera.zoom / 180.0f * M_PI), float(windowWidth / windowHeight), 0.1f, 1000.0f);
		glm::mat3 normalMat = glm::mat3(transpose(inverse(modelMat)));
		glm::vec3 lightPosNormalized = normalize(lightPos);
		glm::vec3 lightDirNormalized = normalize(lightdir);

		glm::mat4 shadowMat;
		//CreateShadowMatrix(1024, camera.pos, camera.pos + camera.front, camera.up, shadowMat);

		glm::mat4 lightMat = glm::lookAt(lightPos, lightPos + lightDirNormalized, vec3(0, 1, 0));

		shadowMat = proectionMat * lightMat;
		vec4 t(-0.5f, 20.0f, -0.5f, 1.0);
		vec4 t1 = viewMat * t;
		vec4 t1_2 = proectionMat * t1;

		float t2 = t1.z / t1.w;

		vec4 t11(-0.5f, 20.0f, 1.0f, 1.0);
		vec4 t12 = proectionMat * t11;
		float t13 = t12.z / t12.w;



		int a;
		a = 0;

		glBindFramebuffer(GL_FRAMEBUFFER, shadowmap.GetDepthFBO());
		glViewport(0, 0, windowWidth * 4, windowHeight * 4);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK); // Cull back-facing triangles -> draw only front-facing triangles


		/////////////////////////////////////////////
		programDepthCreate.StartUseShader();
		// Set uniforms
		programDepthCreate.SetUniform("ShadowMat", shadowMat);

		glBindVertexArray(shape.vao); GL_CHECK_ERRORS;
		shape.Draw(); GL_CHECK_ERRORS;
		glBindVertexArray(0); GL_CHECK_ERRORS;

		programDepthCreate.StopUseShader();
		/////////////////////////////////////////////

		/*
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, shadowmap.GetDepthFBO());
		glBlitFramebuffer(0, 0, windowWidth, windowHeight, 0, 0, windowWidth, windowHeight,
			GL_DEPTH_BUFFER_BIT, GL_NEAREST);
			*/

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, windowWidth, windowHeight);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK); // Cull back-facing triangles -> draw only front-facing triangles


		/*
		/////////////////////////////////////////////
		programMain.StartUseShader();
		// Set uniforms
		programMain.SetUniform("ModelMat", modelMat);
		programMain.SetUniform("NormalMat", normalMat);
		programMain.SetUniform("VPMat", proectionMat * lightMat);
		programMain.SetUniform("LightDir", -lightPosNormalized);

		glBindVertexArray(shape.vao); GL_CHECK_ERRORS;
		shape.Draw(); GL_CHECK_ERRORS;
		glBindVertexArray(0); GL_CHECK_ERRORS;

		programMain.StopUseShader();
		/////////////////////////////////////////////

		if (bWrite)
		{
			std::vector<float> buf(windowWidth * windowHeight);
			glBindTexture(GL_TEXTURE_2D, shadowmap.GetDepthTexture());
			glGetTexImage(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, GL_FLOAT, &buf[0]);

			float m = 10000;
			int imin = 0, jmin = 0;

			FILE* fp = fopen("d:\\tmp\\pixels.txt", "a");
			for (int i = 0; i < windowHeight; i++)
			{
				int offset = i * windowWidth;
				for (int j = 0; j < windowWidth; j++)
				{
					if (buf[offset + j] < m)
					{
						m = buf[offset + j];
						imin = i;
						jmin = j;
					}
					//fprintf(fp, "%06.3f ", buf[offset + j] * 100);
				}
				//fprintf(fp, "\n");
			}
			
			fclose(fp);
			bWrite = false;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, windowWidth, windowHeight);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//glEnable(GL_CULL_FACE);
		//glCullFace(GL_BACK); // Cull back-facing triangles -> draw only front-facing triangles

		/////////////////////////////////////////////
		programDepthShow.StartUseShader();
		// Set uniforms
		programDepthShow.SetUniform("MVPMat", proectionMat * viewMat);
		programDepthShow.SetUniform("BiasShadowMat", proectionMat * viewMat);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, shadowmap.GetDepthTexture());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

		programDepthShow.SetUniform("depthTexture", 0); GL_CHECK_ERRORS;


		glBindVertexArray(shape.vao); GL_CHECK_ERRORS;
		shape.Draw(); GL_CHECK_ERRORS;
		glBindVertexArray(0); GL_CHECK_ERRORS;
		programDepthShow.StopUseShader();
		*/
		/////////////////////////////////////////////
		programMainShow.StartUseShader();
		// Set uniforms
		programMainShow.SetUniform("ModelMat", modelMat);
		programMainShow.SetUniform("NormalMat", normalMat);
		programMainShow.SetUniform("VPMat", proectionMat * viewMat);
		programMainShow.SetUniform("ShadowMat", proectionMat * lightMat);
		programMainShow.SetUniform("LightDir", -lightPosNormalized);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, shadowmap.GetDepthTexture());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

		programMainShow.SetUniform("depthTexture", 0); GL_CHECK_ERRORS;


		glBindVertexArray(shape.vao); GL_CHECK_ERRORS;
		shape.Draw(); GL_CHECK_ERRORS;
		glBindVertexArray(0); GL_CHECK_ERRORS;
		programMainShow.StopUseShader();
		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
	glfwWindowShouldClose(window) == 0);

	glBindVertexArray(0);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

