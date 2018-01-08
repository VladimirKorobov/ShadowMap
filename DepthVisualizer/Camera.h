#ifndef CAMERA_H
#define CAMERA_H

///////////////////////
// Camera class based on samples from learnopengl.com

#include "common.h"
#include <glm/gtc/matrix_transform.hpp>

enum Movement_Direction
{
  FORWARD,
  BACKWARD,
  LEFT,
  RIGHT
};

static float DEG_TO_RAD = 3.1415926535897932384626433832795f / 180.0f;

class Camera
{
public:

  glm::vec3 pos;
  glm::vec3 front;
  glm::vec3 up;
  glm::vec3 right;

  GLfloat yaw;
  GLfloat pitch;

  GLfloat moveSpeed;
  GLfloat mouseSensitivity;
  GLfloat zoom;

  Camera(glm::vec3 a_pos = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 a_up = glm::vec3(0.0f, 1.0f, 0.0f),
         glm::vec3 a_front = glm::vec3(0.0f, 0.0f, -1.0f), GLfloat a_yaw = -90.0f, GLfloat a_pitch = 0.0f,
         GLfloat a_moveSpeed = 50.0f, GLfloat a_mouseSensitivity = 0.1f, GLfloat a_zoom = 45.0f);

  virtual ~Camera() {};

  glm::mat4 GetViewMatrix() const;

  void ProcessKeyboard(Movement_Direction dir, GLfloat deltaTime);
  void ProcessMouseMove(GLfloat deltaX, GLfloat deltaY, GLboolean limitPitch = true);
  void ProcessMouseScroll(GLfloat deltaY);

private:
  void updateCameraVectors();
  glm::vec3 worldUp;

};


#endif