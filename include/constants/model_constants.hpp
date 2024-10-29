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
  constexpr std::size_t NUM_MODELS = 1;

  struct Model
  {
    const char *label;
    const char *path;
  };

  constexpr std::array<Model, NUM_MODELS> models = {{
      {"Monkey", "resources/models/monkey/monkey.obj"},
  }};

  constexpr glm::vec3 POSITION_DEFAULT = glm::vec3(0.0f, 0.0f, 0.0f);

  // ROTATION 파라미터는 Euler 각을 기준으로 기본값, 최솟값, 최댓값 정의
  constexpr glm::vec3 ROTATION_DEFAULT = glm::vec3(0.0f, 0.0f, 0.0f);

  constexpr glm::vec3 SCALE_DEFAULT = glm::vec3(1.0f, 1.0f, 1.0f);

  constexpr size_t MODEL_INDEX_DEFAULT = 0;
}

#endif /* MODEL_CONSTANTS_HPP */
