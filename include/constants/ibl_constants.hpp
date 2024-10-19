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
  constexpr bool DEFAULT_IBL_VISIBILITY = true;
  constexpr bool DEFAULT_SKYBOX_VISIBILITY = true;
  constexpr float DEFAULT_IBL_INTENSITY = 1.0f;
  constexpr size_t DEFAULT_HDR_IMAGE_INDEX = 0;
}

#endif /* IBL_CONSTANTS_HPP */
