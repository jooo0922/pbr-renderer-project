#ifndef VERTEX_BUFFER_OBJECT_HPP
#define VERTEX_BUFFER_OBJECT_HPP

/*
  #ifndef ~ #endif 전처리기는
  헤더파일의 헤더가드를 처리하여 중복 include 방지해 줌!
*/

#include <glad/glad.h> // OpenGL 함수를 초기화하기 위한 헤더
#include <gl_objects/gl_object.hpp>

/**
 * VertexBufferObject 클래스
 *
 * VBO 객체를 추상화한 클래스
 */
class VertexBufferObject final : public IGLObject
{
public:
  VertexBufferObject();

  ~VertexBufferObject() override;

  void setData(const void *data, GLsizeiptr size, GLenum usage);

  GLuint getID() const;

  void bind() const override;

  void unbind() const override;

  void destroy() override;

private:
  GLuint ID;
};

#endif // VERTEX_BUFFER_OBJECT_HPP
