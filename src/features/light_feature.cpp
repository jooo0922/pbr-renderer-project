#include "features/light_feature.hpp"

LightFeature::LightFeature()
    : pbrShaderPtr(nullptr)
{
}

void LightFeature::initialize()
{
  // LightUi 에서 관리되는 각 ImGui 요소에 입력할 초기값 설정
  for (unsigned int i = 0; i < lightParameter.lightDataArray.size(); i++)
  {
    lightParameter.lightDataArray[i].position = LightConstants::lightUiDataArray[i].position;
    lightParameter.lightDataArray[i].color = LightConstants::COLOR_DEFAULT;
    lightParameter.lightDataArray[i].intensity = LightConstants::INTENSITY_DEFAULT;
  }
}

void LightFeature::process()
{
  /* 광원 정보 쉐이더 전송 */

  // pbrShader 바인딩
  pbrShaderPtr->use();

  // 광원 데이터 개수만큼 for-loop 순회
  for (unsigned int i = 0; i < lights.size(); ++i)
  {
    // 광원 위치 및 색상 데이터를 쉐이더 프로그램에 전송
    pbrShaderPtr->setVec3("lightPositions[" + std::to_string(i) + "]", lights[i].getPosition());
    pbrShaderPtr->setVec3("lightColors[" + std::to_string(i) + "]", lights[i].getColor() * lights[i].getIntensity());
  }
}

void LightFeature::finalize()
{
  pbrShaderPtr = nullptr;
}

void LightFeature::onChange(const LightParameter &param)
{
  unsigned int lightIndex = 0;
  for (const auto &lightData : param.lightDataArray)
  {
    Light &light = lights[lightIndex];

    if (light.getPosition() != lightData.position)
    {
      setPosition(light, lightData.position);
    }

    if (light.getColor() != lightData.color)
    {
      setColor(light, lightData.color);
    }

    if (light.getIntensity() != lightData.intensity)
    {
      setIntensity(light, lightData.intensity);
    }

    lightIndex += 1;
  }

  lightParameter = param;
}

void LightFeature::setPbrShader(std::shared_ptr<Shader> pbrShader)
{
  pbrShaderPtr = pbrShader;
}

void LightFeature::getLightParameter(LightParameter &param) const
{
  param = lightParameter;
}

void LightFeature::setPosition(Light &light, const glm::vec3 &position)
{
  light.setPosition(position);
}

void LightFeature::setColor(Light &light, const glm::vec3 &color)
{
  light.setColor(color);
}

void LightFeature::setIntensity(Light &light, const float &intensity)
{
  light.setIntensity(intensity);
}
