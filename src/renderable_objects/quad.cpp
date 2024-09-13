#include "renderable_objects/quad.hpp"
#include <memory>

Quad::Quad()
{
  std::vector<SimpleVertexData> vertices;
  std::vector<unsigned int> indices;

  // 하위 클래스에서 구현된 vertex 데이터 생성
  generateVertexData(vertices, indices);

  // 생성된 데이터를 이용하여 Mesh 객체 인스턴스화 (mesh 멤버변수를 스마트 포인터로 관리하는 이유 하단 필기 참고)
  mesh = std::make_unique<Mesh<SimpleVertexData>>(vertices, indices);
}

void Quad::draw(Shader &shader)
{
  mesh->draw(shader);
}

void Quad::generateVertexData(std::vector<SimpleVertexData> &vertices, std::vector<unsigned int> &indices) const
{
  // Quad 정점 데이터 추가
  vertices = {
      // positions          // normals        // texCoords
      {{-1.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
      {{-1.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
      {{1.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
      {{1.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
  };

  // Quad 인덱스 데이터 추가
  indices = {0, 1, 2, 0, 2, 3};
}
