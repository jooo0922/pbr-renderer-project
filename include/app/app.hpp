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
#include "features/camera_feature.hpp"
#include "features/light_feature.hpp"
#include "features/offscreen_rendering_feature.hpp"
#include "features/ibl_feature.hpp"

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

  // TODO : main.cpp 에서 각 Shader 포인터를 참조하기 위한 임시 getter -> 각 Shader 관련 코드들을 App 안으로 모두 옮긴 후 제거 예정.
  std::shared_ptr<Shader> getPbrShader() const;
  std::shared_ptr<Shader> getBackgroundShader() const;

  // controller getters
  Controller<MaterialParameter> &getMaterialController();
  Controller<CameraParameter> &getCameraController();
  Controller<LightParameter> &getLightController();
  Controller<IBLParameter> &getIBLController();

private:
  void initializeShaders();
  void initializeFeatures();
  void initializeControllers();

  // Shaders
  std::shared_ptr<Shader> pbrShader;
  std::shared_ptr<Shader> backgroundShader;

  // Features
  MaterialFeature materialFeature;
  CameraFeature cameraFeature;
  LightFeature lightFeature;
  OffscreenRenderingFeature offscreenRenderingFeature;
  IBLFeature iblFeature;

  // Controllers
  Controller<MaterialParameter> materialController;
  Controller<CameraParameter> cameraController;
  Controller<LightParameter> lightController;
  Controller<IBLParameter> iblController;
};

#endif // APP_HPP
