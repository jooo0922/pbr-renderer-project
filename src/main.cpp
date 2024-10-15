// 행렬 및 벡터 계산에서 사용할 Header Only 라이브러리 include
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader/shader.hpp"
#include "glfw_impl/glfw_impl.hpp"
#include "renderable_objects/sphere.hpp"
#include "gl_context/gl_context.hpp"

#include "app/app.hpp"
#include "ui_manager/ui_manager.hpp"

#include <memory>
#include <iostream>
#include <spdlog/spdlog.h>

int main()
{
  // 앱 시작
  spdlog::info("Starting the application");

  // GLFWImpl 객체 생성
  GLFWImpl glfwImpl(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_NAME);

  // GLFWImpl 초기화
  if (glfwImpl.init() != 0)
  {
    spdlog::error("Failed to initialize GLFWImpl");
    return -1;
  }

  /* OpenGL 전역 상태값 설정 */

  // GLContext 싱글턴 인스턴스 접근
  GLContext &glContext = GLContext::getInstance();

  /* App 클래스 초기화 */
  App app;
  app.initialize();

  /* UiManager 클래스 초기화 */
  UiManager uiManager;
  uiManager.initializeWindow(glfwImpl.getWindow());
  uiManager.initializeUiComponents(app);

  /* PBR 구현에 필요한 쉐이더 객체 생성 및 컴파일 */

  // TODO : App 클래스 내부에서 동적 할당된 각 Shader 객체의 스마트 포인터 참조 -> 각 Shader 관련 코드 리팩토링 완료 시 제거 예정
  std::shared_ptr<Shader> pbrShader = app.getPbrShader();
  std::shared_ptr<Shader> backgroundShader = app.getBackgroundShader();

  /** renderable objects 초기화 */

  // Sphere 객체 생성
  Sphere sphere;

  glfwImpl.restoreViewport();

  // while 문으로 렌더링 루프 구현
  while (!glfwImpl.shouldClose())
  {
    // 렌더링 루프에서의 glfw 처리
    glfwImpl.process();

    // 색상 버퍼 및 깊이 버퍼 초기화
    glContext.clear();

    app.process();

    /* 각 Sphere 에 적용할 모델행렬 계산 및 Sphere 렌더링 */

    pbrShader->use();

    // 모델행렬을 단위행렬로 초기화
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.0f));

    // 계산된 모델행렬을 쉐이더 프로그램에 전송
    pbrShader->setMat4("model", model);

    /*
      쉐이더 코드에서 노멀벡터를 World Space 로 변환할 때
      사용할 노멀행렬을 각 구체의 계산된 모델행렬로부터 계산 후,
      쉐이더 코드에 전송
    */
    pbrShader->setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));

    // 구체 렌더링
    sphere.draw(*pbrShader);

    uiManager.process();

    glfwImpl.swapBuffers();
    glfwImpl.pollEvents();
  }

  uiManager.finalize();

  return 0;
}
