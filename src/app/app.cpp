#include "app/app.hpp"

App::App() : pbrShader(nullptr)
{
}

App::~App()
{
  /* 각 Feature 클래스들 종료 */
  materialFeature.finalize();
  cameraFeature.finalize();
  lightFeature.finalize();
  offscreenRenderingFeature.finalize();
  iblFeature.finalize();
  modelFeature.finalize();
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
  lightFeature.process();
  offscreenRenderingFeature.process();
  iblFeature.process();
  modelFeature.process();
}

Controller<MaterialParameter> &App::getMaterialController()
{
  return materialController;
}

Controller<CameraParameter> &App::getCameraController()
{
  return cameraController;
}

Controller<LightParameter> &App::getLightController()
{
  return lightController;
}

Controller<IBLParameter> &App::getIBLController()
{
  return iblController;
}

Controller<ModelParameter> &App::getModelController()
{
  return modelController;
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

  // lightFeature 초기화
  lightFeature.setPbrShader(pbrShader);
  lightFeature.initialize();

  // offscreenRenderingFeature 초기화
  offscreenRenderingFeature.setPbrShader(pbrShader);
  offscreenRenderingFeature.setBackgroundShader(backgroundShader);
  offscreenRenderingFeature.initialize();

  // iblFeature 초기화
  iblFeature.setPbrShader(pbrShader);
  iblFeature.setBackgroundShader(backgroundShader);
  iblFeature.setOffscreenRenderingFeature(&offscreenRenderingFeature);
  iblFeature.initialize();

  // modelFeature 초기화
  modelFeature.setPbrShader(pbrShader);
  modelFeature.initialize();
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

  // lightController 객채의 파라미터 값 초기화 및 리스너 등록
  LightParameter lightParameter;
  lightFeature.getLightParameter(lightParameter);
  lightController.addListener(lightFeature);
  lightController.setValue(lightParameter);

  // iblController 객체의 파라미터 값 초기화 및 리스너 등록
  IBLParameter iblParameter;
  iblFeature.getIBLParameter(iblParameter);
  iblController.addListener(iblFeature);
  iblController.setValue(iblParameter);

  // modelController 객체의 파라미터 값 초기화 및 리스너 등록
  ModelParameter modelParameter;
  modelFeature.getModelParameter(modelParameter);
  modelController.addListener(modelFeature);
  modelController.setValue(modelParameter);
}
