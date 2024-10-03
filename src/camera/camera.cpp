#include "camera/camera.hpp"

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
    : front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM)
{
  this->position = position;
  this->worldUp = up;
  this->yaw = yaw;
  this->pitch = pitch;
  updateCameraVectors();
}

Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
    : front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM)
{
  this->position = glm::vec3(posX, posY, posZ);
  this->worldUp = glm::vec3(upX, upY, upZ);
  this->yaw = yaw;
  this->pitch = pitch;
  updateCameraVectors();
}

glm::mat4 Camera::getViewMatrix()
{
  return glm::lookAt(position, position + front, up);
}

glm::vec3 Camera::getCameraPosition() const
{
  return position;
}

float Camera::getCameraYaw() const
{
  return yaw;
}

float Camera::getCameraPitch() const
{
  return pitch;
}

float Camera::getCameraZoom() const
{
  return zoom;
}

void Camera::processCameraMove(Camera_Movement direction, float deltaTime)
{
  float velocity = movementSpeed * deltaTime;
  if (direction == FORWARD)
    position += front * velocity;
  if (direction == BACKWARD)
    position -= front * velocity;
  if (direction == LEFT)
    position -= right * velocity;
  if (direction == RIGHT)
    position += right * velocity;
}

void Camera::processCameraRotate(float xoffset, float yoffset, GLboolean constrainPitch)
{
  xoffset *= mouseSensitivity;
  yoffset *= mouseSensitivity;

  yaw += xoffset;
  pitch += yoffset;

  if (constrainPitch)
  {
    if (pitch > 89.0f)
      pitch = 89.0f;
    if (pitch < -89.0f)
      pitch = -89.0f;
  }

  updateCameraVectors();
}

void Camera::processCameraZoom(float yoffset)
{
  zoom -= (float)yoffset;
  if (zoom < 1.0f)
    zoom = 1.0f;
  if (zoom > 45.0f)
    zoom = 45.0f;
}

void Camera::setCameraYaw(const float yaw)
{
  this->yaw = yaw;

  updateCameraVectors();
}

void Camera::setCameraPitch(const float pitch, GLboolean constrainPitch)
{
  this->pitch = pitch;

  if (constrainPitch)
  {
    if (this->pitch > 89.0f)
      this->pitch = 89.0f;
    if (this->pitch < -89.0f)
      this->pitch = -89.0f;
  }

  updateCameraVectors();
}

void Camera::setCameraZoom(const float zoom)
{
  this->zoom = zoom;

  if (this->zoom < 1.0f)
    this->zoom = 1.0f;
  if (this->zoom > 45.0f)
    this->zoom = 45.0f;
}

void Camera::setCameraPosition(const glm::vec3 &position)
{
  this->position = position;
}

void Camera::updateCameraVectors()
{
  glm::vec3 direction;
  direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
  direction.y = sin(glm::radians(pitch));
  direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
  front = glm::normalize(direction);

  right = glm::normalize(glm::cross(front, worldUp));
  up = glm::normalize(glm::cross(right, front));
}
