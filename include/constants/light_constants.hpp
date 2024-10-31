#ifndef LIGHT_CONSTANTS_HPP
#define LIGHT_CONSTANTS_HPP

/*
  #ifndef ~ #endif 전처리기는
  헤더파일의 헤더가드를 처리하여 중복 include 방지해 줌!
*/

#include <array>
#include <glm/glm.hpp>

/**
 * Light 관련 심볼릭 상수 정의
 *
 * 일반적으로 권장되는 심볼릭 상수 정의 방식은 아래와 같음.
 *
 * 1. 헤더 파일 안에 한 곳에 모아서
 * 2. 네임스페이스로 논리적 그룹을 묶어서
 * 3. constexpr 로 선언
 *
 * https://github.com/jooo0922/cpp-study/blob/main/TBCppStudy/Chapter2_9/MY_CONSTANTS.h 참고
 */
namespace LightConstants
{
  constexpr std::size_t NUM_LIGHTS = 4;

  struct LightUiData
  {
    const char *label;
    glm::vec3 position;
  };

  constexpr std::array<LightUiData, NUM_LIGHTS> lightUiDataArray = {{
      {"light 1", glm::vec3(-10.0f, 10.0f, 10.0f)},
      {"light 2", glm::vec3(10.0f, 10.0f, 10.0f)},
      {"light 3", glm::vec3(-10.0f, -10.0f, 10.0f)},
      {"light 4", glm::vec3(10.0f, -10.0f, 10.0f)},
  }};

  constexpr const char LIGHT_SELECTOR_UI_LABEL[] = "select light";

  constexpr float INTENSITY_DEFAULT = 300.f;
  constexpr float INTENSITY_MIN = 0.0f;
  constexpr float INTENSITY_MAX = 1000.0f;
  constexpr float INTENSITY_UI_SPEED = 0.1f;
  constexpr const char INTENSITY_UI_LABEL[] = "intensity";

  constexpr float POSITION_MIN = -100.0f;
  constexpr float POSITION_MAX = 100.0f;
  constexpr float POSITION_UI_SPEED = 0.1f;
  constexpr const char POSITION_UI_LABEL[] = "position";

  constexpr glm::vec3 COLOR_DEFAULT = glm::vec3(1.0f, 1.0f, 1.0f);
  constexpr glm::vec3 COLOR_UI_DEFAULT = glm::vec3(1.0f, 1.0f, 1.0f);
  constexpr const char COLOR_UI_LABEL[] = "color";
}

#endif /* LIGHT_CONSTANTS_HPP */
