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

  // ImGui 플래그 설정
  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

  /**
   * ImGui_ImplGlfw_InitForOpenGL(window, true);
   *
   * 위 함수에서 두 번째 매개변수를 true 로 지정함으로써,
   * ImGui 내부에서의 GLFW 입력 콜백을 자동으로 처리하도록 함.
   *
   * 이것을 활성화해야 ImGui 내부에 정의된 입력 이벤트 콜백이 정상 작동함.
   *
   * -> 단, ImGui 입력 이벤트 콜백을 자동 처리할 경우,
   * 사용자가 glfwGetKey() 함수로 직접 정의한 입력 처리는 비활성화하는 것이 좋음.
   */
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);
  ImGui_ImplOpenGL3_CreateFontsTexture();
  ImGui_ImplOpenGL3_CreateDeviceObjects();
}

void UiManager::initializeUiComponents(App &app)
{
  appPtr = &app;

  // 각 Controller 객체에 UiContainer 객체를 리스너로 등록
  appPtr->getMaterialController().addListener(materialUi);
  appPtr->getCameraController().addListener(cameraUi);
  appPtr->getLightController().addListener(lightUi);
  appPtr->getIBLController().addListener(iblUi);
  appPtr->getModelController().addListener(modelUi);

  /**
   * 각 Controller 객체에 초기화된 파라미터 값들을
   * (-> App::initializeControllers() 함수에서 초기화됨.)
   * 리스너로 등록된 ui_containers 내부의 ui_component 요소들의 초기값으로 지정
   */
  const MaterialParameter materialParameter = appPtr->getMaterialController().getValue();
  appPtr->getMaterialController().setValue(materialParameter);

  const CameraParameter cameraParameter = appPtr->getCameraController().getValue();
  appPtr->getCameraController().setValue(cameraParameter);

  const LightParameter lightParameter = appPtr->getLightController().getValue();
  appPtr->getLightController().setValue(lightParameter);

  const IBLParameter iblParameter = appPtr->getIBLController().getValue();
  appPtr->getIBLController().setValue(iblParameter);

  const ModelParameter modelParameter = appPtr->getModelController().getValue();
  appPtr->getModelController().setValue(modelParameter);
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

    // CameraUi 패널 생성
    ImGui::Begin("Camera");
    if (cameraUi.onUiComponents())
    {
      onChangeCameraUi();
    }
    ImGui::End();

    // LightUi 패널 생성
    ImGui::Begin("Light");
    if (lightUi.onUiComponents())
    {
      onChangeLightUi();
    }
    ImGui::End();

    // IBLUi 패널 생성
    ImGui::Begin("IBL");
    if (iblUi.onUiComponents())
    {
      onChangeIBLUi();
    }
    ImGui::End();

    // ModelUi 패널 생성
    ImGui::Begin("Model");
    if (modelUi.onUiComponents())
    {
      onChangeModelUi();
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

void UiManager::onChangeCameraUi()
{
  // CameraUi 컨테이너로부터 현재 ImGui 입력값을 가져와서 CameraParameter 에 복사 후 Controller 객체에 notify 전파
  CameraParameter cameraParameter;
  cameraUi.getCameraParam(cameraParameter);
  appPtr->getCameraController().setValue(cameraParameter, &cameraUi);
}

void UiManager::onChangeLightUi()
{
  // LightUi 컨테이너로부터 현재 ImGui 입력값을 가져와서 LightParameter 에 복사 후 Controller 객체에 notify 전파
  LightParameter lightParameter;
  lightUi.getLightParam(lightParameter);
  appPtr->getLightController().setValue(lightParameter, &lightUi);
}

void UiManager::onChangeIBLUi()
{
  // IBLUi 컨테이너로부터 현재 ImGui 입력값을 가져와서 IBLParameter 에 복사 후 Controller 객체에 notify 전파
  IBLParameter iblParameter;
  iblUi.getIBLParam(iblParameter);
  appPtr->getIBLController().setValue(iblParameter, &iblUi);
}

void UiManager::onChangeModelUi()
{
  // ModelUi 컨테이너로부터 현재 ImGui 입력값을 가져와서 ModelParameter 에 복사 후 Controller 객체에 notify 전파
  ModelParameter modelParameter;
  modelUi.getModelParam(modelParameter);
  appPtr->getModelController().setValue(modelParameter, &modelUi);
}
