#include "app/app.hpp"

App::App() : pbrShader(nullptr)
{
}

App::~App()
{
  /* 각 Feature 클래스들 종료 */
  materialFeature.finalize();
  cameraFeature.finalize();
}

void App::initialize()
{
  initializeShaders();
  initializeFeatures();
  initializeControllers();
}

void App::process()
{
  /* 각 Feature 클래스들의 렌더링 루프 작업 수행 */
  materialFeature.process();
  cameraFeature.process();
}

std::shared_ptr<Shader> App::getPbrShader() const
{
  return pbrShader;
}

std::shared_ptr<Shader> App::getBackgroundShader() const
{
  return backgroundShader;
}

Controller<MaterialParameter> &App::getMaterialController()
{
  return materialController;
}

Controller<CameraParameter> &App::getCameraController()
{
  return cameraController;
}

void App::initializeShaders()
{
  /* PBR 구현에 필요한 쉐이더 객체 생성 및 컴파일 */

  // 구체 렌더링 시 적용할 PBR 쉐이더 객체 생성
  pbrShader = std::make_shared<Shader>("resources/shaders/pbr.vs", "resources/shaders/pbr.fs");

  // 배경에 적용할 skybox 를 렌더링하는 쉐이더 객체 생성
  backgroundShader = std::make_shared<Shader>("resources/shaders/background.vs", "resources/shaders/background.fs");
}

void App::initializeFeatures()
{
  /* Feature 객체 초기화 */

  // materialFeature 초기화
  materialFeature.setPbrShader(pbrShader);
  materialFeature.initialize();

  // cameraFeature 초기화
  cameraFeature.setPbrShader(pbrShader);
  cameraFeature.setBackgroundShader(backgroundShader);
  cameraFeature.initialize();
}

void App::initializeControllers()
{
  /* Controller 객체들의 파라미터 값 초기화 및 리스너 등록 */

  // materialController 객채의 파라미터 값 초기화 및 리스너 등록
  MaterialParameter materialParameter;
  materialFeature.getMaterialParameter(materialParameter);
  materialController.addListener(materialFeature);
  materialController.setValue(materialParameter);

  // cameraController 객채의 파라미터 값 초기화 및 리스너 등록
  CameraParameter cameraParameter;
  cameraFeature.getCameraParameter(cameraParameter);
  cameraController.addListener(cameraFeature);
  cameraController.setValue(cameraParameter);
}
