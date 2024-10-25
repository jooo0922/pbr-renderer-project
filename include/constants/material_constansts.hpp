#ifndef MATERIAL_CONSTANSTS_HPP
#define MATERIAL_CONSTANSTS_HPP

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
  constexpr float ROUGHNESS_DEFAULT = 0.5f;
  constexpr float ROUGHNESS_MIN = 0.05f; // perfectly smooth surfaces(roughness of 0.0) 은 부자연스러워 보이므로, roughness 최솟값을 0.05 로 설정
  constexpr float ROUGHNESS_MAX = 1.0f;
  constexpr float ROUGHNESS_UI_SPEED = 0.001f;
  constexpr const char ROUGHNESS_UI_LABEL[] = "roughness";

  constexpr float METALLIC_DEFAULT = 0.0f;
  constexpr float METALLIC_MIN = 0.0f;
  constexpr float METALLIC_MAX = 1.0f;
  constexpr float METALLIC_UI_SPEED = 0.001f;
  constexpr const char METALLIC_UI_LABEL[] = "metallic";

  constexpr float AMBIENT_OCCLUSION_DEFAULT = 1.0f; // 각 프래그먼트의 ambient occlusion(환경광 차폐) 기본값을 1로 지정 -> 즉, 환경광이 차폐되는 영역이 없음!
  constexpr float AMBIENT_OCCLUSION_MIN = 0.0f;
  constexpr float AMBIENT_OCCLUSION_MAX = 1.0f;
  constexpr float AMBIENT_OCCLUSION_UI_SPEED = 0.001f;
  constexpr const char AMBIENT_OCCLUSION_UI_LABEL[] = "ambientOcclusion";

  constexpr glm::vec3 ALBEDO_DEFAULT = glm::vec3(0.5f, 0.0f, 0.0f);
  constexpr glm::vec3 ALBEDO_UI_DEFAULT = glm::vec3(1.0f, 1.0f, 1.0f);
  constexpr const char ALBEDO_UI_LABEL[] = "albedo";
}

#endif /* MATERIAL_CONSTANSTS_HPP */
