#ifndef MODEL_UI_HPP
#define MODEL_UI_HPP

#include "features/model_feature.hpp"
#include "ui_components/drag_float3.hpp"
#include "ui_components/combo.hpp"

/**
 * ModelUi 클래스
 *
 * model 관련 파라미터들의 UI 입력을 처리하는
 * UiComponent 요소들을 관리하는 UI 컨테이너 클래스
 */
class ModelUi : public IListener<ModelParameter>
{
public:
  ModelUi();
  ~ModelUi();

  bool onUiComponents();
  void onChange(const ModelParameter &param) override;

  void getModelParam(ModelParameter &param) const;

private:
  DragFloat3 position;
  DragFloat3 rotation;
  DragFloat3 scale;
  Combo modelSelector;
};

#endif /* MODEL_UI_HPP */
