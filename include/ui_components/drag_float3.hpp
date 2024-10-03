#ifndef DRAG_FLOAT3_HPP
#define DRAG_FLOAT3_HPP

/*
  #ifndef ~ #endif 전처리기는
  헤더파일의 헤더가드를 처리하여 중복 include 방지해 줌!
*/

#include <ui_components/ui_component.hpp>
#include <glm/glm.hpp>

/**
 * DragFloat3 클래스
 *
 * ImGui::DragFloat3 요소를 wrapping 하는 UiComponent 클래스
 */
class DragFloat3 : public IUiComponent
{
public:
  DragFloat3();

  bool onUiComponent() override;

  void setLabel(const char *label);
  void setValue(const glm::vec3 &value);
  void setSpeed(const float speed);
  void setMin(const float min);
  void setMax(const float max);

  glm::vec3 getValue() const;

private:
  const char *label_;
  glm::vec3 value_;
  float speed_;
  float min_;
  float max_;
};

#endif // DRAG_FLOAT3_HPP
