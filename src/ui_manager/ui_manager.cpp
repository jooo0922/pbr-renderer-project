#include "ui_manager/ui_manager.hpp"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

UiManager::UiManager()
    : appPtr(nullptr)
{
}

void UiManager::initializeWindow(GLFWwindow *window)
{
  const char *glsl_version = "#version 330 core";

  auto imguiContext = ImGui::CreateContext();
  ImGui::SetCurrentContext(imguiContext);
  ImGui::StyleColorsDark();

  ImGui_ImplGlfw_InitForOpenGL(window, false);
  ImGui_ImplOpenGL3_Init(glsl_version);
  ImGui_ImplOpenGL3_CreateFontsTexture();
  ImGui_ImplOpenGL3_CreateDeviceObjects();
}

void UiManager::initializeUiComponents(App &app)
{
  appPtr = &app;

  // 각 Controller 객체에 UiContainer 객체를 리스너로 등록
  appPtr->getMaterialController().addListener(materialUi);

  /**
   * 각 Controller 객체에 초기화된 파라미터 값들을
   * (-> App::initializeControllers() 함수에서 초기화됨.)
   * 리스너로 등록된 ui_containers 내부의 ui_component 요소들의 초기값으로 지정
   */
  const MaterialParameter materialParameter = appPtr->getMaterialController().getValue();
  appPtr->getMaterialController().setValue(materialParameter);
}

void UiManager::process()
{
  ImGui_ImplGlfw_NewFrame();

  // ImGui 프레임 새로 생성
  ImGui::NewFrame();

  {
    // MaterialUi 패널 생성
    ImGui::Begin("Material");
    if (materialUi.onUiComponents())
    {
      onChangeMaterialUi();
    }
    ImGui::End();
  }

  // ImGui 가 렌더링할 drawData 를 모아 둠.
  ImGui::Render();

  // drawData 를 가져와서 OpenGL 로 ImGui 실제 렌더링
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void UiManager::finalize()
{
  ImGui_ImplOpenGL3_DestroyFontsTexture();
  ImGui_ImplOpenGL3_DestroyDeviceObjects();
  ImGui_ImplGlfw_Shutdown();

  ImGui::DestroyContext();
}

void UiManager::onChangeMaterialUi()
{
  // MaterialUi 컨테이너로부터 현재 ImGui 입력값을 가져와서 MaterialParameter 에 복사 후 Controller 객체에 notify 전파
  MaterialParameter materialParameter;
  materialUi.getMaterialParam(materialParameter);
  appPtr->getMaterialController().setValue(materialParameter, &materialUi);
}
