#ifndef LIGHT_UI_HPP
#define LIGHT_UI_HPP

/*
  #ifndef ~ #endif 전처리기는
  헤더파일의 헤더가드를 처리하여 중복 include 방지해 줌!
*/

#include "features/light_feature.hpp"
#include "ui_components/drag_float.hpp"
#include "ui_components/drag_float3.hpp"
#include "ui_components/color_edit.hpp"
#include "ui_components/combo.hpp"

struct LightUiSet
{
  DragFloat3 position;
  ColorEdit color;
  DragFloat intensity;
};

/**
 * LightUi 클래스
 *
 * camera 관련 파라미터들의 UI 입력을 처리하는
 * UiComponent 요소들을 관리하는 UI 컨테이너 클래스
 */
class LightUi : public IListener<LightParameter>
{
public:
  LightUi();
  ~LightUi();

  bool onUiComponents();
  void onChange(const LightParameter &param) override;

  void getLightParam(LightParameter &param) const;

private:
  Combo lightSelector;
  std::array<LightUiSet, LightConstants::NUM_LIGHTS> uiList;
};

#endif /* LIGHT_UI_HPP */
