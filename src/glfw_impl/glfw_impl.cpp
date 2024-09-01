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
  glfwSetWindowUserPointer(window, this); // GLFWwindow 에 사용자 정의 데이터 this 연결 (하단 필기 참고)

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

  // 함수 포인터에 저장한 람다 함수를 GLFW 콜백 함수로 등록 (하단 필기 참고)
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
 * -> 이것을 가능하게 해주는 게 lamda-introducer 에 this 항목을 추가하여 '[this]' 와 같이
 * 캡쳐 리스트를 정의한 것인데, 이와 관련해서는 내용이 길어져서 하단에 따로 정리함.
 *
 *
 * 2. glfw 콜백 함수의 시그니처와 호환성
 *
 * glfwSetFramebufferSizeCallback() 같은 api 들은 콜백 함수를 등록할 때,
 * GLFWframebuffersizefun 타입의 함수 포인터와 같이 특정 함수 시그니처를 요구하는데,
 * std::function 은 이와 같은 시그니처를 가진 람다 함수를 포장하여 호환 가능한 상태를 만들어 줌!
 */

/**
 * lamda introducer []
 *
 *
 * 람다 함수는 아래와 같이
 * 람다 함수의 시작을 알리는 '[]' 기호를 볼 수 있음.
 *
 * framebufferSizeCallbackFunc = [this](GLFWwindow *window, int width, int height)
 * {
 *  this->framebufferSizeCallback(window, width, height);
 * };
 *
 * 이를 'lamda introducer' 라고 하는데,
 * 이것의 역할은 '캡쳐 리스트'로써, 람다 함수 내에서
 * 외부 스코프에 있는 변수들에 접근할 수 있도록 '캡쳐(포획)'하기를 원하는
 * 변수들을 캡쳐 리스트 [] 에 지정할 수 있음.
 *
 * [this] 로 캡쳐 리스트를 지정했다면,
 * 현재 클래스 GLFWImpl 의 인스턴스 포인터를 캡쳐한다는 뜻임.
 *
 * 이렇게 하면, 람다 함수 내에서 this 에 접근할 수 있게 됨에 따라,
 * 현재 클래스의 멤버 변수 및 멤버 함수에 접근할 수 있게 됨.
 */

/**
 * glfwSetWindowUserPointer(GLFWwindow*, this)
 *
 *
 * 이 함수는 매개변수로 넘겨준 GLFWwindow 객체에
 * 사용자가 원하는 데이터(= 사용자 정의 데이터)를 접근할 수 있도록
 * 연결시켜주는 역할을 함.
 *
 * 이것의 장점은 GLFWwindow 에 사용자가 원하는 데이터를 연결시키면,
 * GLFWwindow 에 등록된 콜백함수 내에서 연결된 사용자 정의 데이터를
 * 직접 접근할 수 있음.
 *
 * 예를 들어, 위 코드에서 연결하려는 사용자 정의 데이터가 this 라면,
 * GLFWwindow 객체에 등록된 콜백함수 내에서 현재 GLFWImpl 클래스의 인스턴스인
 * this 에 접근할 수 있음.
 *
 * 이것을 어떻게 접근하냐면, GLFWwindow 객체에 연결된 콜백함수 내에서
 * glfwGetWindowUserPointer(window) 를 똑같이 호출하면,
 *
 * 이전에 이미 glfwSetWindowUserPointer(window, this) 로
 * 현재 GLFWwindow 인 window 에 연결을 시도했던
 * 사용자 정의 데이터 this 를 반환하게 됨.
 *
 * 그러나, 연결된 사용자 정의 데이터의 반환 타입이 void* 으로 고정되어 있으므로,
 * 일반적으로 static_cast 를 사용하여 반환된 사용자 정의 데이터를
 * 아래와 같이 원래의 타입으로 형변환해서 사용함.
 *
 * static_cast<GLFWImpl*>(glfwSetWindowUserPointer(window))
 */

/**
 * 람다 함수 내에서 사용자 정의 데이터로 연결한 this 사용하기
 *
 *
 * 아래 코드와 같이, GLFW 에 람다 함수를 사용하여 콜백 함수를 등록할 경우,
 * 이전에 glfwSetWindowUserPointer(GLFWwindow*, this) 로 연결한 사용자 정의 데이터 this 를
 * GLFW 에 등록하려는 콜백 함수 내에서 사용할 수 있게 됨.
 *
 * glfwSetFramebufferSizeCallback(window, [](GLFWwindow *window, int width, int height)
 *                                { static_cast<GLFWImpl *>(glfwGetWindowUserPointer(window))->framebufferSizeCallbackFunc(window, width, height); });
 *
 * 즉, 'static_cast<GLFWImpl *>(glfwGetWindowUserPointer(window))' 부분이
 * 위에서 말한 '연결된 변수 this 를 GLFW 콜백 함수 내에서 사용자 정의 데이터 this 를 반환받아 사용하는 것' 에 해당
 *
 * 이때, void* 으로 데이터 반환 타입이 고정되어 있기 때문에,
 * static_cast 를 사용하여 원래의 사용자 정의 데이터 타입인 GLFWImpl* 으로
 * 형변환해서 사용한다고 했었지?
 *
 * 결국, 위 코드는 아래와 같이
 * 람다 함수의 주소값을 할당받은
 * std::function<void(GLFWwindow *, int, int)> 함수 포인터 타입의 멤버 변수인
 * 'framebufferSizeCallbackFunc' 를 호출하는 코드를 또 다른 람다 함수로 감싸서
 * GLFW 에 콜백함수로 등록하는 것이라고 보면 됨.
 *
 * 즉, 아래와 같이 연결된 사용자 데이터 this 의 함수 포인터 멤버변수인 framebufferSizeCallbackFunc 을
 * 호출하는 코드를 또 다른 람다 함수로 감싸서 콜백함수로 등록했다는 뜻!
 *
 * glfwSetFramebufferSizeCallback(window, [](GLFWwindow *window, int width, int height)
 *                                { this->framebufferSizeCallbackFunc(window, width, height); });
 */
