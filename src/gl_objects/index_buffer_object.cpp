#include "gl_objects/index_buffer_object.hpp"
#include <stdexcept>

IndexBufferObject::IndexBufferObject()
{
  glGenBuffers(1, &ID);

  if (ID == 0)
  {
    throw std::runtime_error("Failed to generate IBO.");
  }
}

IndexBufferObject::~IndexBufferObject()
{
  destroy();
}

void IndexBufferObject::setData(const void *data, GLsizeiptr size, GLenum usage)
{
  if (ID == 0)
  {
    throw std::runtime_error("IBO not initialized.");
  }

  bind();

  glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, usage);

  unbind();
}

GLuint IndexBufferObject::getID() const
{
  return ID;
}

void IndexBufferObject::bind() const
{
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
}

void IndexBufferObject::unbind() const
{
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void IndexBufferObject::destroy()
{
  if (ID != 0)
  {
    glDeleteBuffers(1, &ID);
    ID = 0;
  }
}
