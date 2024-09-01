#ifndef GLFW_IMPL_HPP
#define GLFW_IMPL_HPP

/*
  #ifndef ~ #endif 전처리기는
  헤더파일의 헤더가드를 처리하여 중복 include 방지해 줌!
*/

#include <glad/glad.h>  // 운영체제(플랫폼)별 OpenGL 함수를 함수 포인터에 저장 및 초기화 (OpenGL 을 사용하는 다른 라이브러리(GLFW)보다 먼저 include 할 것.)
#include <GLFW/glfw3.h> // OpenGL 컨텍스트 생성, 윈도우 생성, 사용자 입력 처리 관련 OpenGL 라이브러리
#include <spdlog/spdlog.h>

// 클래스 전방 선언
class Camera;

/**
 * GLFWImpl 클래스
 *
 * GLFW 관련 초기화 작업 및 콜백 함수들을 관리하는 클래스
 */
class GLFWImpl
{
public:
  // 생성자
  GLFWImpl(int width, int height, const char *title, Camera *camera);

  // 소멸자
  ~GLFWImpl();

  // GLFW 및 GLAD 초기화
  int init();

  // GLFWWindow 창 닫기 입력 감지
  int shouldClose();

  // 뷰포트 해상도 복구
  void restoreViewport();

  // 렌더링 루프에서 처리해야 할 GLFW 관련 작업
  void process();

  // 버퍼 교체
  void swapBuffers();

  // 이벤트 폴링
  void pollEvents();

  // GLFW time 반환
  double getTime();

private:
  // GLFWWindow 객체 관련 멤버 변수
  GLFWwindow *window;
  int width;
  int height;
  const char *title;

  // 마우스 입력 관련 멤버 변수
  float lastX;
  float lastY;
  bool firstMouse;

  // 카메라 및 시간 관련 멤버 변수
  Camera *camera;
  float deltaTime; // 마지막에 그려진 프레임 ~ 현재 프레임 사이의 시간 간격
  float lastFrame; // 마지막에 그려진 프레임의 ElapsedTime(경과시간)

  // 람다 함수 주소값을 저장할 포인터 멤버 변수
  std::function<void(GLFWwindow *, int, int)> framebufferSizeCallbackFunc;
  std::function<void(GLFWwindow *, double, double)> cursorPosCallbackFunc;
  std::function<void(GLFWwindow *, double, double)> scrollCallbackFunc;

  // 람다 함수에서 호출할 실제 콜백 함수를 정의하는 멤버 함수
  void framebufferSizeCallback(GLFWwindow *window, int width, int height);
  void cursorPosCallback(GLFWwindow *window, double xposIn, double yposIn);
  void scrollCallback(GLFWwindow *window, double xoffset, double yoffset);

  // GLFW 윈도우 및 키 입력 감지 및 이에 대한 반응 처리 함수 선언
  void processInput(GLFWwindow *window);
};

#endif // GLFW_IMPL_HPP