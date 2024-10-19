#include "ui_containers/ibl_ui.hpp"
#include <vector>

IBLUi::IBLUi()
{
  std::vector<const char *> hdrImageLabels;
  for (const auto &image : OffscreenRenderingConstants::HDR_IMAGES)
  {
    hdrImageLabels.push_back(image.label);
  }

  hdrImageSelector.setLabel("select HDR Images");
  hdrImageSelector.setItems(hdrImageLabels);

  iblVisibility.setLabel("apply IBL");

  skyboxVisibility.setLabel("show environment");

  iblIntensity.setLabel("IBL intensity");
  iblIntensity.setMin(0.f);
  iblIntensity.setMax(1.f);
  iblIntensity.setSpeed(0.001f);
}

IBLUi::~IBLUi()
{
}

bool IBLUi::onUiComponents()
{
  bool ret = false;
  ret |= iblVisibility.onUiComponent();
  ret |= skyboxVisibility.onUiComponent();
  ret |= iblIntensity.onUiComponent();
  ret |= hdrImageSelector.onUiComponent();
  return ret;
}

void IBLUi::onChange(const IBLParameter &param)
{
  iblVisibility.setValue(param.iblVisibility);
  skyboxVisibility.setValue(param.skyboxVisibility);
  iblIntensity.setValue(param.iblIntensity);
  hdrImageSelector.setCurrentIndex(param.hdrImageIndex);
}

void IBLUi::getIBLParam(IBLParameter &param) const
{
  param.iblVisibility = iblVisibility.getValue();
  param.skyboxVisibility = skyboxVisibility.getValue();
  param.iblIntensity = iblIntensity.getValue();
  param.hdrImageIndex = hdrImageSelector.getCurrentIndex();
}
