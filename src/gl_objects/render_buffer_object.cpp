#include "gl_objects/render_buffer_object.hpp"
#include <stdexcept>

RenderBufferObject::RenderBufferObject()
{
  glGenRenderbuffers(1, &ID);

  if (ID == 0)
  {
    throw std::runtime_error("Failed to generate RBO.");
  }
}

RenderBufferObject::~RenderBufferObject()
{
  destroy();
}

void RenderBufferObject::setStorage(GLsizei width, GLsizei height) const
{
  // RBO 객체 메모리 공간 할당 -> 단일 Renderbuffer 에 depth 값만 저장하는 데이터 포맷 지정(GL_DEPTH_COMPONENT24)
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
}

GLuint RenderBufferObject::getID() const
{
  return ID;
}

void RenderBufferObject::bind() const
{
  glBindRenderbuffer(GL_RENDERBUFFER, ID);
}

void RenderBufferObject::unbind() const
{
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void RenderBufferObject::destroy()
{
  if (ID != 0)
  {
    glDeleteRenderbuffers(1, &ID);
    ID = 0;
  }
}
