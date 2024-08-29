#include <iostream>
#include <spdlog/spdlog.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <shader/shader.hpp>

/** 콜백함수 선언 */

// GLFW 윈도우 resizing 콜백함수
void framebuffer_size_callback(GLFWwindow *window, int width, int height);

// GLFW 윈도우 키 입력 콜백함수
void processInput(GLFWwindow *window);

int main()
{
  // 앱 시작
  spdlog::info("Starting the application");

  // GLFW 초기화
  if (!glfwInit())
  {
    spdlog::error("Failed to initialize GLFW");
    return -1;
  }

  // GLFW 윈도우 설정 구성
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // GLFW 윈도우 생성
  spdlog::info("Create GLFW window");
  GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_NAME, nullptr, nullptr);

  // 생성된 GLFW 윈도우 컨텍스트를 현재 OpenGL 컨텍스트로 등록
  glfwMakeContextCurrent(window);

  // GLFW 윈도우 resizing 콜백함수 등록
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  // GLAD 를 사용하여 OpenGL 표준 API 호출 시 사용할 현재 그래픽 드라이버에 구현된 함수 포인터 런타임 로드
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    // 함수 포인터 로드 실패
    spdlog::error("Failed to initialized glad");
    return -1;
  }

  if (!window)
  {
    spdlog::error("Failed to create GLFW window");
    glfwTerminate();
    return -1;
  }

  // 삼각형 렌더링 시 바인딩할 쉐이더 객체 생성
  Shader shader("resources/shaders/triangle.vs", "resources/shaders/triangle.fs");

  /** 정점 데이터 정의 */

  // 2개의 삼각형을 Indexed Drawing 으로 렌더링 시 사용할 정점 position 데이터 생성
  float vertices[] = {
      0.5f, 0.5f, 0.0f,   // 우측 상단
      0.5f, -0.5f, 0.0f,  // 우측 하단
      -0.5f, -0.5f, 0.0f, // 좌측 하단
      -0.5f, 0.5f, 0.0f   // 좌측 상단
  };

  // EBO(Element Buffer Object) 생성 시 사용할 정점 인덱스 배열 생성 > EBO 는 한마디로 인덱스 버퍼라고 보면 됨!
  unsigned int indices[] = {
      0, 1, 3, // 첫 번째 삼각형 인덱스
      1, 2, 3  // 두 번째 삼각형 인덱스
  };

  /**
   * VAO(Vertex Array Object), VBO(Vertex Buffer Object), EBO(Element Buffer Object) 객체 생성 및 바인딩
   *
   * TODO : 각 GL Object 들은 별도 클래스로 추상화할 것.
   */

  // VBO, VAO 객체(object) 참조 id 를 저장할 변수
  unsigned int VBO, VAO, EBO;

  // VAO(Vertex Array Object) 객체 생성
  glGenVertexArrays(1, &VAO);

  // VBO(Vertex Buffer Object) 객체 생성
  glGenBuffers(1, &VBO);

  // EBO(Element Buffer Object) 객체 생성
  glGenBuffers(1, &EBO);

  // VAO 객체 먼저 컨텍스트에 바인딩(연결)함. -> 재사용할 VBO, EBO 객체를 먼저 바인딩된 VAO 에 저장(= 바인딩)할 목적
  glBindVertexArray(VAO);

  // VBO 객체 바인딩 및 버퍼에 데이터 쓰기
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // EBO 객체 바인딩 및 버퍼에 데이터 쓰기
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  // VBO 버퍼에 쓰여진 정점 데이터 해석 방식 정의
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  // VBO 객체 설정 완료 후 OpenGL 컨텍스트로부터 바인딩 해제
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // VAO 객체 설정 완료 후 OpenGL 컨텍스트로부터 바인딩 해제
  glBindVertexArray(0);

  // 와이어프레임 모드로 그리기 설정
  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  // 색상 버퍼 초기화할 색상 변경
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

  // 렌더링 루프 실행
  while (!glfwWindowShouldClose(window))
  {
    // GLFW 윈도우 키 입력 감지
    processInput(window);

    // 색상 버퍼 초기화
    glClear(GL_COLOR_BUFFER_BIT);

    /** 삼각형 indexed drawing 으로 그리기 */

    // 삼각형 렌더링에 사용할 쉐이더 객체 바인딩
    shader.use();

    // 삼각형 렌더링 시 필요한 GL 버퍼들이 저장된 VAO 객체 바인딩
    glBindVertexArray(VAO);

    // indexed drawing 으로 2개의 삼각형 그리기 호출
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // Back 버퍼에 렌더링된 최종 이미지를 Front 버퍼에 교체 -> blinking 현상 방지
    glfwSwapBuffers(window);

    // 키보드, 마우스 입력 이벤트 발생 검사 후 등록된 콜백함수 호출 + 이벤트 발생에 따른 GLFWwindow 상태 업데이트
    glfwPollEvents();
  }

  // 렌더링 루프 종료 시, GL 버퍼 객체 메모리 반납
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);

  // GLFW 종료 및 메모리 반납
  glfwTerminate();

  return 0;
}

/** 콜백함수 구현부 */

// GLFW 윈도우 resizing 콜백함수
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
  glViewport(0, 0, width, height);
}

// GLFW 윈도우 키 입력 콜백함수
void processInput(GLFWwindow *window)
{
  // ESC 키 입력 시 렌더링 루프 및 프로그램 종료
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
  {
    glfwSetWindowShouldClose(window, true);
  }
}
