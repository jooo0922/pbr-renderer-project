#include "gl_context/gl_context.hpp"

// 싱글턴 인스턴스 초기화
GLContext *GLContext::instance = nullptr;

GLContext &GLContext::getInstance()
{
  if (!instance)
  {
    instance = new GLContext();
  }
  return *instance;
}

GLContext::GLContext()
{
  initialize();
}

void GLContext::initialize()
{
  // Depth Test(깊이 테스팅) 상태를 활성화함
  enable(GL_DEPTH_TEST);

  // 깊이 테스트 함수 변경 (skybox 렌더링 목적)
  setDepthFunc(depthFunc);

  // Cubemap 의 각 face 사이의 seam line 방지 활성화 (하단 필기 참고)
  enable(GL_TEXTURE_CUBE_MAP);

  // 현재까지 저장되어 있는 프레임 버퍼(그 중에서도 색상 버퍼) 초기화하기
  setClearColor(clearColor);
}

void GLContext::resize(int width, int height)
{
  viewportWidth = width;
  viewportHeight = height;
  glViewport(viewportX, viewportY, viewportWidth, viewportHeight);
}

void GLContext::clear()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GLContext::enable(GLenum capability)
{
  glEnable(capability);
}

void GLContext::disable(GLenum capability)
{
  glDisable(capability);
}

void GLContext::setClearColor(const glm::vec4 &color)
{
  clearColor = color;
  glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
}

void GLContext::setDepthFunc(GLenum func)
{
  depthFunc = func;
  glDepthFunc(depthFunc);
}
