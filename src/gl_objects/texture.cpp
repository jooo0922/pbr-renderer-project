#include "gl_objects/texture.hpp"

// 이미지 파일 로드 라이브러리 include (관련 설명 하단 참고)
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#include <stdexcept>
#include <string>

Texture::Texture(const char *url, GLenum format, GLenum internalFormat)
    : format(format), internalFormat(internalFormat)
{
  // 텍스쳐 이미지 로드 후, y축 방향으로 뒤집어 줌 > OpenGL 이 텍스쳐 좌표를 읽는 방향과 이미지의 픽셀 좌표가 반대라서!
  stbi_set_flip_vertically_on_load(true);

  int nrComponents;

  // 이미지 데이터 가져와서 float 타입의 bytes 데이터로 저장.
  // 이미지 width, height, 색상 채널 변수의 주소값도 넘겨줌으로써, 해당 함수 내부에서 값을 변경. -> 출력변수 역할
  float *data = stbi_loadf(url, &width, &height, &nrComponents, 0);

  if (data)
  {
    // 텍스쳐 객체 생성 및 바인딩
    glGenTextures(1, &ID);

    bind();

    /*
      [0, 1] 범위를 넘어선 HDR 이미지 데이터(data)들을 온전히 저장하기 위해,
      GL_RGB16F floating point(부동 소수점) 포맷으로 프레임버퍼의 내부 색상 포맷 지정
      (하단 Floating point framebuffer 관련 필기 참고)
    */
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, internalFormat, GL_FLOAT, data);

    // 현재 GL_TEXTURE_2D 상태에 바인딩된 텍스쳐 객체 설정하기
    // Texture Wrapping 모드를 반복 모드로 설정 ([(0, 0), (1, 1)] 범위를 벗어나는 텍스쳐 좌표에 대한 처리)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);

    // 텍스쳐 축소/확대 및 Mipmap 교체 시 Texture Filtering (텍셀 필터링(보간)) 모드 설정
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);

    // 텍스쳐 객체에 이미지 데이터를 전달하고, 밉맵까지 생성 완료했다면, 로드한 이미지 데이터는 항상 메모리 해제할 것!
    stbi_image_free(data);
  }
  else
  {
    std::runtime_error("Failed to load image: " + std::string(url));
  }
}

Texture::Texture(GLsizei width, GLsizei height, GLenum format, GLenum internalFormat)
    : width(width), height(height), format(format), internalFormat(internalFormat)
{
  // 텍스쳐 객체 생성 및 바인딩
  glGenTextures(1, &ID);

  bind();

  /*
    [0, 1] 범위를 넘어선 HDR 이미지 데이터(data)들을 온전히 저장하기 위해,
    GL_RGB16F floating point(부동 소수점) 포맷으로 프레임버퍼의 내부 색상 포맷 지정
    (하단 Floating point framebuffer 관련 필기 참고)
  */
  glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, internalFormat, GL_FLOAT, nullptr);

  // 현재 GL_TEXTURE_2D 상태에 바인딩된 텍스쳐 객체 설정하기
  // Texture Wrapping 모드를 반복 모드로 설정 ([(0, 0), (1, 1)] 범위를 벗어나는 텍스쳐 좌표에 대한 처리)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);

  // 텍스쳐 축소/확대 및 Mipmap 교체 시 Texture Filtering (텍셀 필터링(보간)) 모드 설정
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
}

Texture::~Texture()
{
  destroy();
}

void Texture::bind() const
{
  glBindTexture(GL_TEXTURE_2D, ID);
}

void Texture::unbind() const
{
  glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::destroy()
{
  glDeleteTextures(1, &ID);
}

void Texture::use(GLenum textureUnit) const
{
  glActiveTexture(textureUnit);

  bind();
}

void Texture::setWrapS(GLint wrapMode)
{
  wrapS = wrapMode;

  bind();

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);

  unbind();
}

void Texture::setWrapT(GLint wrapMode)
{
  wrapT = wrapMode;

  bind();

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);

  unbind();
}

void Texture::setMinFilter(GLint filterMode)
{
  minFilter = filterMode;

  bind();

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);

  unbind();
}

void Texture::setMagFilter(GLint filterMode)
{
  magFilter = filterMode;

  bind();

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);

  unbind();
}

GLuint Texture::getID() const
{
  return ID;
}

void Texture::generateMipmap()
{
  bind();

  glGenerateMipmap(GL_TEXTURE_2D);

  unbind();
}

/*
  stb_image.h

  주요 이미지 파일 포맷을 로드할 수 있는
  싱글 헤더 이미지로드 라이브러리.

  #define 매크로 전처리기를 통해
  특정 매크로를 선언함으로써, 헤더파일 내에서
  해당 매크로 영역의 코드만 include 할 수 있도록 함.

  실제로 stb_image.h 안에 보면

  #ifdef STB_IMAGE_IMPLEMENTATION
  ~
  #endif

  요렇게 전처리기가 정의되어 있는 부분이 있음.
  이 부분의 코드들만 include 하겠다는 것이지!
*/

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
