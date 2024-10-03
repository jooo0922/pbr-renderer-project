#include "features/material_feature.hpp"

MaterialFeature::MaterialFeature()
    : pbrShaderPtr(nullptr)
{
}

void MaterialFeature::initialize()
{
  // MaterialUi 에서 관리되는 각 ImGui 요소에 입력할 초기값 설정
  // TODO : 추후에 초기값을 별도 파일에 constant 로 추출하여 정리할 것.
  materialParameter.roughness = 0.5f;
  materialParameter.metallic = 0.0f;
  materialParameter.ambientOcclusion = 1.0f; // 각 프래그먼트의 ambient occlusion(환경광 차폐) factor 를 1로 지정 -> 즉, 환경광이 차폐되는 영역이 없음!
  materialParameter.albedo = glm::vec3(0.5f, 0.0f, 0.0f);
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

  materialParameter = param;
}

void MaterialFeature::setPbrShader(std::shared_ptr<Shader> pbrShader)
{
  pbrShaderPtr = pbrShader;
}

void MaterialFeature::getMaterialParameter(MaterialParameter &param) const
{
  param = materialParameter;
}

void MaterialFeature::setRoughness(const float roughness)
{
  this->roughness = roughness;
}

void MaterialFeature::setMetallic(const float metallic)
{
  this->metallic = metallic;
}

void MaterialFeature::setAmbientOcclusion(const float ambientOcclusion)
{
  this->ambientOcclusion = ambientOcclusion;
}

void MaterialFeature::setAlbedo(const glm::vec3 &albedo)
{
  this->albedo = albedo;
}
