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

  // TODO : IBLFeature 구현 후, offscreen buffer 바인딩 코드 이동 (Combo ui 입력값 변경에 따른 index 매개변수 전달 추가)
  offscreenRenderingFeature.useIrradianceMap(0);
  offscreenRenderingFeature.usePrefilterMap(0);
  offscreenRenderingFeature.useBRDFLUTTexture();

  /** skybox 렌더링 */
  // TODO : IBLFeature 구현 후 skybox 렌더링 코드 이동

  // skybox 쉐이더 프로그램 바인딩
  backgroundShader->use();

  // HDR 큐브맵 텍스쳐를 바인딩하여 skybox 텍스쳐로 사용
  offscreenRenderingFeature.useEnvCubemap(0);

  // skybox 렌더링
  offscreenRenderingFeature.getCube().draw(*backgroundShader);
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

Controller<LightParameter> &App::getLightController()
{
  return lightController;
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
}
