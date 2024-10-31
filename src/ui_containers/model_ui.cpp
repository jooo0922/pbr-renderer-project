#include "ui_containers/model_ui.hpp"
#include "constants/model_constants.hpp"
#include <vector>

ModelUi::ModelUi()
{
  std::vector<const char *> modelLabels;
  for (const auto &model : ModelConstants::models)
  {
    modelLabels.push_back(model.label);
  }

  modelSelector.setLabel(ModelConstants::MODEL_SELECTOR_UI_LABEL);
  modelSelector.setItems(modelLabels);

  position.setLabel(ModelConstants::POSITION_UI_LABEL);
  position.setMin(ModelConstants::POSITION_MIN);
  position.setMax(ModelConstants::POSITION_MAX);
  position.setSpeed(ModelConstants::POSITION_UI_SPEED);

  rotation.setLabel(ModelConstants::ROTATION_UI_LABEL);
  rotation.setMin(ModelConstants::ROTATION_MIN);
  rotation.setMax(ModelConstants::ROTATION_MAX);
  rotation.setSpeed(ModelConstants::ROTATION_UI_SPEED);

  scale.setLabel(ModelConstants::SCALE_UI_LABEL);
  scale.setMin(ModelConstants::SCALE_MIN);
  scale.setMax(ModelConstants::SCALE_MAX);
  scale.setSpeed(ModelConstants::SCALE_UI_SPEED);
}

ModelUi::~ModelUi()
{
}

bool ModelUi::onUiComponents()
{
  bool ret = false;
  ret |= position.onUiComponent();
  ret |= rotation.onUiComponent();
  ret |= scale.onUiComponent();
  ret |= modelSelector.onUiComponent();
  return ret;
}

void ModelUi::onChange(const ModelParameter &param)
{
  position.setValue(param.position);
  rotation.setValue(param.rotation);
  scale.setValue(param.scale);
  modelSelector.setCurrentIndex(param.modelIndex);
}

void ModelUi::getModelParam(ModelParameter &param) const
{
  param.position = position.getValue();
  param.rotation = rotation.getValue();
  param.scale = scale.getValue();
  param.modelIndex = modelSelector.getCurrentIndex();
}
