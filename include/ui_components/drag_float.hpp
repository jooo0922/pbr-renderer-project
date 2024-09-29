#ifndef DRAG_FLOAT_HPP
#define DRAG_FLOAT_HPP

/*
  #ifndef ~ #endif 전처리기는
  헤더파일의 헤더가드를 처리하여 중복 include 방지해 줌!
*/

#include <ui_components/ui_component.hpp>

/**
 * DragFloat 클래스
 *
 * ImGui::DragFloat 요소를 wrapping 하는 UiComponent 클래스
 */
class DragFloat : public IUiComponent
{
public:
  DragFloat();

  bool onUiComponent() override;

  void setLabel(const char *label);
  void setValue(const float value);
  void setSpeed(const float speed);
  void setMin(const float min);
  void setMax(const float max);

  float getValue() const;

private:
  const char *label_;
  float value_;
  float speed_;
  float min_;
  float max_;
};

#endif // DRAG_FLOAT_HPP
