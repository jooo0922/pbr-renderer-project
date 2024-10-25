#ifndef CAMERA_CONSTANTS_HPP
#define CAMERA_CONSTANTS_HPP

/*
  #ifndef ~ #endif 전처리기는
  헤더파일의 헤더가드를 처리하여 중복 include 방지해 줌!
*/

#include <glm/glm.hpp>

/**
 * Camera 관련 심볼릭 상수 정의
 *
 * 일반적으로 권장되는 심볼릭 상수 정의 방식은 아래와 같음.
 *
 * 1. 헤더 파일 안에 한 곳에 모아서
 * 2. 네임스페이스로 논리적 그룹을 묶어서
 * 3. constexpr 로 선언
 *
 * https://github.com/jooo0922/cpp-study/blob/main/TBCppStudy/Chapter2_9/MY_CONSTANTS.h 참고
 */
namespace CameraConstants
{
  constexpr float YAW_DEFAULT = -90.0f;
  constexpr float YAW_MIN = -180.0f;
  constexpr float YAW_MAX = 80.0f;
  constexpr float YAW_UI_SPEED = 0.1f;
  constexpr const char YAW_UI_LABEL[] = "yaw";

  constexpr float PITCH_DEFAULT = 0.0f;
  constexpr float PITCH_MIN = -89.0f;
  constexpr float PITCH_MAX = 89.0f;
  constexpr float PITCH_UI_SPEED = 0.1f;
  constexpr const char PITCH_UI_LABEL[] = "pitch";

  constexpr float ZOOM_DEFAULT = 45.0f;
  constexpr float ZOOM_MIN = 1.0f;
  constexpr float ZOOM_MAX = 45.0f;
  constexpr float ZOOM_UI_SPEED = 0.1f;
  constexpr const char ZOOM_UI_LABEL[] = "zoom";

  constexpr glm::vec3 POSITION_DEFAULT = glm::vec3(0.0f, 0.0f, 3.0f);
  constexpr float POSITION_MIN = -100.0f;
  constexpr float POSITION_MAX = 100.0f;
  constexpr float POSITION_UI_SPEED = 0.001f;
  constexpr const char POSITION_UI_LABEL[] = "position";
}

#endif // CAMERA_CONSTANTS_HPP
