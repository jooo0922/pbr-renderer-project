#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

// 카메라 이동 관련 상수들
enum Camera_Movement
{
  FORWARD,
  BACKWARD,
  LEFT,
  RIGHT
};

// 카메라 기본값 상수들
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

class Camera
{
public:
  // 카메라 벡터들
  glm::vec3 Position;
  glm::vec3 Front;
  glm::vec3 Up;
  glm::vec3 Right;
  glm::vec3 WorldUp;

  // 오일러 각
  float Yaw;
  float Pitch;

  // 이동 속도, 마우스 감도, 줌 레벨
  float MovementSpeed;
  float MouseSensitivity;
  float Zoom;

  // 생성자들
  Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH);
  Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);

  // 뷰 행렬 반환
  glm::mat4 GetViewMatrix();

  // 키보드 입력 처리
  void ProcessKeyboard(Camera_Movement direction, float deltaTime);

  // 마우스 이동 처리
  void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);

  // 마우스 스크롤 처리
  void ProcessMouseScroll(float yoffset);

private:
  // 카메라 벡터들 업데이트
  void updateCameraVectors();
};

#endif // CAMERA_H
