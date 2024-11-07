#ifndef MODEL_CONSTANTS_HPP
#define MODEL_CONSTANTS_HPP

#include <array>
#include <glm/glm.hpp>

/**
 * Model 관련 심볼릭 상수 정의
 *
 * 일반적으로 권장되는 심볼릭 상수 정의 방식은 아래와 같음.
 *
 * 1. 헤더 파일 안에 한 곳에 모아서
 * 2. 네임스페이스로 논리적 그룹을 묶어서
 * 3. constexpr 로 선언
 *
 * https://github.com/jooo0922/cpp-study/blob/main/TBCppStudy/Chapter2_9/MY_CONSTANTS.h 참고
 */

namespace ModelConstants
{
  constexpr int NUM_MODELS = 3;

  struct Model
  {
    const char *label;
    const char *path;
  };

  constexpr std::array<Model, NUM_MODELS> models = {{
      {"Monkey", "resources/models/monkey/monkey.obj"},
      {"Dragon", "resources/models/dragon/dragon.obj"},
      {"Bunny", "resources/models/bunny/bunny.obj"},
  }};

  constexpr glm::vec3 POSITION_DEFAULT = glm::vec3(0.0f, 0.0f, 0.0f);
  constexpr float POSITION_MIN = -100.0f;
  constexpr float POSITION_MAX = 100.0f;
  constexpr float POSITION_UI_SPEED = 0.001f;
  constexpr const char POSITION_UI_LABEL[] = "position";

  // ROTATION 파라미터는 Euler 각을 기준으로 기본값, 최솟값, 최댓값 정의
  constexpr glm::vec3 ROTATION_DEFAULT = glm::vec3(0.0f, 0.0f, 0.0f);
  constexpr float ROTATION_MIN = -180.0f;
  constexpr float ROTATION_MAX = 180.0f;
  constexpr float ROTATION_UI_SPEED = 0.1f;
  constexpr const char ROTATION_UI_LABEL[] = "rotation";

  constexpr glm::vec3 SCALE_DEFAULT = glm::vec3(1.0f, 1.0f, 1.0f);
  constexpr float SCALE_MIN = 0.1f;
  constexpr float SCALE_MAX = 10.0f;
  constexpr float SCALE_UI_SPEED = 0.001f;
  constexpr const char SCALE_UI_LABEL[] = "scale";

  constexpr int MODEL_INDEX_DEFAULT = 0;
  constexpr const char MODEL_SELECTOR_UI_LABEL[] = "select Models";
}

#endif /* MODEL_CONSTANTS_HPP */
