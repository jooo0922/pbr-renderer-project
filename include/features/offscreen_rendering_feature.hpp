#ifndef OFFSCREEN_RENDERING_FEATURE_HPP
#define OFFSCREEN_RENDERING_FEATURE_HPP

/*
  #ifndef ~ #endif 전처리기는
  헤더파일의 헤더가드를 처리하여 중복 include 방지해 줌!
*/

#include <memory>
#include <array>
#include <glm/glm.hpp>
#include <features/feature.hpp>
#include <shader/shader.hpp>
#include <gl_objects/frame_buffer_object.hpp>
#include <gl_objects/render_buffer_object.hpp>
#include <gl_objects/texture.hpp>
#include <gl_objects/cube_texture.hpp>
#include <renderable_objects/cube.hpp>
#include <renderable_objects/quad.hpp>
#include <constants/offscreen_rendering_constants.hpp>

/**
 * OffscreenRenderingFeature 클래스
 *
 * offscreen rendering 에 사용되는 텍스쳐 버퍼 및 프레임 버퍼들을 관리하는 Feature 클래스
 */
class OffscreenRenderingFeature : public IFeature
{
public:
  OffscreenRenderingFeature();

  void initialize() override;
  void process() override;
  void finalize() override;

  void setPbrShader(std::shared_ptr<Shader> pbrShader);
  void setBackgroundShader(std::shared_ptr<Shader> backgroundShader);

  // 각 텍스쳐 버퍼의 index 를 매개변수로 전달받아 사용할 offscreen rendering 버퍼를 바인딩하는 함수
  void useEnvCubemap(const size_t index);
  void useIrradianceMap(const size_t index);
  void usePrefilterMap(const size_t index);
  void useBRDFLUTTexture();

  // 각 primitive getter 함수들
  Cube &getCube();
  Quad &getQuad();

private:
  // offscreen rendering 결과가 저장된 텍스쳐들의 texture unit 위치값을 전송할 쉐이더 객체들
  std::shared_ptr<Shader> pbrShaderPtr;
  std::shared_ptr<Shader> backgroundShaderPtr;

  // offscreen rendering 시 렌더링할 primitive 객체들
  Cube cube;
  Quad quad;

  // offscreen rendering 시 바인딩할 FBO(FrameBufferObject) 및 RBO(RenderBufferObject) 객체
  FrameBufferObject captureFBO;
  RenderBufferObject captureRBO;

  // offscreen rendering 시 사용할 원본 .hdr 이미지 경로들
  std::array<const char *, OffscreenRenderingConstants::NUM_HDR_IMAGES> hdrImages;

  // offscreen rendering 결과를 저장할 텍스쳐 버퍼들
  std::array<std::unique_ptr<Texture>, OffscreenRenderingConstants::NUM_HDR_IMAGES> hdrTextures;
  std::array<std::unique_ptr<CubeTexture>, OffscreenRenderingConstants::NUM_HDR_IMAGES> envCubemaps;
  std::array<std::unique_ptr<CubeTexture>, OffscreenRenderingConstants::NUM_HDR_IMAGES> irradianceMaps;
  std::array<std::unique_ptr<CubeTexture>, OffscreenRenderingConstants::NUM_HDR_IMAGES> prefilterMaps;
  std::unique_ptr<Texture> brdfLUTTexture;

  // CubeTexture 버퍼에 offscreen rendering 시 단위 큐브 객체에 적용할 변환 행렬들
  glm::mat4 captureProjection;
  std::array<glm::mat4, OffscreenRenderingConstants::NUM_CUBE_MAP_FACES> captureViews;

  // 각 텍스쳐 버퍼에 offscreen rendering 하는 함수들
  void generateEnvCubemap();
  void generateIrradianceMap();
  void generatePrefilterMap();
  void generateBRDFLUTTexture();
};

#endif /* OFFSCREEN_RENDERING_FEATURE_HPP */
