#ifndef MATERIAL_UI_HPP
#define MATERIAL_UI_HPP

/*
  #ifndef ~ #endif 전처리기는
  헤더파일의 헤더가드를 처리하여 중복 include 방지해 줌!
*/

#include "features/material_feature.hpp"
#include "ui_components/drag_float.hpp"
#include "ui_components/color_edit.hpp"

/**
 * MaterialUi 클래스
 *
 * material 관련 파라미터들의 UI 입력을 처리하는
 * UiComponent 요소들을 관리하는 UI 컨테이너 클래스
 */
class MaterialUi : public IListener<MaterialParameter>
{
public:
  MaterialUi();
  ~MaterialUi();

  bool onUiComponents();
  void onChange(const MaterialParameter &param) override;

  void getMaterialParam(MaterialParameter &param) const;

private:
  DragFloat roughness;
  DragFloat metallic;
  DragFloat ambientOcclusion;
  ColorEdit albedo;
};

#endif // MATERIAL_UI_HPP
