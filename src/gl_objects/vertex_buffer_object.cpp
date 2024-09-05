#include "gl_objects/vertex_buffer_object.hpp"
#include <stdexcept>

VertexBufferObject::VertexBufferObject()
{
  glGenBuffers(1, &ID);

  if (ID == 0)
  {
    throw std::runtime_error("Failed to generate VBO.");
  }
}

VertexBufferObject::~VertexBufferObject()
{
  destroy();
}

void VertexBufferObject::setData(const void *data, GLsizeiptr size, GLenum usage)
{
  if (ID == 0)
  {
    throw std::runtime_error("VBO not initialized.");
  }

  bind();

  glBufferData(GL_ARRAY_BUFFER, size, data, usage);

  unbind();
}

GLuint VertexBufferObject::getID() const
{
  return ID;
}

void VertexBufferObject::bind() const
{
  glBindBuffer(GL_ARRAY_BUFFER, ID);
}

void VertexBufferObject::unbind() const
{
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexBufferObject::destroy()
{
  if (ID != 0)
  {
    glDeleteBuffers(1, &ID);
    ID = 0;
  }
}
