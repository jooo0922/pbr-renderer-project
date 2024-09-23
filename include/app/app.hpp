#ifndef APP_HPP
#define APP_HPP

/*
  #ifndef ~ #endif 전처리기는
  헤더파일의 헤더가드를 처리하여 중복 include 방지해 줌!
*/

#include <memory>
#include "shader/shader.hpp"
#include "common/controller.hpp"
#include "features/material_feature.hpp"

/**
 * App 클래스
 *
 * 각 Feature 및 Controller 객체 등 주요 시스템을 관리하고
 * 필요한 의존성을 제공하는 중앙 관리 클래스
 */
class App
{
public:
  App();
  ~App();

  void initialize();
  void process();

  // TODO : main.cpp 에서 pbrShader 포인터를 참조하기 위한 임시 getter -> pbrShader 관련 코드들을 App 안으로 모두 옮긴 후 제거 예정.
  std::shared_ptr<Shader> getPbrShader() const;

  // controller getters
  Controller<MaterialParameter> &getMaterialController();

private:
  void initializeShaders();
  void initializeFeatures();
  void initializeControllers();

  // Shaders
  std::shared_ptr<Shader> pbrShader;

  // Features
  MaterialFeature materialFeature;

  // Controllers
  Controller<MaterialParameter> materialController;
};

#endif // APP_HPP
