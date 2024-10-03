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
  glm::mat4 getViewMatrix();

  // 카메라 position getter
  glm::vec3 getCameraPosition() const;

  // 카메라 yaw getter
  float getCameraYaw() const;

  // 카메라 pitch getter
  float getCameraPitch() const;

  // 카메라 zoom getter
  float getCameraZoom() const;

  // 카메라 이동 처리
  void processCameraMove(Camera_Movement direction, float deltaTime);

  // 카메라 회전 처리
  void processCameraRotate(float xoffset, float yoffset, GLboolean constrainPitch = true);

  // 카메라 줌 처리
  void processCameraZoom(float yoffset);

  // 카메라 yaw 설정
  void setCameraYaw(const float yaw);

  // 키메라 pitch 설정
  void setCameraPitch(const float pitch, GLboolean constrainPitch = true);

  // 카메라 zoom 설정
  void setCameraZoom(const float zoom);

  // 카메라 position 설정
  void setCameraPosition(const glm::vec3 &position);

private:
  // 카메라 벡터들
  glm::vec3 position;
  glm::vec3 front;
  glm::vec3 up;
  glm::vec3 right;
  glm::vec3 worldUp;

  // 오일러 각
  float yaw;
  float pitch;

  // 이동 속도, 마우스 감도, 줌 레벨
  float movementSpeed;
  float mouseSensitivity;
  float zoom;

  // 카메라 벡터들 업데이트
  void updateCameraVectors();
};

#endif // CAMERA_H
