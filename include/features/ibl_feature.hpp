#ifndef IBL_FEATURE_HPP
#define IBL_FEATURE_HPP

#include <memory>
#include <features/feature.hpp>
#include <common/listener.hpp>
#include <shader/shader.hpp>
#include <features/offscreen_rendering_feature.hpp>

struct IBLParameter
{
  bool iblVisibility;
  bool skyboxVisibility;
  float iblIntensity;
  int hdrImageIndex;
};

/**
 * IBLFeature 클래스
 *
 * IBL 관련 파라미터들을 관리하는 Feature 클래스
 */
class IBLFeature : public IFeature, public IListener<IBLParameter>
{
public:
  IBLFeature();

  void initialize() override;
  void process() override;
  void finalize() override;

  void onChange(const IBLParameter &param) override;

  void setPbrShader(std::shared_ptr<Shader> pbrShader);
  void setBackgroundShader(std::shared_ptr<Shader> backgroundShader);
  void setOffscreenRenderingFeature(OffscreenRenderingFeature *offscreenRenderingFeature);

  void getIBLParameter(IBLParameter &param) const;

private:
  std::shared_ptr<Shader> pbrShaderPtr;
  std::shared_ptr<Shader> backgroundShaderPtr;
  OffscreenRenderingFeature *offscreenRenderingFeaturePtr;

  bool iblVisibility;
  bool skyboxVisibility;
  float iblIntensity;
  int hdrImageIndex;

  IBLParameter iblParameter;

  // 파라미터 Setter 멤버 함수
  void setIBLVisibility(const bool iblVisibility);
  void setSkyboxVisibility(const bool skyboxVisibility);
  void setIBLIntensity(const float iblIntensity);
  void setHDRImageIndex(const int hdrImageIndex);
};

#endif /* IBL_FEATURE_HPP */
