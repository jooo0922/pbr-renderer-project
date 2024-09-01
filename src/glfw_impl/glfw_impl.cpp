#include "glfw_impl/glfw_impl.hpp"
#include "camera/camera.hpp" // Camera 클래스 멤버 함수 호출 시, 클래스 전체 정의가 담긴 Camera.hpp 헤더를 소스 파일에 포함해야 함.

GLFWImpl::GLFWImpl(int width, int height, const char *title, Camera *camera)
    : width(width), height(height), title(title),
      firstMouse(true), lastX(width / 2.0f), lastY(height / 2.0f), // 가장 최근 마우스 입력 좌표값을 스크린 좌표의 중점으로 초기화
      camera(camera), deltaTime(0.0f), lastFrame(0.0f)
{
}

GLFWImpl::~GLFWImpl()
{
  // while 렌더링 루프 탈출 시, GLFWwindow 종료 및 리소스 메모리 해제
  glfwTerminate();
}

int GLFWImpl::init()
{
  if (!glfwInit())
  {
    spdlog::error("Failed to initialize GLFW");
    return -1;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  spdlog::info("Create GLFW Window");
  window = glfwCreateWindow(width, height, title, nullptr, nullptr);
  if (!window)
  {
    spdlog::error("Failed to create GLFW window");
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);
  glfwSetWindowUserPointer(window, this); // 사용자 데이터 설정

  /** 콜백 함수 등록 */

  // std::function 함수 포인터 멤버변수에 람다 함수 주소값을 저장함으로써, 람다 함수를 std::function 으로 포장 (관련 내용 하단 필기 참고)
  framebufferSizeCallbackFunc = [this](GLFWwindow *window, int width, int height)
  {
    // 람다 함수 내에서 실제 콜백 함수 호출
    this->framebufferSizeCallback(window, width, height);
  };
  cursorPosCallbackFunc = [this](GLFWwindow *window, double xposIn, double yposIn)
  {
    this->cursorPosCallback(window, xposIn, yposIn);
  };
  scrollCallbackFunc = [this](GLFWwindow *window, double xoffset, double yoffset)
  {
    this->scrollCallback(window, xoffset, yoffset);
  };

  // 함수 포인터에 저장한 람다 함수를 GLFW 콜백 함수로 등록
  glfwSetFramebufferSizeCallback(window, [](GLFWwindow *window, int width, int height)
                                 { static_cast<GLFWImpl *>(glfwGetWindowUserPointer(window))->framebufferSizeCallbackFunc(window, width, height); });
  glfwSetCursorPosCallback(window, [](GLFWwindow *window, double xposIn, double yposIn)
                           { static_cast<GLFWImpl *>(glfwGetWindowUserPointer(window))->cursorPosCallbackFunc(window, xposIn, yposIn); });
  glfwSetScrollCallback(window, [](GLFWwindow *window, double xoffset, double yoffset)
                        { static_cast<GLFWImpl *>(glfwGetWindowUserPointer(window))->scrollCallbackFunc(window, xoffset, yoffset); });

  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    spdlog::error("Failed to initialize glad");
    return -1;
  }

  return 0;
}

int GLFWImpl::shouldClose()
{
  return glfwWindowShouldClose(window);
}

void GLFWImpl::restoreViewport()
{
  /* 기본 프레임버퍼 렌더링 루프 진입 이전에 해상도 복구 */

  // 해상도 값을 담을 변수 선언 (원래는 포인터 변수로 선언해야 함.)
  int scrWidth, scrHeight;

  // 현재 GLFWwindow 객체의 프레임버퍼 해상도를 두 int 타입 포인터 변수에 저장 -> 함수 외부 변수의 주소값을 넘겨줬으니, 포인터와 마찬가지!
  glfwGetFramebufferSize(window, &scrWidth, &scrHeight);

  // 기본 프레임버퍼 해상도 복구
  glViewport(0, 0, scrWidth, scrHeight);
}

void GLFWImpl::process()
{
  /* 카메라 이동속도 보정을 위한 deltaTime 계산 */

  // 현재 프레임 경과시간
  float currentFrame = static_cast<float>(getTime());

  // 현재 프레임 경과시간 - 마지막 프레임 경과시간 = 두 프레임 사이의 시간 간격
  deltaTime = currentFrame - lastFrame;

  // 마지막 프레임 경과시간을 현재 프레임 경과시간으로 업데이트!
  lastFrame = currentFrame;

  // 윈도우 창 및 키 입력 감지 밎 이벤트 처리
  processInput(window);
}

void GLFWImpl::swapBuffers()
{
  // Double Buffer 상에서 Back Buffer 에 픽셀들이 모두 그려지면, Front Buffer 와 교체(swap)해버림.
  glfwSwapBuffers(window);
}

