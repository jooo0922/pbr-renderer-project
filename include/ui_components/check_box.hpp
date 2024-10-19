#ifndef CHECK_BOX_HPP
#define CHECK_BOX_HPP

#include <ui_components/ui_component.hpp>

/**
 * CheckBox 클래스
 *
 * ImGui::CheckBox 요소를 wrapping 하는 UiComponent 클래스
 */
class CheckBox : public IUiComponent
{
public:
  CheckBox();

  bool onUiComponent() override;

  void setLabel(const char *label);
  void setValue(const bool value);

  bool getValue() const;

private:
  const char *label_;
  bool value_;
};

#endif /* CHECK_BOX_HPP */
