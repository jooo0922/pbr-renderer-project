#ifndef PRIMITIVE_HPP
#define PRIMITIVE_HPP

/*
  #ifndef ~ #endif 전처리기는
  헤더파일의 헤더가드를 처리하여 중복 include 방지해 줌!
*/

#include <memory>
#include "shader/shader.hpp"
#include "mesh/mesh.hpp"
#include "renderable_objects/renderable_object.hpp"

class Primitive : public IRenderableObject
{
protected:
  // Mesh 클래스 멤버변수를 스마트 포인터로 관리하는 이유 sphere.cpp 필기 참고
  std::unique_ptr<Mesh<SimpleVertexData>> mesh;

  virtual void generateVertexData(std::vector<SimpleVertexData> &vertices, std::vector<unsigned int> &indices) const = 0;
};

#endif // PRIMITIVE_HPP
