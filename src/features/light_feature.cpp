#include "features/light_feature.hpp"

LightFeature::LightFeature()
    : pbrShaderPtr(nullptr)
{
}

void LightFeature::initialize()
{
  // LightUi 에서 관리되는 각 ImGui 요소에 입력할 초기값 설정

  // 광원 위치값이 담긴 정적 배열 초기화
  glm::vec3 lightPositions[] = {
      glm::vec3(-10.0f, 10.0f, 10.0f),
      glm::vec3(10.0f, 10.0f, 10.0f),
      glm::vec3(-10.0f, -10.0f, 10.0f),
      glm::vec3(10.0f, -10.0f, 10.0f),
  };

  // 광원 색상값이 담긴 정적 배열 초기화
  glm::vec3 lightColors[] = {
      glm::vec3(300.0f, 300.0f, 300.0f),
      glm::vec3(300.0f, 300.0f, 300.0f),
      glm::vec3(300.0f, 300.0f, 300.0f),
      glm::vec3(300.0f, 300.0f, 300.0f),
  };

  for (unsigned int i = 0; i < numLights; i++)
  {
    lightParameter.lightDataArray[i].position = lightPositions[i];
    lightParameter.lightDataArray[i].color = lightColors[i];
  }
}

void LightFeature::process()
{
  /* 광원 정보 쉐이더 전송 */

  // pbrShader 바인딩
  pbrShaderPtr->use();

  // 광원 데이터 개수만큼 for-loop 순회
  for (unsigned int i = 0; i < numLights; ++i)
  {
    // 광원 위치 및 색상 데이터를 쉐이더 프로그램에 전송
    pbrShaderPtr->setVec3("lightPositions[" + std::to_string(i) + "]", lights[i].getPosition());
    pbrShaderPtr->setVec3("lightColors[" + std::to_string(i) + "]", lights[i].getColor());
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
