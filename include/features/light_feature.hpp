#ifndef LIGHT_FEATURE_HPP
#define LIGHT_FEATURE_HPP

/*
  #ifndef ~ #endif 전처리기는
  헤더파일의 헤더가드를 처리하여 중복 include 방지해 줌!
*/

#include <memory>
#include <array>
#include <glm/glm.hpp>
#include <features/feature.hpp>
#include <common/listener.hpp>
#include <shader/shader.hpp>
#include <light/light.hpp>
#include <constants/light_constants.hpp>

struct LightData
{
  glm::vec3 position;
  glm::vec3 color;
  float intensity;
};

struct LightParameter
{
  std::array<LightData, LightConstants::NUM_LIGHTS> lightDataArray;
};

/**
 * LightFeature 클래스
 *
 * 직접광(= direct light) 관련 파라미터들을 관리하는 Feature 클래스
 */
class LightFeature : public IFeature, public IListener<LightParameter>
{
public:
  LightFeature();

  void initialize() override;
  void process() override;
  void finalize() override;

  void onChange(const LightParameter &param) override;

  void setPbrShader(std::shared_ptr<Shader> pbrShader);

  void getLightParameter(LightParameter &param) const;

private:
  std::array<Light, LightConstants::NUM_LIGHTS> lights;

  std::shared_ptr<Shader> pbrShaderPtr;

  LightParameter lightParameter;

  // 파라미터 Setter 멤버 함수
  void setPosition(Light &light, const glm::vec3 &position);
  void setColor(Light &light, const glm::vec3 &color);
  void setIntensity(Light &light, const float &intensity);
};

#endif /* LIGHT_FEATURE_HPP */
