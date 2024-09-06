#include "gl_objects/vertex_array_object.hpp"
#include <stdexcept>

VertexArrayObject::VertexArrayObject()
{
  glGenVertexArrays(1, &ID);

  if (ID == 0)
  {
    throw std::runtime_error("Failed to generate VAO.");
  }
}

VertexArrayObject::~VertexArrayObject()
{
  destroy();
}

void VertexArrayObject::linkVBO(const VertexBufferObject &vbo, const std::vector<std::tuple<GLuint, GLint, GLenum, GLboolean, GLsizei, const void *>> &attributes)
{
  bind();
  vbo.bind();

  /**
   * 현재 프로젝트에서 VBO 객체에 쓰여진 데이터들이 interleaved buffer 구조로 되어있으므로,
   * 각 attribute 설정값을 tuple 자료형의 입력 매개변수로 받아 참조
   *
   * -> 각 attribute 를 순회하며 VBO 데이터 해석 방식 및
   * 버텍스 쉐이더의 각 location 변수 활성화
   */
  for (const auto &attr : attributes)
  {
    // 구조적 바인딩을 사용하여 tuple 각 요소 추출
    auto [index, size, type, normalized, stride, offset] = attr;

    glVertexAttribPointer(index, size, type, normalized, stride, offset);
    glEnableVertexAttribArray(index);
  }

  vbo.unbind();
  unbind();
}

void VertexArrayObject::linkIBO(const IndexBufferObject &ibo)
{
  bind();
  ibo.bind();
  unbind();
}

GLuint VertexArrayObject::getID() const
{
  return ID;
}

void VertexArrayObject::bind() const
{
  glBindVertexArray(ID);
}

void VertexArrayObject::unbind() const
{
  glBindVertexArray(0);
}

void VertexArrayObject::destroy()
{
  if (ID != 0)
  {
    glDeleteVertexArrays(1, &ID);
    ID = 0;
  }
}