void GLFWImpl::pollEvents()
{
  // 키보드, 마우스 입력 이벤트 발생 검사 후 등록된 콜백함수 호출 + 이벤트 발생에 따른 GLFWwindow 상태 업데이트
  glfwPollEvents();
}

double GLFWImpl::getTime()
{
  return glfwGetTime();
}

// GLFWwindow 윈도우 창 리사이징 감지 시, 호출할 콜백 함수 정의
void GLFWImpl::framebufferSizeCallback(GLFWwindow *window, int width, int height)
{
  glViewport(0, 0, width, height);
}

// GLFW 윈도우에 마우스 입력 감지 시, 호출할 콜백함수 정의
void GLFWImpl::cursorPosCallback(GLFWwindow *window, double xposIn, double yposIn)
{
  // 콜백함수의 매개변수로 전달받는 마우스 좌표값의 타입을 double > float 으로 형변환
  float xpos = static_cast<float>(xposIn);
  float ypos = static_cast<float>(yposIn);

  if (firstMouse)
  {
    // 맨 처음 전달받은 마우스 좌표값은 초기에 설정된 lastX, Y 와 offset 차이가 심할 것임.
    // 이 offset 으로 yaw, pitch 변화를 계산하면 회전이 급격하게 튀다보니,
    // 맨 처음 전달받은 마우스 좌표값으로는 offset 을 계산하지 않고, lastX, Y 값을 업데이트 하는 데에만 사용함.
    lastX = xpos;
    lastY = ypos;
    firstMouse = false;
  }

  // 마지막 프레임의 마우스 좌표값에서 현재 프레임의 마우스 좌표값까지 이동한 offset 계산
  float xoffset = xpos - lastX;
  float yoffset = lastY - ypos; // y축 좌표는 스크린좌표계와 3D 좌표계(오른손 좌표계)와 방향이 반대이므로, -(ypos - lastY) 와 같이 뒤집어준 것!

  // 마지막 프레임의 마우스 좌표값 갱신
  lastX = xpos;
  lastY = ypos;

  // 마우스 이동량(offset)에 따른 카메라 오일러 각 재계산 및 카메라 로컬 축 벡터 업데이트
  camera->ProcessMouseMovement(xoffset, yoffset);
}

// GLFW 윈도우에 스크롤 입력 감지 시, 호출할 콜백함수 정의
void GLFWImpl::scrollCallback(GLFWwindow *window, double xoffset, double yoffset)
{
  camera->ProcessMouseScroll(static_cast<float>(yoffset));
}

// GLFW 윈도우 및 키 입력 감지 및 이에 대한 반응 처리 함수 구현
void GLFWImpl::processInput(GLFWwindow *window)
{
  // 현재 GLFWwindow 에 대하여(활성화 시,) 특정 키(esc 키)가 입력되었는지 여부를 감지
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
  {
    glfwSetWindowShouldClose(window, true); // GLFWwindow 의 WindowShouldClose 플래그(상태값)을 true 로 설정 -> main() 함수의 while 조건문에서 렌더링 루프 탈출 > 렌더링 종료!
  }

  // 카메라 이동속도 보정 (기본 속도 2.5 가 어느 컴퓨터에서든 유지될 수 있도록 deltaTime 값으로 속도 보정)
  float cameraSpeed = static_cast<float>(2.5 * deltaTime);

  // 키 입력에 따른 카메라 이동 처리 (GLFW 키 입력 메서드에 독립적인 enum 사용)
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
  {
    camera->ProcessKeyboard(FORWARD, deltaTime);
  }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
  {
    camera->ProcessKeyboard(BACKWARD, deltaTime);
  }
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
  {
    camera->ProcessKeyboard(LEFT, deltaTime);
  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
  {
    camera->ProcessKeyboard(RIGHT, deltaTime);
  }
}

/**
 * 람다 함수를 std:function 함수 포인터로 포장하는 이유 두 가지
 *
 *
 * 1. std::function 은 람다 함수의 캡쳐 상태를 유지시켜 줌.
 *
 * 캡쳐 상태란, 람다 함수가 함수 block 외부 변수의 값이나 참조가 가능한 상태를 의미하며,
 * 여기서는 특정 람다 함수가 GLFWImpl 클래스의 멤버변수 및 인스턴스(this)에 대한 참조와 변경이 가능한 상태를
 * 유지시켜 주는 것을 의미함!
 *
 *
 * 2. glfw 콜백 함수의 시그니처와 호환성
 *
 * glfwSetFramebufferSizeCallback() 같은 api 들은 콜백 함수를 등록할 때,
 * GLFWframebuffersizefun 타입의 함수 포인터와 같이 특정 함수 시그니처를 요구하는데,
 * std::function 은 이와 같은 시그니처를 가진 람다 함수를 포장하여 호환 가능한 상태를 만들어 줌!
 */
