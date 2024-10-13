#ifndef LIGHT_CONSTANTS_HPP
#define LIGHT_CONSTANTS_HPP

/*
  #ifndef ~ #endif 전처리기는
  헤더파일의 헤더가드를 처리하여 중복 include 방지해 줌!
*/

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
  constexpr float DEFAULT_INTENSITY = 300.f;
  constexpr glm::vec3 DEFAULT_COLOR = glm::vec3(1.0f, 1.0f, 1.0f);
}

#endif // LIGHT_CONSTANTS_HPP
