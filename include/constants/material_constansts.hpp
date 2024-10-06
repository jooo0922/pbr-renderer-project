#ifndef MATERIAL_CONSTANTS_HPP
#define MATERIAL_CONSTANTS_HPP

/*
  #ifndef ~ #endif 전처리기는
  헤더파일의 헤더가드를 처리하여 중복 include 방지해 줌!
*/

#include <glm/glm.hpp>

/**
 * Material 관련 심볼릭 상수 정의
 *
 * 일반적으로 권장되는 심볼릭 상수 정의 방식은 아래와 같음.
 *
 * 1. 헤더 파일 안에 한 곳에 모아서
 * 2. 네임스페이스로 논리적 그룹을 묶어서
 * 3. constexpr 로 선언
 *
 * https://github.com/jooo0922/cpp-study/blob/main/TBCppStudy/Chapter2_9/MY_CONSTANTS.h 참고
 */
namespace MaterialConstants
{
  constexpr float DEFAULT_ROUGHNESS = 0.5f;
  constexpr float DEFAULT_METALLIC = 0.0f;
  constexpr float DEFAULT_AMBIENT_OCCLUSION = 1.0f; // 각 프래그먼트의 ambient occlusion(환경광 차폐) 기본값을 1로 지정 -> 즉, 환경광이 차폐되는 영역이 없음!
  constexpr glm::vec3 DEFAULT_ALBEDO = glm::vec3(0.5f, 0.0f, 0.0f);
}

#endif // MATERIAL_CONSTANTS_HPP
