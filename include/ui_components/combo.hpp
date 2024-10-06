#ifndef COMBO_HPP
#define COMBO_HPP

/*
  #ifndef ~ #endif 전처리기는
  헤더파일의 헤더가드를 처리하여 중복 include 방지해 줌!
*/

#include <ui_components/ui_component.hpp>
#include <vector>

/**
 * Combo 클래스
 *
 * ImGui::Combo 요소를 wrapping 하는 UiComponent 클래스
 */
class Combo : public IUiComponent
{
public:
  Combo();

  bool onUiComponent() override;

  void setLabel(const char *label);
  void setItems(const std::vector<const char *> &items);
  void setCurrentIndex(int index);

  int getCurrentIndex() const;

private:
  const char *label_;
  std::vector<const char *> items_;
  int currentIndex_;
};

#endif // COMBO_HPP
