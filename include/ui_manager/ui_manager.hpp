#ifndef UI_MANAGER_HPP
#define UI_MANAGER_HPP

/*
  #ifndef ~ #endif 전처리기는
  헤더파일의 헤더가드를 처리하여 중복 include 방지해 줌!
*/

#include <memory>
#include "app/app.hpp"
#include "ui_containers/material_ui.hpp"
#include "ui_containers/camera_ui.hpp"
#include "ui_containers/light_ui.hpp"
#include "ui_containers/ibl_ui.hpp"
#include <GLFW/glfw3.h> // 다른 모듈에서 glad.h 를 포함하고 있을 지 모르니, glfw3.h 는 가급적 맨 마지막에 include 할 것.

/**
 * UiManager 클래스
 *
 * UI Container 들을 관리하는 클래스.
 * ImGui 입력 변경을 루프 함수에서 감지하여 notify 를 전파함.
 */
class UiManager
{
public:
  UiManager();
  ~UiManager() = default;

  void initializeWindow(GLFWwindow *window);
  void initializeUiComponents(App &app);

  void process();

  void finalize();

private:
  App *appPtr;

  // ui_containers
  MaterialUi materialUi;
  CameraUi cameraUi;
  LightUi lightUi;
  IBLUi iblUi;

  // ImGui 입력 변경 시 호출할 콜백 함수들
  void onChangeMaterialUi();
  void onChangeCameraUi();
  void onChangeLightUi();
  void onChangeIBLUi();
};

#endif // UI_MANAGER_HPP
