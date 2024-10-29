#include "features/model_feature.hpp"
#include <glm/gtc/matrix_transform.hpp> // 행렬 변환 관련 함수
#include <glm/gtc/quaternion.hpp>       // 쿼터니언 정의 및 관련 함수
#include <glm/gtx/quaternion.hpp>       // 쿼터니언에 대한 추가 함수

ModelFeature::ModelFeature()
    : pbrShaderPtr(nullptr),
      transform(glm::mat4(1.0f))
{
  /** Model 관련 정적 배열 컨테이너들 초기화 */
  for (size_t i = 0; i < ModelConstants::NUM_MODELS; i++)
  {
    /** 모델링 파일 url 초기화 */
    modelUrls[i] = ModelConstants::models[i].path;

    /** 모델링 파일을 로드하여 Model 객체 생성 */
    models[i] = std::make_unique<Model>(modelUrls[i]);
  }
}

void ModelFeature::initialize()
{
  // ModelUi 에서 관리되는 각 ImGui 요소에 입력할 초기값 설정
  modelParameter.position = ModelConstants::POSITION_DEFAULT;
  modelParameter.rotation = ModelConstants::ROTATION_DEFAULT;
  modelParameter.scale = ModelConstants::SCALE_DEFAULT;
  modelParameter.modelIndex = ModelConstants::MODEL_INDEX_DEFAULT;
}

void ModelFeature::process()
{
  pbrShaderPtr->use();

  // 모델 행렬을 단위 행렬로 초기화
  transform = glm::mat4(1.0f);

  // 위치 변환 적용
  transform = glm::translate(transform, position);

  // degree 단위의 Euler 각 인터페이스를 Quaternion 으로 변환 (게임수학 p.574 참고)
  glm::quat quaternion = glm::quat(glm::radians(rotation));

  // Quaternion 을 회전 행렬로 변환 (게임수학 p.578 참고)
  glm::mat4 rotationMatrix = glm::toMat4(quaternion);

  // 회전 변환 적용
  transform *= rotationMatrix;

  // 크기 변환 적용
  transform = glm::scale(transform, scale);

  // 계산된 모델 행렬을 쉐이더 프로그램에 전송
  pbrShaderPtr->setMat4("model", transform);

  /*
    쉐이더 코드에서 노멀벡터를 World Space 로 변환할 때
    사용할 노멀행렬을 각 구체의 계산된 모델행렬로부터 계산 후,
    쉐이더 코드에 전송
  */
  pbrShaderPtr->setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(transform))));

  // 선택된 Model 렌더링
  models[modelIndex]->draw(*pbrShaderPtr);
}

void ModelFeature::finalize()
{
  pbrShaderPtr = nullptr;
}

void ModelFeature::onChange(const ModelParameter &param)
{
  if (position != param.position)
  {
    setPosition(param.position);
  }

  if (rotation != param.rotation)
  {
    setRotation(param.rotation);
  }

  if (scale != param.scale)
  {
    setScale(param.scale);
  }

  if (modelIndex != param.modelIndex)
  {
    setModelIndex(param.modelIndex);
  }

  modelParameter = param;
}

void ModelFeature::setPbrShader(std::shared_ptr<Shader> pbrShader)
{
  pbrShaderPtr = pbrShader;
}

void ModelFeature::getModelParameter(ModelParameter &param) const
{
  param = modelParameter;
}

void ModelFeature::setPosition(const glm::vec3 &position)
{
  this->position = position;
}

void ModelFeature::setRotation(const glm::vec3 &rotation)
{
  this->rotation = rotation;
}

void ModelFeature::setScale(const glm::vec3 &scale)
{
  this->scale = scale;
}

void ModelFeature::setModelIndex(const size_t modelIndex)
{
  this->modelIndex = modelIndex;
}
