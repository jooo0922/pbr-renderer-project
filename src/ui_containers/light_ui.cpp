#include "ui_containers/light_ui.hpp"

LightUi::LightUi()
{
  lightSelector.setLabel("select light");
  lightSelector.setItems({"light 1", "light 2", "light 3", "light 4"}); // TODO : 만약 빌드 에러 나면 std::vector 로 초기화해서 넘기기

  for (unsigned int i = 0; i < uiList.size(); i++)
  {
    uiList[i].position.setLabel("position");
    uiList[i].position.setMin(-100.f);
    uiList[i].position.setMax(100.f);
    uiList[i].position.setSpeed(0.1f);

    uiList[i].color.setLabel("color");
    uiList[i].color.setColor(glm::vec3(1.0f, 1.0f, 1.0f));

    uiList[i].intensity.setLabel("intensity");
    uiList[i].intensity.setMin(0.f);
    uiList[i].intensity.setMax(1000.f);
    uiList[i].intensity.setSpeed(0.1f);
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
