#ifndef IBL_UI_HPP
#define IBL_UI_HPP

#include "features/ibl_feature.hpp"
#include "constants/offscreen_rendering_constants.hpp"
#include "ui_components/check_box.hpp"
#include "ui_components/drag_float.hpp"
#include "ui_components/combo.hpp"

/**
 * IBLUi 클래스
 *
 * IBL 관련 파라미터들의 UI 입력을 처리하는
 * UiComponent 요소들을 관리하는 UI 컨테이너 클래스
 */
class IBLUi : public IListener<IBLParameter>
{
public:
  IBLUi();
  ~IBLUi();

  bool onUiComponents();
  void onChange(const IBLParameter &param) override;

  void getIBLParam(IBLParameter &param) const;

private:
  CheckBox iblVisibility;
  CheckBox skyboxVisibility;
  DragFloat iblIntensity;
  Combo hdrImageSelector;
};

#endif /* IBL_UI_HPP */
