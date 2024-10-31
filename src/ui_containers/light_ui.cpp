#include "ui_containers/light_ui.hpp"

LightUi::LightUi()
{
  std::vector<const char *> lightLabels;
  for (const auto &lightUiData : LightConstants::lightUiDataArray)
  {
    lightLabels.push_back(lightUiData.label);
  }

  lightSelector.setLabel(LightConstants::LIGHT_SELECTOR_UI_LABEL);
  lightSelector.setItems(lightLabels);

  for (unsigned int i = 0; i < uiList.size(); i++)
  {
    uiList[i].position.setLabel(LightConstants::POSITION_UI_LABEL);
    uiList[i].position.setMin(LightConstants::POSITION_MIN);
    uiList[i].position.setMax(LightConstants::POSITION_MAX);
    uiList[i].position.setSpeed(LightConstants::POSITION_UI_SPEED);

    uiList[i].color.setLabel(LightConstants::COLOR_UI_LABEL);
    uiList[i].color.setColor(LightConstants::COLOR_UI_DEFAULT);

    uiList[i].intensity.setLabel(LightConstants::INTENSITY_UI_LABEL);
    uiList[i].intensity.setMin(LightConstants::INTENSITY_MIN);
    uiList[i].intensity.setMax(LightConstants::INTENSITY_MAX);
    uiList[i].intensity.setSpeed(LightConstants::INTENSITY_UI_SPEED);
  }
}

LightUi::~LightUi()
{
}

bool LightUi::onUiComponents()
{
  bool ret = false;

  lightSelector.onUiComponent();

  const int currentIndex = lightSelector.getCurrentIndex();

  ret |= uiList[currentIndex].position.onUiComponent();
  ret |= uiList[currentIndex].color.onUiComponent();
  ret |= uiList[currentIndex].intensity.onUiComponent();

  return ret;
}

void LightUi::onChange(const LightParameter &param)
{
  unsigned int lightIndex = 0;
  for (const auto &lightData : param.lightDataArray)
  {
    uiList[lightIndex].position.setValue(lightData.position);
    uiList[lightIndex].color.setColor(lightData.color);
    uiList[lightIndex].intensity.setValue(lightData.intensity);

    lightIndex += 1;
  }
}

void LightUi::getLightParam(LightParameter &param) const
{
  for (unsigned int lightIndex = 0; lightIndex < param.lightDataArray.size(); lightIndex++)
  {
    param.lightDataArray[lightIndex].position = uiList[lightIndex].position.getValue();
    param.lightDataArray[lightIndex].color = uiList[lightIndex].color.getColor();
    param.lightDataArray[lightIndex].intensity = uiList[lightIndex].intensity.getValue();
  }
}
