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
  // 생성자들
  Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH);
  Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);

  // 뷰 행렬 반환
  glm::mat4 GetViewMatrix();

  // 카메라 Position getter
  glm::vec3 GetCameraPosition() const;

  // 카메라 Yaw getter
  float GetCameraYaw() const;

  // 카메라 Pitch getter
  float GetCameraPitch() const;

  // 카메라 Zoom getter
  float GetCameraZoom() const;

  // 카메라 이동 처리
  void ProcessCameraMove(Camera_Movement direction, float deltaTime);

  // 카메라 회전 처리
  void ProcessCameraRotate(float xoffset, float yoffset, GLboolean constrainPitch = true);

  // 카메라 줌 처리
  void ProcessCameraZoom(float yoffset);

  // 카메라 Yaw 설정
  void SetCameraYaw(const float yaw);

  // 키메라 Pitch 설정
  void SetCameraPitch(const float pitch, GLboolean constrainPitch = true);

  // 카메라 Zoom 설정
  void SetCameraZoom(const float zoom);

  // 카메라 위치 설정
  void SetCameraPosition(const glm::vec3 &position);

private:
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

  // 카메라 벡터들 업데이트
  void updateCameraVectors();
};

#endif // CAMERA_H
