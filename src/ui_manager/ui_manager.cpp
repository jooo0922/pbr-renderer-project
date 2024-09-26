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

void UiManager::initializeControllers(App &app)
{
  appPtr = &app;

  const MaterialParameter materialParameter = appPtr->getMaterialController().getValue();
  appPtr->getMaterialController().setValue(materialParameter);
}

void UiManager::process()
{
  ImGui_ImplGlfw_NewFrame();

  // ImGui 프레임 새로 생성
  ImGui::NewFrame();

  // 테스트용 ImGui 렌더링
  {
    ImGui::Begin("Hello, ImGui!");         // 창 이름
    ImGui::Text("This is a simple text."); // 텍스트 표시
    ImGui::End();
  }

  // TODO : MaterialUi::OnUiComponents() 를 호출하여 입력값 변경 감지 시, onChangeMaterialUi() 호출하는 코드 추가

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
  MaterialParameter materialParameter;

  // TODO : MaterialUi 컨테이너로부터 현재 ImGui 입력값을 가져와서 MaterialParameter 에 복사하는 코드 추가

  appPtr->getMaterialController().setValue(materialParameter);
}
