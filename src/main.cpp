#include <memory>
#include <iostream>

#include <spdlog/spdlog.h>

#include "glfw_impl/glfw_impl.hpp"
#include "gl_context/gl_context.hpp"
#include "app/app.hpp"
#include "ui_manager/ui_manager.hpp"

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

  glfwImpl.restoreViewport();

  // while 문으로 렌더링 루프 구현
  while (!glfwImpl.shouldClose())
  {
    // 렌더링 루프에서의 glfw 처리
    glfwImpl.process();

    // 색상 버퍼 및 깊이 버퍼 초기화
    glContext.clear();

    app.process();

    uiManager.process();

    glfwImpl.swapBuffers();
    glfwImpl.pollEvents();
  }

  uiManager.finalize();

  return 0;
}
