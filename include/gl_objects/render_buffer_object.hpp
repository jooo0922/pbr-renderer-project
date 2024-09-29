#ifndef RENDER_BUFFER_OBJECT_HPP
#define RENDER_BUFFER_OBJECT_HPP

/*
  #ifndef ~ #endif 전처리기는
  헤더파일의 헤더가드를 처리하여 중복 include 방지해 줌!
*/

#include <glad/glad.h> // OpenGL 함수를 초기화하기 위한 헤더
#include <gl_objects/gl_object.hpp>

/**
 * RenderBufferObject 클래스
 *
 * offscreen rendering 에 사용할 Renderbuffer 객체를 추상화한 클래스
 */
class RenderBufferObject : public IGLObject
{
public:
  RenderBufferObject();

  ~RenderBufferObject();

  void setStorage(GLsizei width, GLsizei height) const;

  GLuint getID() const;

  void bind() const override;

  void unbind() const override;

  void destroy() override;

private:
  GLuint ID;
};

#endif // RENDER_BUFFER_OBJECT_HPP
