#ifndef UI_COMPONENT_HPP
#define UI_COMPONENT_HPP

/*
  #ifndef ~ #endif 전처리기는
  헤더파일의 헤더가드를 처리하여 중복 include 방지해 줌!
*/

/**
 * IUiComponent 클래스
 *
 * ImGui 요소들을 Wrapping 하는 각 UiComponent 클래스들이
 * 상속받는 인터페이스 클래스
 */
class IUiComponent
{
public:
  virtual bool onUiComponent() = 0;
};

#endif // UI_COMPONENT_HPP
