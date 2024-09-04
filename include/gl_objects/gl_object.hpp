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
  /**
   * 가상 소멸자
   *
   * 부모 클래스 소멸자 호출 시,
   * 상속받은 자식 클래스의 소멸자까지 한방에 호출하기 위한 목적
   *
   * https://github.com/jooo0922/cpp-study/blob/main/Chapter12/Chapter12_04/Chapter12_04.cpp 참고
   */
  virtual ~IGLObject() = default;

  // gl object 초기화
  virtual void initialize() = 0;

  // gl object 바인딩
  virtual void bind() const = 0;

  // gl object 바인딩 해제
  virtual void unbind() const = 0;

  // gl object 메모리 반납
  virtual void destroy() = 0;

protected:
  /**
   * 현재 인터페이스 클래스는 인스턴스화할 수 없음을 명시하고,
   * 상속받는 자식 클래스에서만 생성자 함수를 호출하도록 protected 접근자로 지정
   */
  IGLObject() = default;
};

#endif // GL_OBJECT_HPP