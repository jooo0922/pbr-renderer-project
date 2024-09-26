#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

/*
  #ifndef ~ #endif 전처리기는
  헤더파일의 헤더가드를 처리하여 중복 include 방지해 줌!
*/

#include <set>
#include <common/listener.hpp>

/**
 * Controller 클래스
 *
 * 각 Feature 클래스들을 리스너로 등록하여 관리하는 클래스
 *
 * ImGui 입력 변경사항을 등록된 리스너 객체들에 notify 하기 위해
 * 옵저버 패턴으로 구현됨.
 *
 * -> 템플릿 클래스는 헤더 파일에 선언부와 구현부가 모두 포함되어 있어야
 * 템플릿의 자료형을 찾지 못하는 컴파일 에러가 발생하지 않음!
 */
template <typename ParameterType>
class Controller
{
public:
  void setValue(const ParameterType &param)
  {
    parameter = param;
  }

  ParameterType getValue() const
  {
    return parameter;
  }

  void addListener(IListener<ParameterType> &listener)
  {
    listeners.insert(&listener);
  }

  void removeListener(IListener<ParameterType> &listener)
  {
    listeners.erase(&listener);
  }

private:
  ParameterType parameter;
  std::set<IListener<ParameterType> *> listeners;

  void notify()
  {
    for (auto listener : listeners)
    {
      listener->onChange(parameter);
    }
  }
};

#endif // CONTROLLER_HPP
