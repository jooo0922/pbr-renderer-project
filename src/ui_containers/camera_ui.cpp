#include "ui_containers/camera_ui.hpp"
#include "constants/camera_constansts.hpp"

CameraUi::CameraUi()
{
  yaw.setLabel(CameraConstants::YAW_UI_LABEL);
  yaw.setMin(CameraConstants::YAW_MIN);
  yaw.setMax(CameraConstants::YAW_MAX);
  yaw.setSpeed(CameraConstants::YAW_UI_SPEED);

  pitch.setLabel(CameraConstants::PITCH_UI_LABEL);
  pitch.setMin(CameraConstants::PITCH_MIN);
  pitch.setMax(CameraConstants::PITCH_MAX);
  pitch.setSpeed(CameraConstants::PITCH_UI_SPEED);

  zoom.setLabel(CameraConstants::ZOOM_UI_LABEL);
  zoom.setMin(CameraConstants::ZOOM_MIN);
  zoom.setMax(CameraConstants::ZOOM_MAX);
  zoom.setSpeed(CameraConstants::ZOOM_UI_SPEED);

  position.setLabel(CameraConstants::POSITION_UI_LABEL);
  position.setMin(CameraConstants::POSITION_MIN);
  position.setMax(CameraConstants::POSITION_MAX);
  position.setSpeed(CameraConstants::POSITION_UI_SPEED);
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
