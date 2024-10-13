#ifndef CUBE_TEXTURE_HPP
#define CUBE_TEXTURE_HPP

/*
  #ifndef ~ #endif 전처리기는
  헤더파일의 헤더가드를 처리하여 중복 include 방지해 줌!
*/

#include <glad/glad.h> // OpenGL 함수를 초기화하기 위한 헤더
#include <gl_objects/gl_object.hpp>

/**
 * CubeTexture 클래스
 *
 * CubeTexture 객체를 추상화한 클래스
 * Texture 클래스와 달리, stb_image 라이브러리를 사용한 이미지 로드 기능을 지원하지 않음.
 *
 * -> 현재 프로젝트에서 CubeTexture 는 offscreen rendering 결과를 저장하기 위한
 * 텍스쳐 버퍼 역할만 하고 있으므로, CubeTexture 클래스에는 해당 목적으로만 사용함.
 */
class CubeTexture : public IGLObject
{
public:
  CubeTexture() = default;

  // 비어있는 텍스쳐 버퍼 객체 생성 -> offscreen rendering 결과를 저장할 용도
  CubeTexture(GLsizei width, GLsizei height, GLenum format, GLenum internalFormat);

  // 소멸자
  ~CubeTexture();

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

  void setWrapR(GLint wrapMode);

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

  GLint wrapR = GL_CLAMP_TO_EDGE;

  GLint minFilter = GL_LINEAR;

  GLint magFilter = GL_LINEAR;
};

#endif // CUBE_TEXTURE_HPP
