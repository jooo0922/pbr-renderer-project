#include "gl_objects/frame_buffer_object.hpp"
#include <stdexcept>

FrameBufferObject::FrameBufferObject()
{
  glGenFramebuffers(1, &ID);

  if (ID == 0)
  {
    throw std::runtime_error("Failed to generate FBO.");
  }
}

FrameBufferObject::~FrameBufferObject()
{
  destroy();
}

void FrameBufferObject::attachTexture(GLuint textureID, GLenum target, GLint mipLevel) const
{
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, target, textureID, mipLevel);
}

void FrameBufferObject::attachRenderBuffer(GLuint renderBufferID) const
{
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderBufferID);
}

GLuint FrameBufferObject::getID() const
{
  return ID;
}

void FrameBufferObject::bind() const
{
  glBindFramebuffer(GL_FRAMEBUFFER, ID);
}

void FrameBufferObject::unbind() const
{
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBufferObject::destroy()
{
  if (ID != 0)
  {
    glDeleteFramebuffers(1, &ID);
    ID = 0;
  }
}
