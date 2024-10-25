#include "ui_containers/ibl_ui.hpp"
#include "constants/ibl_constants.hpp"
#include <vector>

IBLUi::IBLUi()
{
  std::vector<const char *> hdrImageLabels;
  for (const auto &image : OffscreenRenderingConstants::HDR_IMAGES)
  {
    hdrImageLabels.push_back(image.label);
  }

  hdrImageSelector.setLabel(IBLConstants::HDR_IMAGE_SELECTOR_UI_LABEL);
  hdrImageSelector.setItems(hdrImageLabels);

  iblVisibility.setLabel(IBLConstants::IBL_VISIBILITY_UI_LABEL);

  skyboxVisibility.setLabel(IBLConstants::SKYBOX_VISIBILITY_UI_LABEL);

  iblIntensity.setLabel(IBLConstants::IBL_INTENSITY_UI_LABEL);
  iblIntensity.setMin(IBLConstants::IBL_INTENSITY_MIN);
  iblIntensity.setMax(IBLConstants::IBL_INTENSITY_MAX);
  iblIntensity.setSpeed(IBLConstants::IBL_INTENSITY_UI_SPEED);
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
