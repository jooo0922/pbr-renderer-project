#ifndef GL_CONTEXT_HPP
#define GL_CONTEXT_HPP

/*
  #ifndef ~ #endif 전처리기는
  헤더파일의 헤더가드를 처리하여 중복 include 방지해 줌!
*/

#include <glad/glad.h> // OpenGL 함수를 초기화하기 위한 헤더
#include <glm/glm.hpp>

/**
 * OpenGL 전역 상태를 관리하는 싱글톤 클래스
 *
 * TODO : 현재 프로젝트는 단일 OpenGL 컨텍스트를 사용하고 있어 문제가 없지만,
 * 추후 프로젝트가 다중 컨텍스트를 지원해야 할 경우, 싱글톤 구조를 사용하지 않고,
 * 각 컨텍스트별 상태 관리 객체를 생성하여 다른 클래스들에 dependency injection 하여
 * 사용하는 식으로 구조를 변경할 것!
 */
class GLContext
{
public:
  // 싱글톤 인스턴스 접근
  static GLContext &getInstance();

  // viewport 크기 변경
  void resize(int width, int height);

  // 색상 및 깊이 버퍼 초기화
  void clear();

  // OpenGL 상태 활성화
  void enable(GLenum capability);

  // OpenGL 상태 비활성화
  void disable(GLenum capability);

  // 색상 버퍼 변경
  void setClearColor(const glm::vec4 &color);

  // 깊이 테스트 함수 변경
  void setDepthFunc(GLenum func);

private:
  // 사용자가 직접 싱글턴 인스턴스화하지 못하도록 생성자 및 소멸자를 private 접근자로 캡슐화
  GLContext();
  ~GLContext() = default;

  /**
   * 기본 복사 생성자 및 대입 연산자를 클래스 인스턴스가 적재되는 메모리 공간에서 제거
   *
   * delete 키워드 관련 하단 링크 참고
   * https://github.com/jooo0922/cpp-study/blob/main/Chapter9/Chapter9_10/Chapter9_10.cpp
   */
  GLContext(const GLContext &) = delete;
  GLContext &operator=(const GLContext &) = delete;

  // OpenGL 전역 상태 기본값 설정 함수
  void initialize();

  // OpenGL 전역 상태 변수 기본값 설정
  int viewportX = 0;
  int viewportY = 0;
  int viewportWidth = 0;
  int viewportHeight = 0;
  glm::vec4 clearColor = glm::vec4(0.2f, 0.3f, 0.3f, 1.0f);
  GLenum depthFunc = GL_LEQUAL;

  // 싱글톤 인스턴스 포인터
  static GLContext *instance;
};

#endif // GL_CONTEXT_HPP