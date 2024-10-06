#include "features/camera_feature.hpp"
#include "constants/camera_constansts.hpp"

CameraFeature::CameraFeature()
    : camera(Camera()),
      pbrShaderPtr(nullptr),
      backgroundShaderPtr(nullptr)
{
}

void CameraFeature::initialize()
{
  // CameraUi 에서 관리되는 각 ImGui 요소에 입력할 초기값 설정
  cameraParameter.yaw = CameraConstants::DEFAULT_YAW;
  cameraParameter.pitch = CameraConstants::DEFAULT_PITCH;
  cameraParameter.zoom = CameraConstants::DEFAULT_ZOOM;
  cameraParameter.position = CameraConstants::DEFAULT_POSITION;
}

void CameraFeature::process()
{
  // 카메라의 zoom 값으로부터 투영 행렬 계산
  glm::mat4 projection = glm::perspective(glm::radians(camera.getCameraZoom()), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);

  // 카메라 클래스로부터 뷰 행렬(= LookAt 행렬) 가져오기
  glm::mat4 view = camera.getViewMatrix();

  // pbrShader 쉐이더 프로그램 바인딩 및 현재 카메라의 projection 및 view 행렬 전송
  pbrShaderPtr->use();
  pbrShaderPtr->setMat4("projection", projection);
  pbrShaderPtr->setMat4("view", view);

  // pbrShader 쉐이더 프로그램에 카메라 위치값 전송
  pbrShaderPtr->setVec3("camPos", camera.getCameraPosition());

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
  if (camera.getCameraYaw() != param.yaw)
  {
    setYaw(param.yaw);
  }

  if (camera.getCameraPitch() != param.pitch)
  {
    setPitch(param.pitch);
  }

  if (camera.getCameraZoom() != param.zoom)
  {
    setZoom(param.zoom);
  }

  if (camera.getCameraPosition() != param.position)
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

void CameraFeature::setYaw(const float yaw)
{
  camera.setCameraYaw(yaw);
}

void CameraFeature::setPitch(const float pitch)
{
  camera.setCameraPitch(pitch);
}

void CameraFeature::setZoom(const float zoom)
{
  camera.setCameraZoom(zoom);
}

void CameraFeature::setPosition(const glm::vec3 &position)
{
  camera.setCameraPosition(position);
}
