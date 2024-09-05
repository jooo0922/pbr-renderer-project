#ifndef INDEX_BUFFER_OBJECT_HPP
#define INDEX_BUFFER_OBJECT_HPP

/*
  #ifndef ~ #endif 전처리기는
  헤더파일의 헤더가드를 처리하여 중복 include 방지해 줌!
*/

#include <glad/glad.h> // OpenGL 함수를 초기화하기 위한 헤더
#include <gl_object.hpp>

/**
 * IndexBufferObject 클래스
 *
 * IBO 객체를 추상화한 클래스
 */
class IndexBufferObject : public IGLObject
{
public:
  IndexBufferObject();

  ~IndexBufferObject() override;

  void setData(const void *data, GLsizeiptr size, GLenum usage);

  GLuint getID() const;

  void bind() const;

  void unbind() const;

  void destroy();

private:
  GLuint ID;
};

#endif // INDEX_BUFFER_OBJECT_HPP
