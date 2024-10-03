#ifndef CAMERA_UI_HPP
#define CAMERA_UI_HPP

/*
  #ifndef ~ #endif 전처리기는
  헤더파일의 헤더가드를 처리하여 중복 include 방지해 줌!
*/

#include "features/camera_feature.hpp"
#include "ui_components/drag_float.hpp"
#include "ui_components/drag_float3.hpp"

/**
 * CameraUi 클래스
 *
 * camera 관련 파라미터들의 UI 입력을 처리하는
 * UiComponent 요소들을 관리하는 UI 컨테이너 클래스
 */
class CameraUi : public IListener<CameraParameter>
{
public:
  CameraUi();
  ~CameraUi();

  bool onUiComponents();
  void onChange(const CameraParameter &param) override;

  void getCameraParam(CameraParameter &param) const;

private:
  DragFloat yaw;
  DragFloat pitch;
  DragFloat zoom;
  DragFloat3 position;
};

#endif // CAMERA_UI_HPP
