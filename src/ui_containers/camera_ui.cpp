#include "ui_containers/camera_ui.hpp"

CameraUi::CameraUi()
{
  yaw.setLabel("yaw");
  yaw.setMin(-180.f);
  yaw.setMax(180.f);
  yaw.setSpeed(1.f);

  pitch.setLabel("pitch");
  pitch.setMin(-89.f);
  pitch.setMax(89.f);
  pitch.setSpeed(1.f);

  zoom.setLabel("zoom");
  zoom.setMin(1.f);
  zoom.setMax(45.f);
  zoom.setSpeed(1.f);

  position.setLabel("position");
  position.setMin(-50.f);
  position.setMax(50.f);
  position.setSpeed(1.f);
}

CameraUi::~CameraUi()
{
}

bool CameraUi::onUiComponents()
{
  bool ret = false;
  ret |= yaw.onUiComponent();
  ret |= pitch.onUiComponent();
  ret |= zoom.onUiComponent();
  ret |= position.onUiComponent();
  return ret;
}

void CameraUi::onChange(const CameraParameter &param)
{
  yaw.setValue(param.yaw);
  pitch.setValue(param.pitch);
  zoom.setValue(param.zoom);
  position.setValue(param.position);
}

void CameraUi::getCameraParam(CameraParameter &param) const
{
  param.yaw = yaw.getValue();
  param.pitch = pitch.getValue();
  param.zoom = zoom.getValue();
  param.position = position.getValue();
}
