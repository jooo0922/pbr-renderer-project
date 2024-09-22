#ifndef LISTENER_HPP
#define LISTENER_HPP

/*
  #ifndef ~ #endif 전처리기는
  헤더파일의 헤더가드를 처리하여 중복 include 방지해 줌!
*/

/**
 * IListener 인터페이스 클래스
 *
 * Feature 클래스를 리스너로 등록하기 위한 인터페이스 클래스.
 *
 * 상속받은 각 Feature 클래스들이 관리하는 파라미터 타입을
 * 템플릿 파라미터로 전달하기 위해 IListener 클래스를 템플릿 클래스로 구현
 */
template <typename ParameterType>
class IListener
{
public:
  virtual void onChange(const ParameterType &param) = 0;
};

#endif // LISTENER_HPP
