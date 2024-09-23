#include "app/app.hpp"

App::App() : pbrShader(nullptr)
{
}

App::~App()
{
  /* 각 Feature 클래스들 종료 */
  materialFeature.finalize();
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
}

std::shared_ptr<Shader> App::getPbrShader() const
{
  return pbrShader;
}

Controller<MaterialParameter> &App::getMaterialController()
{
  return materialController;
}

void App::initializeShaders()
{
  /* PBR 구현에 필요한 쉐이더 객체 생성 및 컴파일 */

  // 구체 렌더링 시 적용할 PBR 쉐이더 객체 생성
  pbrShader = std::make_shared<Shader>("resources/shaders/pbr.vs", "resources/shaders/pbr.fs");
}

void App::initializeFeatures()
{
  /* Feature 객체 초기화 */

  // materialFeature 초기화
  materialFeature.setPbrShader(pbrShader);
  materialFeature.initialize();
}

void App::initializeControllers()
{
  /* Controller 객체 초기화 */

  // materialController 초기화
  materialController.addListener(materialFeature);
}
