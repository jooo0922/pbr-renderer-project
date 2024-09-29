#ifndef COLOR_EDIT_HPP
#define COLOR_EDIT_HPP

/*
  #ifndef ~ #endif 전처리기는
  헤더파일의 헤더가드를 처리하여 중복 include 방지해 줌!
*/

#include <glm/glm.hpp>
#include <ui_components/ui_component.hpp>

/**
 * ColorEdit 클래스
 *
 * ImGui::ColorEdit3 요소를 wrapping 하는 UiComponent 클래스
 */
class ColorEdit : public IUiComponent
{
public:
  ColorEdit();

  bool onUiComponent() override;

  void setLabel(const char *label);
  void setColor(const glm::vec3 &color);

  glm::vec3 getColor() const;

private:
  const char *label_;
  glm::vec3 color_;
};

#endif // COLOR_EDIT_HPP
