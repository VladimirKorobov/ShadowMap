#include "Camera.h"

Camera::Camera(glm::vec3 a_pos, glm::vec3 a_up, glm::vec3 a_front, GLfloat a_yaw, GLfloat a_pitch,
               GLfloat a_moveSpeed, GLfloat a_mouseSensitivity, GLfloat a_zoom) : pos(a_pos), front(a_front), up(a_up),
               yaw(a_yaw), pitch(a_pitch), moveSpeed(a_moveSpeed), mouseSensitivity(a_mouseSensitivity), zoom(a_zoom)
{
  right   = normalize(cross(a_up, a_front));
  worldUp = up;//normalize(a_pos);
  yaw = 90;
}



glm::mat4 Camera::GetViewMatrix() const
{
  return glm::lookAt(pos, pos + front, up);
}

void Camera::ProcessKeyboard(Movement_Direction dir, GLfloat deltaTime)
{
  GLfloat velocity = moveSpeed * deltaTime;
  if (dir == FORWARD)
    pos += (front /*+ up * 0.5f*/) * velocity;
  if (dir == BACKWARD)
	  pos -= (front /*+ up * 0.5f*/) * velocity;
  if (dir == LEFT)
    pos -= right * velocity;
  if (dir == RIGHT)
    pos += right * velocity;

  //worldUp = normalize(pos);

}

void Camera::ProcessMouseMove(GLfloat deltaX, GLfloat deltaY, GLboolean limitPitch)
{
  deltaX *= mouseSensitivity;
  deltaY *= mouseSensitivity;

  yaw += deltaX;
  pitch += deltaY;

  if (limitPitch)
  {
    if (pitch > 89.0f)
      pitch = 89.0f;
    if (pitch < -89.0f)
      pitch = -89.0f;
  }

  updateCameraVectors();
}

void Camera::ProcessMouseScroll(GLfloat deltaY)
{
  if (zoom >= 1.0f && zoom <= 45.0f)
    zoom -= deltaY;
  if (zoom <= 1.0f)
    zoom = 1.0f;
  if (zoom >= 45.0f)
    zoom = 45.0f;
}

void Camera::updateCameraVectors()
{
	/*
  glm::vec3 tmpFront;

  tmpFront.x = cos(DEG_TO_RAD*yaw) * cos(DEG_TO_RAD*pitch);
  tmpFront.y = sin(DEG_TO_RAD*pitch);
  tmpFront.z = sin(DEG_TO_RAD*yaw) * cos(DEG_TO_RAD*pitch);

  glm::dvec3 dn(front.x, front.y, front.z);
  glm::dvec3 dup(worldUp.x, worldUp.y, worldUp.z);
  glm::dvec3 du = glm::normalize(glm::cross(dup, dn));
  glm::dvec3 dv = glm::normalize(glm::cross(dn, du));
  glm::dmat3 mat3(du, dv, dn);
  glm::dvec3 tmp = mat3 * glm::dvec3(tmpFront.x, tmpFront.y, tmpFront.z);

  front = normalize(glm::vec3(float(tmp.x), float(tmp.y), float(tmp.z)));
  right = normalize(cross(front, worldUp));
  up    = normalize(cross(right, front));

  pitch = 0;
  yaw = 90;
  */
	glm::vec3 tmpFront;

	tmpFront.x = cos(DEG_TO_RAD*yaw) * cos(DEG_TO_RAD*pitch);
	tmpFront.y = sin(DEG_TO_RAD*pitch);
	tmpFront.z = sin(DEG_TO_RAD*yaw) * cos(DEG_TO_RAD*pitch);

	front = normalize(tmpFront);
	right = normalize(cross(front, worldUp));
	up = normalize(cross(right, front));
}