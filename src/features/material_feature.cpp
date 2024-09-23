#include "features/material_feature.hpp"

MaterialFeature::MaterialFeature()
    : pbrShaderPtr(nullptr),
      roughness(0.5f),
      metallic(0.0f),
      ambientOcclusion(1.0f), // 각 프래그먼트의 ambient occlusion(환경광 차폐) factor 를 1로 지정 -> 즉, 환경광이 차폐되는 영역이 없음!
      albedo(glm::vec3(0.5f, 0.0f, 0.0f))
{
}

void MaterialFeature::initialize()
{
}

void MaterialFeature::process()
{
  pbrShaderPtr->use();

  pbrShaderPtr->setFloat("roughness", roughness);
  pbrShaderPtr->setFloat("metallic", metallic);
  pbrShaderPtr->setFloat("ao", ambientOcclusion);
  pbrShaderPtr->setVec3("albedo", albedo); // 표면 밖으로 빠져나온 diffuse light 색상값을 쉐이더 프로그램에 전송
}

void MaterialFeature::finalize()
{
  pbrShaderPtr = nullptr;
}

void MaterialFeature::onChange(const MaterialParameter &param)
{
  if (roughness != param.roughness)
  {
    setRoughness(param.roughness);
  }

  if (metallic != param.metallic)
  {
    setMetallic(param.metallic);
  }

  if (ambientOcclusion != param.ambientOcclusion)
  {
    setAmbientOcclusion(param.ambientOcclusion);
  }

  if (albedo != param.albedo)
  {
    setAlbedo(param.albedo);
  }
}

void MaterialFeature::setPbrShader(Shader *pbrShader)
{
  pbrShaderPtr = pbrShader;
}

void MaterialFeature::setRoughness(const float value)
{
  roughness = value;
}

void MaterialFeature::setMetallic(const float value)
{
  metallic = value;
}

void MaterialFeature::setAmbientOcclusion(const float value)
{
  ambientOcclusion = value;
}

void MaterialFeature::setAlbedo(const glm::vec3 &color)
{
  albedo = color;
}
