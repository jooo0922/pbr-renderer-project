#ifndef MODEL_FEATURE_HPP
#define MODEL_FEATURE_HPP

#include <memory>
#include <glm/glm.hpp>
#include <features/feature.hpp>
#include <common/listener.hpp>
#include <shader/shader.hpp>
#include <model/model.hpp>
#include <constants/model_constants.hpp>

struct ModelParameter
{
  glm::vec3 position;
  glm::vec3 rotation; // rotation 파라미터는 Euler 각 기준 인터페이스 정의 -> 내부에서는 Quaternion 으로 계산
  glm::vec3 scale;
  int modelIndex;
};

/**
 * ModelFeature 클래스
 *
 * model 관련 파라미터들을 관리하는 Feature 클래스
 */
class ModelFeature : public IFeature, public IListener<ModelParameter>
{
public:
  ModelFeature();

  void initialize() override;
  void process() override;
  void finalize() override;

  void onChange(const ModelParameter &param) override;

  void setPbrShader(std::shared_ptr<Shader> pbrShader);

  void getModelParameter(ModelParameter &param) const;

private:
  std::shared_ptr<Shader> pbrShaderPtr;

  glm::vec3 position;
  glm::vec3 rotation;
  glm::vec3 scale;
  int modelIndex;

  ModelParameter modelParameter;

  // ImGui 로부터 입력받은 position, rotation, scale 값으로 계산할 모델 행렬
  glm::mat4 transform;

  // 모델링 파일 url 을 저장할 컨테이너
  std::array<const char *, ModelConstants::NUM_MODELS> modelUrls;

  // Model 인스턴스를 저장할 정적 배열 컨테이너
  std::array<std::unique_ptr<Model>, ModelConstants::NUM_MODELS> models;

  // 파라미터 Setter 멤버 함수
  void setPosition(const glm::vec3 &position);
  void setRotation(const glm::vec3 &rotation);
  void setScale(const glm::vec3 &scale);
  void setModelIndex(const int modelIndex);
};

#endif /* MODEL_FEATURE_HPP */
