#ifndef OFFSCREEN_RENDERING_CONSTANTS_HPP
#define OFFSCREEN_RENDERING_CONSTANTS_HPP

/*
  #ifndef ~ #endif 전처리기는
  헤더파일의 헤더가드를 처리하여 중복 include 방지해 줌!
*/

#include <array>
#include <glm/glm.hpp>

/**
 * OffscreenRendering 관련 심볼릭 상수 정의
 *
 * 일반적으로 권장되는 심볼릭 상수 정의 방식은 아래와 같음.
 *
 * 1. 헤더 파일 안에 한 곳에 모아서
 * 2. 네임스페이스로 논리적 그룹을 묶어서
 * 3. constexpr 로 선언
 *
 * https://github.com/jooo0922/cpp-study/blob/main/TBCppStudy/Chapter2_9/MY_CONSTANTS.h 참고
 */
namespace OffscreenRenderingConstants
{
  constexpr std::size_t NUM_CUBE_MAP_FACES = 6;
  constexpr std::size_t NUM_HDR_IMAGES = 2;

  struct HDRImage
  {
    const char *label;
    const char *path;
  };

  constexpr std::array<HDRImage, NUM_HDR_IMAGES> HDR_IMAGES = {{
      {"Newport Loft", "resources/textures/hdr/newport_loft.hdr"},
      {"Buikslotermeerplein", "resources/textures/hdr/buikslotermeerplein.hdr"},
  }};

  // pbrShader 관련 texture unit 상수 정의
  namespace PBRShader
  {
    constexpr int IRRADIANCE_MAP_UNIT = 0;
    constexpr int PREFILTER_MAP_UNIT = 1;
    constexpr int BRDF_LUT_UNIT = 2;
  };

  // backgroundShader 관련 texture unit 상수 정의
  namespace BackgroundShader
  {
    constexpr int ENVIRONMENT_MAP_UNIT = 3;
  };

  // equirectangularToCubemapShader 관련 texture unit 상수 정의
  namespace EquirectangularToCubemapShader
  {
    constexpr int HDR_TEXTURE_UNIT = 0;
  };

  // irradianceShader 관련 texture unit 상수 정의
  namespace IrradianceShader
  {
    constexpr int ENVIRONMENT_MAP_UNIT = 0;
  };

  // prefilterShader 관련 texture unit 상수 정의
  namespace PrefilterShader
  {
    constexpr int ENVIRONMENT_MAP_UNIT = 0;
  };

};

#endif // OFFSCREEN_RENDERING_CONSTANTS_HPP
