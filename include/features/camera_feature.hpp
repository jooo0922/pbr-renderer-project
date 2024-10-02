#ifndef CAMERA_FEATURE_HPP
#define CAMERA_FEATURE_HPP

/*
  #ifndef ~ #endif 전처리기는
  헤더파일의 헤더가드를 처리하여 중복 include 방지해 줌!
*/

#include <memory>
#include <glm/glm.hpp>
#include <features/feature.hpp>
#include <common/listener.hpp>
#include <shader/shader.hpp>
#include <camera/camera.hpp>

struct CameraParameter
{
  float yaw;
  float pitch;
  float zoom;
  glm::vec3 position;
};

/**
 * CameraFeature 클래스
 *
 * camera 관련 파라미터들을 관리하는 Feature 클래스
 */
class CameraFeature : public IFeature, public IListener<CameraParameter>
{
public:
  CameraFeature();

  void initialize() override;
  void process() override;
  void finalize() override;

  void onChange(const CameraParameter &param) override;

  void setPbrShader(std::shared_ptr<Shader> pbrShader);
  void setBackgroundShader(std::shared_ptr<Shader> backgroundShader);

  void getCameraParameter(CameraParameter &param) const;

private:
  Camera camera;

  std::shared_ptr<Shader> pbrShaderPtr;
  std::shared_ptr<Shader> backgroundShaderPtr;

  CameraParameter cameraParameter;

  // 파라미터 Setter 멤버 함수
  void setYaw(const float value);
  void setPitch(const float value);
  void setZoom(const float value);
  void setPosition(const glm::vec3 &position);
};

#endif // CAMERA_FEATURE_HPP
