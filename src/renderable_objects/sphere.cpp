#include "renderable_objects/sphere.hpp"
#include <memory>

Sphere::Sphere()
{
  std::vector<SimpleVertexData> vertices;
  std::vector<unsigned int> indices;

  // 하위 클래스에서 구현된 vertex 데이터 생성
  generateVertexData(vertices, indices);

  // 생성된 데이터를 이용하여 Mesh 객체 인스턴스화 (mesh 멤버변수를 스마트 포인터로 관리하는 이유 하단 필기 참고)
  mesh = std::make_unique<Mesh<SimpleVertexData>>(vertices, indices);

  // Sphere 렌더링을 위한 추가 작업 수행
  mesh->setDrawMode(GL_TRIANGLE_STRIP);
}

void Sphere::draw(Shader &shader)
{
  mesh->draw(shader);
}

void Sphere::generateVertexData(std::vector<SimpleVertexData> &vertices, std::vector<unsigned int> &indices) const
{
  // 구체의 가로 및 세로 방향 분할 수, Pi 값 초기화
  const unsigned int X_SEGMENTS = 64;
  const unsigned int Y_SEGMENTS = 64;
  const float PI = 3.14159265359f;

  // 구체의 가로 및 세로 방향 분할 수 만큼 이중 for 문으로 반복 순회하며 position, normal, uv 값 계산
  for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
  {
    for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
    {
      /*
        구면 좌표계 -> 데카르트 좌표계 변환 공식을 사용하여 구체의 정점 position, normal, uv 계산
        https://ko.wikipedia.org/wiki/%EA%B5%AC%EB%A9%B4%EC%A2%8C%ED%91%9C%EA%B3%84 참고
      */
      // 구체의 가로 및 세로 방향의 현재 세그먼트를 정규화([0, 1] 범위로 맞춤)
      float xSegment = (float)x / (float)X_SEGMENTS;
      float ySegment = (float)y / (float)Y_SEGMENTS;

      // 구면 좌표계 -> 데카르트 좌표계(직교 좌표계) 변환
      /*
        참고로,
        xSegment * 2.0f * PI 는 구면 좌표계의 theta 각,
        ySegment * PI 는 구면 좌표계의 phi 각에 해당하고,
        반지름은 1로 보면 되겠지?
      */
      float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
      float yPos = std::cos(ySegment * PI);
      float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

      SimpleVertexData vertex;
      vertex.Position = glm::vec3(xPos, yPos, zPos);
      vertex.Normal = glm::vec3(xPos, yPos, zPos);
      vertex.TexCoords = glm::vec2(xSegment, ySegment);

      vertices.push_back(vertex);
    }
  }

  /*
    가로 방향 세그먼트들에서 짝수 또는 홀수 줄에 따라 정점의 index 값을 다른 방식으로 계산
  */
  // 현재 순회 중인 세그먼트가 홀수 줄 인지 여부를 나타내는 플래그 초기화
  bool oddRow = false;
  for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
  {
    if (!oddRow)
    {
      // 현재 세그먼트가 짝수 줄일 때 각 정점의 index 계산
      for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
      {
        indices.push_back(y * (X_SEGMENTS + 1) + x);
        indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
      }
    }
    else
    {
      // 현재 세그먼트가 홀수 줄일 때 각 정점의 index 계산
      for (int x = X_SEGMENTS; x >= 0; --x)
      {
        indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
        indices.push_back(y * (X_SEGMENTS + 1) + x);
      }
    }

    // 다음 순회의 세그먼트의 짝수 / 홀수 줄 여부를 계산
    oddRow = !oddRow;
  }
}

/**
 * Mesh 클래스 멤버변수를 스마트 포인터로 관리하는 이유
 *
 *
 * Sphere::Sphere(){...} 생성자 함수에서 스마트 포인터를 사용하지 않고,
 * mesh = Mesh(vertices, indices); 로 인스턴스화하면
 * Mesh::draw() > glDrawElements() 드로우 콜 호출 시 runtime error 가 발생함.
 *
 * 구체적으로 memory access violation 같은 메모리 접근 오류가 발생하는데,
 * 그 원인은 Sphere() 생성자에서 아래 과정과 같이 Mesh 인스턴스화하는 과정에서 발생함.
 *
 *
 * 1. Sphere::Sphere(){...} 생성자 함수 내에서 mesh = Mesh(vertices, indices); 로 Mesh 생성자 함수를 호출함
 *
 * 2. 이때, Mesh 클래스의 멤버변수인 OpenGL object 를 wrapping 한 클래스들인
 * VertexArrayObject, VertexBufferObject, IndexBufferObject 의 생성자들도 같이 호출됨.
 *
 * 3. 또한 이때, Mesh 클래스에는 대입 연산자 = 에 대한 연산자 오버로딩을
 * 별도로 구현하지 못했기 때문에, 기본 멤버 단위 대입 연산이 호출됨.
 *
 * 4. 인스턴스화된 Mesh 클래스는 현재 Sphere 생성자 함수의 block 내에서만 존재하는
 * 지역 변수, 즉, stack memory 영역에 저장된 데이터이므로, Sphere 생성자 함수 종료 시,
 * Mesh 클래스의 소멸자가 의도치 않게 호출되어 버림.
 *
 * 5. 이 소멸자는 OpenGL object 의 wrapping 클래스들인 VAO, VBO, IBO 클래스들의
 * 소멸자들도 같이 호출시켜 버림. 왜냐하면, 얘내들이 Mesh 클래스들의 멤버변수니까!
 *
 * 6. 결과적으로, 이 클래스들의 소멸자에는 glDeleteBuffers() 및 glDeleteVertexArrays() 를 호출하게
 * 구현되어 있으므로, OpenGL 컨텍스트에서는 해당 버퍼들이 이미 메모리 반납되어 버린 상태임.
 *
 * 7. 각 클래스들의 ID 멤버변수에 남아있는 버퍼의 참조 ID 값은 더 이상 유효하지 않으므로,
 * 이 ID 를 OpenGL 컨텍스트에 바인딩해서 glDrawElements() 드로우 콜을 호출하면 당연히 runtime error 가 발생했던 것!
 *
 *
 * 이처럼, OpenGL objects 들을 C++ 클래스로 wrapping 할 경우,
 * C++ 과 copy semantics, move semantics 에 대한 탄탄한 이해가 없으면
 * 의도치 않게 소멸자 호출을 발생시켜 문제를 일으키는 경우가 잦음.
 *
 * 이를 해결하기 위한 방법으로는 Mesh 클래스에 대입 연산자 오버로딩을 구현해서
 * Sphere() 생성자 block 이 종료되어도 Mesh::~Mesh() 소멸자가 호출되지 않도록 할 수도 있으나,
 *
 * 가장 단순한 해결 방법은 has-a 관계에 있는 Mesh 클래스를
 * std::unique_ptr 같은 스마트 포인터로 관리해서
 * Mesh 클래스에 대한 메모리 할당과 해제를 자동으로 관리해주는 게 편함!
 *
 *
 * https://stackoverflow.com/questions/31436436/opengl-glgenvertexarray-exception 참고
 */
