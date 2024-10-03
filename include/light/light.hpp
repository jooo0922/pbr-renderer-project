#ifndef LIGHT_HPP
#define LIGHT_HPP

/*
  #ifndef ~ #endif 전처리기는
  헤더파일의 헤더가드를 처리하여 중복 include 방지해 줌!
*/

#include <glm/glm.hpp>

/**
 * Light 클래스
 *
 * 직접광(= direct light)의 위치와 색상 데이터를 관리하는 클래스
 */
class Light
{
public:
  Light(const glm::vec3 &position = glm::vec3(0.0f, 0.0f, 0.0f), const glm::vec3 &color = glm::vec3(1.0f, 1.0f, 1.0f));

  // 직접광 position getter
  const glm::vec3 &getPosition() const;

  // 직접광 color getter
  const glm::vec3 &getColor() const;

  // 직접광 position setter
  void setPosition(const glm::vec3 &position);

  // 직접광 color setter
  void setColor(const glm::vec3 &color);

private:
  glm::vec3 position;
  glm::vec3 color;
};

#endif // LIGHT_HPP
