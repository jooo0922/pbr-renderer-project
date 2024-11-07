#ifndef IBL_CONSTANTS_HPP
#define IBL_CONSTANTS_HPP

/**
 * IBL 관련 심볼릭 상수 정의
 *
 * 일반적으로 권장되는 심볼릭 상수 정의 방식은 아래와 같음.
 *
 * 1. 헤더 파일 안에 한 곳에 모아서
 * 2. 네임스페이스로 논리적 그룹을 묶어서
 * 3. constexpr 로 선언
 *
 * https://github.com/jooo0922/cpp-study/blob/main/TBCppStudy/Chapter2_9/MY_CONSTANTS.h 참고
 */
namespace IBLConstants
{

  constexpr bool IBL_VISIBILITY_DEFAULT = true;
  constexpr const char IBL_VISIBILITY_UI_LABEL[] = "apply IBL";

  constexpr bool SKYBOX_VISIBILITY_DEFAULT = true;
  constexpr const char SKYBOX_VISIBILITY_UI_LABEL[] = "show environment";

  constexpr float IBL_INTENSITY_DEFAULT = 1.0f;
  constexpr float IBL_INTENSITY_MIN = 0.0f;
  constexpr float IBL_INTENSITY_MAX = 1.0f;
  constexpr float IBL_INTENSITY_UI_SPEED = 0.001f;
  constexpr const char IBL_INTENSITY_UI_LABEL[] = "IBL intensity";

  constexpr int HDR_IMAGE_INDEX_DEFAULT = 0;
  constexpr const char HDR_IMAGE_SELECTOR_UI_LABEL[] = "select HDR Images";
}

#endif /* IBL_CONSTANTS_HPP */
