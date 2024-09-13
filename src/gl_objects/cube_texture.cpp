#include "gl_objects/cube_texture.hpp"
#include <stdexcept>
#include <string>

CubeTexture::CubeTexture(GLsizei width, GLsizei height, GLenum format, GLenum internalFormat)
    : width(width), height(height), format(format), internalFormat(internalFormat)
{
  // 텍스쳐 객체 생성 및 바인딩
  glGenTextures(1, &ID);

  bind();

  // 반복문을 순회하며 Cubemap 각 6면에 이미지 데이터를 저장할 메모리 할당
  for (unsigned int i = 0; i < 6; i++)
  {
    /*
      Cubemap 텍스쳐 또한 [0, 1] 범위를 넘어선 HDR 이미지 데이터(data)들을 온전히 저장하기 위해,

      GL_RGB16F floating point(부동 소수점) 포맷으로 프레임버퍼의 내부 색상 포맷 지정
      (하단 Floating point framebuffer 관련 필기 참고)
    */
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, internalFormat, GL_FLOAT, nullptr);
  }

  // 현재 GL_TEXTURE_CUBE_MAP 상태에 바인딩된 텍스쳐 객체 설정하기
  // CubeTexture Wrapping 모드를 반복 모드로 설정 ([(0, 0), (1, 1)] 범위를 벗어나는 텍스쳐 좌표에 대한 처리)
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  // 텍스쳐 축소/확대 및 Mipmap 교체 시 CubeTexture Filtering (텍셀 필터링(보간)) 모드 설정
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
}

CubeTexture::~CubeTexture()
{
  destroy();
}

void CubeTexture::bind() const
{
  glBindTexture(GL_TEXTURE_CUBE_MAP, ID);
}

void CubeTexture::unbind() const
{
  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void CubeTexture::destroy()
{
  glDeleteTextures(1, &ID);
}

void CubeTexture::use(GLenum textureUnit) const
{
  glActiveTexture(textureUnit);

  bind();
}

void CubeTexture::setWrapS(GLint wrapMode)
{
  wrapS = wrapMode;

  bind();

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, wrapS);

  unbind();
}

void CubeTexture::setWrapT(GLint wrapMode)
{
  wrapT = wrapMode;

  bind();

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, wrapT);

  unbind();
}

void CubeTexture::setWrapR(GLint wrapMode)
{
  wrapR = wrapMode;

  bind();

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, wrapR);

  unbind();
}

void CubeTexture::setMinFilter(GLint filterMode)
{
  minFilter = filterMode;

  bind();

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, minFilter);

  unbind();
}

void CubeTexture::setMagFilter(GLint filterMode)
{
  magFilter = filterMode;

  bind();

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, magFilter);

  unbind();
}

GLuint CubeTexture::getID() const
{
  return ID;
}

void CubeTexture::generateMipmap()
{
  bind();

  glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

  unbind();
}

/*
  Floating point framebuffer


  HDR 을 구현하려면, [0, 1] 범위를 넘어서는 색상값들이
  프레임버퍼에 attach 된 텍스쳐 객체에 저장될 때,
  [0, 1] 사이로 clamping 되지 않고,

  원래의 색상값이 그대로 저장될 수 있어야 함.

  그러나, 일반적인 프레임버퍼에서 color 를 저장할 때,
  내부 포맷으로 사용하는 GL_RGB 같은 포맷은
  fixed point(고정 소수점) 포맷이기 때문에,

  OpenGL 에서 프레임버퍼에 색상값을 저장하기 전에
  자동으로 [0, 1] 사이의 값으로 clamping 해버리는 문제가 있음.


  이를 해결하기 위해,
  GL_RGB16F, GL_RGBA16F, GL_RGB32F, GL_RGBA32F 같은
  floating point(부동 소수점) 포맷으로
  프레임버퍼의 내부 색상 포맷을 변경하면,

  [0, 1] 범위를 벗어나는 값들에 대해서도
  부동 소수점 형태로 저장할 수 있도록 해줌!


  이때, 일반적인 프레임버퍼의 기본 색상 포맷인
  GL_RGB 같은 경우 하나의 컴포넌트 당 8 bits 메모리를 사용하는데,
  GL_RGB32F, GL_RGBA32F 같은 포맷은 하나의 컴포넌트 당 32 bits 의 메모리를 사용하기 때문에,
  우리는 이 정도로 많은 메모리를 필요로 하지는 않음.

  따라서, GL_RGB16F, GL_RGBA16F 같이
  한 컴포넌트 당 16 bits 정도의 메모리를 예약해서 사용하는
  적당한 크기의 색상 포맷으로 사용하는 게 좋겠지!
*/
