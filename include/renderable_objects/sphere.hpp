#ifndef SPHERE_HPP
#define SPHERE_HPP

/*
  #ifndef ~ #endif 전처리기는
  헤더파일의 헤더가드를 처리하여 중복 include 방지해 줌!
*/

#include "renderable_objects/primitive.hpp"

class Sphere : public Primitive
{
public:
  // 생성자
  Sphere();

  void draw(Shader &shader) override;

protected:
  void generateVertexData(std::vector<SimpleVertexData> &vertices, std::vector<unsigned int> &indices) const override;
};

#endif // SPHERE_HPP
