#ifndef MATERIAL_FEATURE_HPP
#define MATERIAL_FEATURE_HPP

/*
  #ifndef ~ #endif 전처리기는
  헤더파일의 헤더가드를 처리하여 중복 include 방지해 줌!
*/

#include <memory>
#include <glm/glm.hpp>
#include <features/feature.hpp>
#include <common/listener.hpp>
#include <shader/shader.hpp>

struct MaterialParameter
{
  float roughness;
  float metallic;
  float ambientOcclusion;
  glm::vec3 albedo;
};

/**
 * MaterialFeature 클래스
 *
 * Metallic-Roughness workflow 상에서
 * material 관련 파라미터들을 관리하는 Feature 클래스
 */
class MaterialFeature : public IFeature, public IListener<MaterialParameter>
{
public:
  MaterialFeature();

  void initialize() override;
  void process() override;
  void finalize() override;

  void onChange(const MaterialParameter &param) override;

  void setPbrShader(std::shared_ptr<Shader> pbrShader);

  void getMaterialParameter(MaterialParameter &param) const;

private:
  std::shared_ptr<Shader> pbrShaderPtr;
  float roughness;
  float metallic;
  float ambientOcclusion;
  glm::vec3 albedo;

  MaterialParameter materialParameter;

  // 파라미터 Setter 멤버 함수
  void setRoughness(const float roughness);
  void setMetallic(const float metallic);
  void setAmbientOcclusion(const float ambientOcclusion);
  void setAlbedo(const glm::vec3 &albedo);
};

#endif // MATERIAL_FEATURE_HPP
