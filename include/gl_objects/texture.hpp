#ifndef TEXTURE_HPP
#define TEXTURE_HPP

/*
  #ifndef ~ #endif 전처리기는
  헤더파일의 헤더가드를 처리하여 중복 include 방지해 줌!
*/

#include <glad/glad.h> // OpenGL 함수를 초기화하기 위한 헤더
#include <gl_objects/gl_object.hpp>

/**
 * Texture 클래스
 *
 * Texture 객체를 추상화한 클래스
 */
class Texture final : public IGLObject
{
public:
  // 이미지 url 을 로드하여 텍스쳐 객체 생성
  Texture(const char *url, GLenum format, GLenum internalFormat);

  // 비어있는 텍스쳐 버퍼 객체 생성 -> offscreen rendering 결과를 저장할 용도
  Texture(GLsizei width, GLsizei height, GLenum format, GLenum internalFormat);

  // 소멸자
  ~Texture();

  // 텍스쳐 바인딩
  void bind() const override;

  // 텍스쳐 바인딩 해제
  void unbind() const override;

  // 텍스쳐 메모리 반납
  void destroy() override;

  // 텍스쳐를 바인딩할 texture unit 활성화 및 바인딩
  void use(GLenum textureUnit) const;

  // 텍스쳐 파라미터 설정
  void setWrapS(GLint wrapMode);

  void setWrapT(GLint wrapMode);

  void setMinFilter(GLint filterMode);

  void setMagFilter(GLint filterMode);

  GLuint getID() const;

  // 밉맵 생성
  void generateMipmap();

private:
  GLuint ID;

  GLsizei width = 0;

  GLsizei height = 0;

  GLenum format = GL_RGB16F;

  GLenum internalFormat = GL_RGB;

  GLint wrapS = GL_CLAMP_TO_EDGE;

  GLint wrapT = GL_CLAMP_TO_EDGE;

  GLint minFilter = GL_LINEAR;

  GLint magFilter = GL_LINEAR;
};

#endif // TEXTURE_HPP
