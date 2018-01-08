// ShadowMap.cpp : Defines the entry point for the console application.
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

Camera camera(glm::vec3(0, 100, 0));// , vec3(0, 0, 1), normalize(vec3(0, -1, 0)));


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

int main( int argc, char** argv )
{
	// Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// We would expect width and height to be 1024 and 768
	int windowWidth = 1024;
	int windowHeight = 768;

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(windowWidth, windowHeight, "Tutorial 16 - Shadows", NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
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

	shaders[GL_VERTEX_SHADER] = exeDir + "water.vsh";
	shaders[GL_FRAGMENT_SHADER] = exeDir + "water.fsh";
	ShaderProgram programWater(shaders); GL_CHECK_ERRORS;

	shaders[GL_VERTEX_SHADER] = exeDir + "watershadow.vsh";
	shaders[GL_FRAGMENT_SHADER] = exeDir + "watershadow.fsh";
	ShaderProgram programWaterShadow(shaders); GL_CHECK_ERRORS;

	shaders[GL_VERTEX_SHADER] = exeDir + "depth.vsh";
	shaders[GL_FRAGMENT_SHADER] = exeDir + "depth.fsh";
	ShaderProgram programDepth(shaders); GL_CHECK_ERRORS;


	glm::vec3 lightPos(100, 1000, 100);

	Shape shape;
	shape.CreateFlat(513, 513);

	shape.Finalize();

	ShadowMap shadowMap(windowWidth, windowHeight);
	shadowMap.Prepare();

	glm::mat4 modelMat({ 1.0 });
	
	glEnable(GL_DEPTH_TEST);  GL_CHECK_ERRORS;
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); GL_CHECK_ERRORS;

	do
	{
		//считаем сколько времени прошло за кадр
		GLfloat currentFrame = GLfloat(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glfwPollEvents();
		doCameraMovement(camera, deltaTime);

		//shadowMap.Clear();

		// Matrices
		glm::mat4 viewMat = camera.GetViewMatrix();
		glm::mat4 proectionMat = glm::perspective(float(camera.zoom / 180.0f * M_PI), float(windowWidth / windowHeight), 0.1f, 1000.0f);
		glm::mat3 normalMat = glm::mat3(transpose(inverse(modelMat)));
		
		glm::mat4 MVMatrix = viewMat * modelMat;

		glm::vec3 lightPosNormalized = normalize(lightPos);

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glDisable(GL_BLEND);

		glViewport(0, 0, windowWidth, windowHeight);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		/////////////////////////////////////////////////////////////////////////////////////////////////////
		programMain.StartUseShader();

		glDisable(GL_BLEND);

		programMain.SetUniform("ModelMat", modelMat);
		programMain.SetUniform("NormalMat", normalMat);
		programMain.SetUniform("VPMat", proectionMat * viewMat);
		programMain.SetUniform("LightDir", -lightPosNormalized);

		glBindVertexArray(shape.vao); GL_CHECK_ERRORS;
		shape.Draw(); GL_CHECK_ERRORS;
		glBindVertexArray(0); GL_CHECK_ERRORS;
		programMain.StopUseShader();
		/////////////////////////////////////////////////////////////////////////////////////////////////////

		// Setup wave
		/////////////////////////////////////////////////////////////////////////////////////////////////////
		static float ph = 0;
		ph += 0.05;

		float waterLevel = -20;// +5 * sin(ph);
		vec3 waterNorm = vec3(0, 1, 0);

		vec3 newWaternorm1 = vec3(MVMatrix * vec4(waterLevel * waterNorm, 1.0f));
		vec3 newWaternorm2 = vec3(MVMatrix * vec4((waterLevel + 1) * waterNorm, 1.0f));
		vec3 newWaternorm = normalize(newWaternorm2 - newWaternorm1);
		vec4 waterplane(newWaternorm, dot(newWaternorm1, newWaternorm));

		vec3 waveVect1 = vec3(MVMatrix * vec4(waterLevel * waterNorm, 1.0f));
		vec3 waveVect2 = vec3(MVMatrix * (vec4(waterLevel * waterNorm + vec3(1, 0, 0), 1.0f)));
		vec3 waveVect = normalize(waveVect2 - waveVect1);
		vec3 waveOffset = vec3(MVMatrix * vec4(0, 0, 0, 1));
		/////////////////////////////////////////////////////////////////////////////////////////////////////

		// Water shadow
		/////////////////////////////////////////////////////////////////////////////////////////////////////
		glBindFramebuffer(GL_FRAMEBUFFER, shadowMap.GetDepthFBO());
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		programWaterShadow.StartUseShader();

		programWaterShadow.SetUniform("ModelMat", modelMat);
		programWaterShadow.SetUniform("ProjectionMat", proectionMat);
		programWaterShadow.SetUniform("ViewMat", viewMat);

		programWaterShadow.SetUniform("WaterPlane", waterplane);
		programWaterShadow.SetUniform("WaveDirection", waveVect);
		programWaterShadow.SetUniform("WaveOffset", waveOffset);
		programWaterShadow.SetUniform("Ph", ph);

		glBindVertexArray(shape.vao); GL_CHECK_ERRORS;
		shape.Draw(); GL_CHECK_ERRORS;
		glBindVertexArray(0); GL_CHECK_ERRORS;

		programWaterShadow.StopUseShader();


		/////////////////////////////////////////////////////////////////////////////////////////////////////

		// Copy depth
		/*
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, shadowMap.GetDepthFBO());
		glBlitFramebuffer(0, 0, windowWidth, windowHeight, 0, 0, windowWidth, windowHeight,
			GL_DEPTH_BUFFER_BIT, GL_NEAREST);
		*/

		// Water
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		programWater.StartUseShader();

		programWater.SetUniform("ModelMat", modelMat);
		programWater.SetUniform("NormalMat", normalMat);
		programWater.SetUniform("ProjectionMat", proectionMat);
		programWater.SetUniform("ViewMat", viewMat);

		programWater.SetUniform("LightDir", -lightPosNormalized);
		programWater.SetUniform("WaterPlane", waterplane);
		programWater.SetUniform("WaveDirection", waveVect);
		programWater.SetUniform("WaveOffset", waveOffset);
		programWater.SetUniform("Ph", ph);
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, shadowMap.GetDepthTexture());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
		programWater.SetUniform("depthTexture", 0); GL_CHECK_ERRORS;

		glBindVertexArray(shape.vao); GL_CHECK_ERRORS;
		shape.Draw(); GL_CHECK_ERRORS;
		glBindVertexArray(0); GL_CHECK_ERRORS;

		programWater.StopUseShader();

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

	glBindVertexArray(0);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

