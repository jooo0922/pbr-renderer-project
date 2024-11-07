#include "features/ibl_feature.hpp"
#include "constants/ibl_constants.hpp"

IBLFeature::IBLFeature()
    : pbrShaderPtr(nullptr),
      backgroundShaderPtr(nullptr),
      offscreenRenderingFeaturePtr(nullptr)
{
}

void IBLFeature::initialize()
{
  // IBLUi 에서 관리되는 각 ImGui 요소에 입력할 초기값 설정
  iblParameter.iblVisibility = IBLConstants::IBL_VISIBILITY_DEFAULT;
  iblParameter.skyboxVisibility = IBLConstants::SKYBOX_VISIBILITY_DEFAULT;
  iblParameter.iblIntensity = IBLConstants::IBL_INTENSITY_DEFAULT;
  iblParameter.hdrImageIndex = IBLConstants::HDR_IMAGE_INDEX_DEFAULT;
}

void IBLFeature::process()
{
  pbrShaderPtr->use();

  pbrShaderPtr->setBool("iblVisibility", iblVisibility);
  pbrShaderPtr->setFloat("iblIntensity", iblIntensity);

  // hdrImageIndex 에 따른 offscreen buffer 바인딩
  offscreenRenderingFeaturePtr->useIrradianceMap(hdrImageIndex);
  offscreenRenderingFeaturePtr->usePrefilterMap(hdrImageIndex);
  offscreenRenderingFeaturePtr->useBRDFLUTTexture();

  /** skybox 렌더링 */
  if (skyboxVisibility)
  {
    // skybox 쉐이더 프로그램 바인딩
    backgroundShaderPtr->use();

    // HDR 큐브맵 텍스쳐를 바인딩하여 skybox 텍스쳐로 사용
    offscreenRenderingFeaturePtr->useEnvCubemap(hdrImageIndex);

    // skybox 렌더링
    offscreenRenderingFeaturePtr->getCube().draw(*backgroundShaderPtr);
  }
}

void IBLFeature::finalize()
{
  pbrShaderPtr = nullptr;
  backgroundShaderPtr = nullptr;
  offscreenRenderingFeaturePtr = nullptr;
}

void IBLFeature::onChange(const IBLParameter &param)
{
  if (iblVisibility != param.iblVisibility)
  {
    setIBLVisibility(param.iblVisibility);
  }

  if (skyboxVisibility != param.skyboxVisibility)
  {
    setSkyboxVisibility(param.skyboxVisibility);
  }

  if (iblIntensity != param.iblIntensity)
  {
    setIBLIntensity(param.iblIntensity);
  }

  if (hdrImageIndex != param.hdrImageIndex)
  {
    setHDRImageIndex(param.hdrImageIndex);
  }

  iblParameter = param;
}

void IBLFeature::setPbrShader(std::shared_ptr<Shader> pbrShader)
{
  pbrShaderPtr = pbrShader;
}

void IBLFeature::setBackgroundShader(std::shared_ptr<Shader> backgroundShader)
{
  backgroundShaderPtr = backgroundShader;
}

void IBLFeature::setOffscreenRenderingFeature(OffscreenRenderingFeature *offscreenRenderingFeature)
{
  offscreenRenderingFeaturePtr = offscreenRenderingFeature;
}

void IBLFeature::getIBLParameter(IBLParameter &param) const
{
  param = iblParameter;
}

void IBLFeature::setIBLVisibility(const bool iblVisibility)
{
  this->iblVisibility = iblVisibility;
}

void IBLFeature::setSkyboxVisibility(const bool skyboxVisibility)
{
  this->skyboxVisibility = skyboxVisibility;
}

void IBLFeature::setIBLIntensity(const float iblIntensity)
{
  this->iblIntensity = iblIntensity;
}

void IBLFeature::setHDRImageIndex(const int hdrImageIndex)
{
  this->hdrImageIndex = hdrImageIndex;
}
