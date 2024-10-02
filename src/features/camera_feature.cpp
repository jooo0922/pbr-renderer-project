#include "features/camera_feature.hpp"

CameraFeature::CameraFeature()
    : camera(Camera()),
      pbrShaderPtr(nullptr),
      backgroundShaderPtr(nullptr)
{
}

void CameraFeature::initialize()
{
  // CameraUi 에서 관리되는 각 ImGui 요소에 입력할 초기값 설정
  // TODO : 추후에 초기값을 별도 파일에 constant 로 추출하여 정리할 것.
  cameraParameter.yaw = -90.0f;
  cameraParameter.pitch = 0.0f;
  cameraParameter.zoom = 45.0f;
  cameraParameter.position = glm::vec3(0.0f, 0.0f, 3.0f);
}

void CameraFeature::process()
{
  // 카메라의 zoom 값으로부터 투영 행렬 계산
  glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);

  // 카메라 클래스로부터 뷰 행렬(= LookAt 행렬) 가져오기
  glm::mat4 view = camera.GetViewMatrix();

  // pbrShader 쉐이더 프로그램 바인딩 및 현재 카메라의 projection 및 view 행렬 전송
  pbrShaderPtr->use();
  pbrShaderPtr->setMat4("projection", projection);
  pbrShaderPtr->setMat4("view", view);

  // pbrShader 쉐이더 프로그램에 카메라 위치값 전송
  pbrShaderPtr->setVec3("camPos", camera.Position);

  // skybox 쉐이더 프로그램 바인딩 및 현재 카메라의 projection 및 view 행렬 전송
  backgroundShaderPtr->use();
  backgroundShaderPtr->setMat4("projection", projection);
  backgroundShaderPtr->setMat4("view", view);
}

void CameraFeature::finalize()
{
  pbrShaderPtr = nullptr;
  backgroundShaderPtr = nullptr;
}

void CameraFeature::onChange(const CameraParameter &param)
{
  if (camera.Yaw != param.yaw)
  {
    setYaw(param.yaw);
  }

  if (camera.Pitch != param.pitch)
  {
    setPitch(param.pitch);
  }

  if (camera.Zoom != param.zoom)
  {
    setZoom(param.zoom);
  }

  if (camera.Position != param.position)
  {
    setPosition(param.position);
  }

  cameraParameter = param;
}

void CameraFeature::setPbrShader(std::shared_ptr<Shader> pbrShader)
{
  pbrShaderPtr = pbrShader;
}

void CameraFeature::setBackgroundShader(std::shared_ptr<Shader> backgroundShader)
{
  backgroundShaderPtr = backgroundShader;
}

void CameraFeature::getCameraParameter(CameraParameter &param) const
{
  param = cameraParameter;
}

void CameraFeature::setYaw(const float value)
{
  camera.Yaw = value;
}

void CameraFeature::setPitch(const float value)
{
  camera.Pitch = value;
}

void CameraFeature::setZoom(const float value)
{
  camera.Zoom = value;
}

void CameraFeature::setPosition(const glm::vec3 &position)
{
  camera.Position = position;
}
