#ifndef QUAD_HPP
#define QUAD_HPP

/*
  #ifndef ~ #endif 전처리기는
  헤더파일의 헤더가드를 처리하여 중복 include 방지해 줌!
*/

#include "renderable_objects/primitive.hpp"

class Quad final : public Primitive
{
public:
  // 생성자
  Quad();

  void draw(Shader &shader) override;

protected:
  void generateVertexData(std::vector<SimpleVertexData> &vertices, std::vector<unsigned int> &indices) const override;
};

#endif // QUAD_HPP
