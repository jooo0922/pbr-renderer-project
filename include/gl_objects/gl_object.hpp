#ifndef GL_OBJECT_HPP
#define GL_OBJECT_HPP

/*
  #ifndef ~ #endif 전처리기는
  헤더파일의 헤더가드를 처리하여 중복 include 방지해 줌!
*/

/* IGLObject 인터페이스 클래스 */
class IGLObject
{
public:
  // gl object 바인딩
  virtual void bind() const = 0;

  // gl object 바인딩 해제
  virtual void unbind() const = 0;

  // gl object 메모리 반납
  virtual void destroy() = 0;
};

#endif // GL_OBJECT_HPP
