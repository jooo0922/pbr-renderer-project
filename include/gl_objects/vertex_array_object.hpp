#ifndef VERTEX_ARRAY_OBJECT_HPP
#define VERTEX_ARRAY_OBJECT_HPP

/*
  #ifndef ~ #endif 전처리기는
  헤더파일의 헤더가드를 처리하여 중복 include 방지해 줌!
*/

#include <glad/glad.h> // OpenGL 함수를 초기화하기 위한 헤더
#include <gl_object.hpp>
#include <vertex_buffer_object.hpp>
#include <index_buffer_object.hpp>
#include <vector>
#include <tuple>

/**
 * VertexArrayObject 클래스
 *
 * VAO 객체를 추상화한 클래스
 */
class VertexArrayObject : public IGLObject
{
public:
  VertexArrayObject();

  ~VertexArrayObject() override;

  // interleaved buffer 구조를 고려하여 VBO 객체 연결 함수 구현
  void linkVBO(const VertexBufferObject &vbo, const std::vector<std::tuple<GLuint, GLint, GLenum, GLboolean, GLsizei, const void *>> &attributes);

  void linkIBO(const IndexBufferObject &ibo);

  GLuint getID() const;

  void bind() const override;

  void unbind() const override;

  void destroy() override;

private:
  GLuint ID;
};

#endif // VERTEX_ARRAY_OBJECT_HPP
