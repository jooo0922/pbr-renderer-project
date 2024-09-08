#include "renderable_objects/cube.hpp"
#include <memory>

Cube::Cube()
{
  std::vector<SimpleVertex> vertices;
  std::vector<unsigned int> indices;

  // 하위 클래스에서 구현된 vertex 데이터 생성
  generateVertexData(vertices, indices);

  // 생성된 데이터를 이용하여 Mesh 객체 인스턴스화 (mesh 멤버변수를 스마트 포인터로 관리하는 이유 하단 필기 참고)
  mesh = std::make_unique<Mesh<SimpleVertex>>(vertices, indices);
}

void Cube::draw(Shader &shader)
{
  mesh->draw(shader);
}

void Cube::generateVertexData(std::vector<SimpleVertex> &vertices, std::vector<unsigned int> &indices) const
{
  // Cube 정점 데이터 추가
  vertices = {
      // positions          // normals         // texCoords
      {{-1.0f, -1.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}}, // Back face
      {{1.0f, -1.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}},
      {{1.0f, 1.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}},
      {{-1.0f, 1.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}},

      {{-1.0f, -1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}}, // Front face
      {{1.0f, -1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
      {{1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
      {{-1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},

      {{-1.0f, 1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}}, // Left face
      {{-1.0f, 1.0f, -1.0f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
      {{-1.0f, -1.0f, -1.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
      {{-1.0f, -1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},

      {{1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}}, // Right face
      {{1.0f, 1.0f, -1.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
      {{1.0f, -1.0f, -1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
      {{1.0f, -1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},

      {{-1.0f, -1.0f, -1.0f}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}}, // Bottom face
      {{1.0f, -1.0f, -1.0f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f}},
      {{1.0f, -1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}},
      {{-1.0f, -1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}},

      {{-1.0f, 1.0f, -1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}}, // Top face
      {{1.0f, 1.0f, -1.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
      {{1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
      {{-1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
  };

  // Cube 인덱스 데이터 추가
  indices = {
      0, 1, 2, 2, 3, 0,       // Back face
      4, 5, 6, 6, 7, 4,       // Front face
      8, 9, 10, 10, 11, 8,    // Left face
      12, 13, 14, 14, 15, 12, // Right face
      16, 17, 18, 18, 19, 16, // Bottom face
      20, 21, 22, 22, 23, 20  // Top face
  };
}
