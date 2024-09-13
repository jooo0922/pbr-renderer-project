#ifndef MESH_HPP
#define MESH_HPP

/*
  #ifndef ~ #endif 전처리기는
  헤더파일의 헤더가드를 처리하여 중복 include 방지해 줌!
*/

#include <glm/glm.hpp>
#include "shader/shader.hpp"                   // Shader 클래스
#include "gl_objects/vertex_array_object.hpp"  // VAO 클래스
#include "gl_objects/vertex_buffer_object.hpp" // VBO 클래스
#include "gl_objects/index_buffer_object.hpp"  // IBO 클래스
#include <vector>
#include <tuple>
#include <type_traits> // std::is_same_v<> 사용을 위해 포함

#define MAX_BONE_INFLUENCE 4

// 메모리 낭비를 줄이기 위한 간소화된 Vertex 구조체 선언
struct SimpleVertexData
{
  glm::vec3 Position;
  glm::vec3 Normal;
  glm::vec2 TexCoords;
};

// 정점 구조체 선언
struct VertexData
{
  glm::vec3 Position;
  glm::vec3 Normal;
  glm::vec2 TexCoords;
  glm::vec3 Tangent;
  glm::vec3 Bitangent;
  int m_BoneIDs[MAX_BONE_INFLUENCE];
  float m_Weights[MAX_BONE_INFLUENCE];
};

// 텍스처 구조체 선언
struct TextureData
{
  unsigned int id;
  std::string type;
  std::string path;
};

/**
 * 템플릿으로 Mesh 클래스 선언
 *
 * 다양한 버전의 정점 구조체 타입을 템플릿 파라미터로 전달하기 위해
 * Mesh 클래스를 템플릿 클래스로 구현
 *
 * -> 템플릿 클래스는 헤더 파일에 선언부와 구현부가 모두 포함되어 있어야
 * 템플릿의 자료형을 찾지 못하는 컴파일 에러가 발생하지 않음!
 */
template <typename VertexDataType>
class Mesh
{
public:
  // 정점, 인덱스, 텍스처 데이터 멤버
  std::vector<VertexDataType> vertices;
  std::vector<unsigned int> indices;
  std::vector<TextureData> textures;

  // 기본 생성자
  Mesh() {}

  // 생성자 override
  Mesh(const std::vector<VertexDataType> &vertices, const std::vector<unsigned int> &indices, const std::vector<TextureData> &textures)
      : vertices(vertices), indices(indices), textures(textures)
  {
    setupMesh();
  }

  // 생성자 override (texture 데이터 미전달)
  Mesh(const std::vector<VertexDataType> &vertices, const std::vector<unsigned int> &indices)
      : vertices(vertices), indices(indices)
  {
    setupMesh();
  }

  void setDrawMode(GLenum mode)
  {
    drawMode = mode;
  }

  // 그리기 함수
  void draw(Shader &shader)
  {
    /* 각각의 텍스쳐들을 적절한 texture unit 위치에 바인딩 */
    // 각각의 동일한 타입의 텍스쳐들이 여러 개 사용될 수 있으므로, 텍스쳐 타입별로 구분짓기 위한 번호 counter
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    unsigned int normalNr = 1;
    unsigned int heightNr = 1;

    // 반복문을 순회하며 쉐이더에 선언된 sampler uniform 변수들의 이름을 파싱함
    for (unsigned int i = 0; i < textures.size(); i++)
    {
      // 현재 순회중인 텍스쳐 객체를 바인딩할 texture unit 활성화
      glActiveTexture(GL_TEXTURE0 + i);

      std::string number;                  // 현재 순회중인 텍스쳐의 번호를 문자열로 저장할 변수 (타입이 동일한 텍스쳐 간 구분 목적)
      std::string name = textures[i].type; // 현재 순회중인 텍스쳐의 타입을 문자열로 저장할 변수

      // 현재 순회중인 텍스쳐 타입과 순서에 따라 현재 텍스쳐의 번호를 저장한 뒤,
      // 타입별 텍스쳐 번호 counter 를 누산시킴.
      if (name == "texture_diffuse")
      {
        // 참고로, increment operator(++) 가 postfix 로 붙어있기 때문에,
        // 현재 ~Nr 변수에 들어있는 unsigned int -> string 으로 먼저 변환한 뒤,
        // ~Nr 변수의 값을 1 증가시키는 것. 즉, 문자열 변환 먼저, 그 값 증가!
        number = std::to_string(diffuseNr++);
      }
      else if (name == "texture_specular")
      {
        number = std::to_string(specularNr++);
      }
      else if (name == "texture_normal")
      {
        number = std::to_string(normalNr++);
      }
      else if (name == "texture_height")
      {
        number = std::to_string(heightNr++);
      }

      // '텍스쳐 타입 + 텍스쳐 번호' 로 파싱한 uniform sampler 변수명을 c-style 문자열로 변환한 뒤,
      // 쉐이더 프로그램에 해당 sampler 가 가져다 쓸 텍스쳐 객체가 바인딩되어 있는 texture unit 값 전달
      shader.setInt((name + number).c_str(), i);

      // 현재 활성화된 texture unit 위치에 현재 순회중인 텍스쳐 객체 바인딩
      glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }

    /* 실제 Mesh 그리기 명령 수행 */

    // VAO 객체를 바인딩하여 이에 저장된 설정대로 그리도록 명령
    vao.bind();

    // indexed drawing 명령 수행
    glDrawElements(drawMode, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);

    // 그리기 명령을 완료했으므로, 바인딩했던 VAO 객체 해제
    vao.unbind();

    // 그리기 명령 완료 후, 활성 texture unit 을 다시 기본값으로 초기화함.
    glActiveTexture(0);
  }

private:
  // VAO, VBO, IBO 객체
  VertexArrayObject vao;
  VertexBufferObject vbo;
  IndexBufferObject ibo;

  // draw mode
  GLenum drawMode = GL_TRIANGLES; // 기본값

  // 버퍼 설정 함수
  void setupMesh()
  {
    // VAO 바인딩
    vao.bind();

    // VBO에 데이터 설정
    vbo.setData(vertices.data(), vertices.size() * sizeof(VertexDataType), GL_STATIC_DRAW);

    // IBO에 데이터 설정
    ibo.setData(indices.data(), indices.size() * sizeof(unsigned int), GL_STATIC_DRAW);

    // 각 정점 데이터 해석 방식을 정의하는 데이터 쌍을 tuple 구조로 저장할 변수 초기화
    std::vector<std::tuple<GLuint, GLint, GLenum, GLboolean, GLsizei, const void *>> attributes;

    /**
     * 템플릿 파라미터인 VertexType 에 따라
     * 최종 컴파일 결과에 포함시킬 if block 을 결정하기 위해
     * if 문을 constexpr 로 정의함.
     *
     * 또한, std::is_same_v<T, U> 는 두 타입 T, U 가 같은지 여부를
     * 컴파일 타임에 체크하여 true 또는 false 를 value 라는 정적 멤버로 제공하는 템플릿 클래스
     *
     * std::is_same_v<T, U>는 위의 value 멤버에 대한 inline 변수를 제공하는 것으로,
     * std::is_same<T, U>::value의 값을 간편하게 사용할 수 있게 해줌.
     *
     * -> 템플릿 프로그래밍에서 자주 사용되는 고급 C++ 기법들
     */
    if constexpr (std::is_same_v<VertexDataType, VertexData>)
    {
      attributes = {
          {0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void *)offsetof(VertexData, Position)},
          {1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void *)offsetof(VertexData, Normal)},
          {2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void *)offsetof(VertexData, TexCoords)},
          {3, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void *)offsetof(VertexData, Tangent)},
          {4, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void *)offsetof(VertexData, Bitangent)},
          {5, 4, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void *)offsetof(VertexData, m_BoneIDs)},
          {6, 4, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void *)offsetof(VertexData, m_Weights)}};
    }
    else if constexpr (std::is_same_v<VertexDataType, SimpleVertexData>)
    {
      attributes = {
          {0, 3, GL_FLOAT, GL_FALSE, sizeof(SimpleVertexData), (void *)offsetof(SimpleVertexData, Position)},
          {1, 3, GL_FLOAT, GL_FALSE, sizeof(SimpleVertexData), (void *)offsetof(SimpleVertexData, Normal)},
          {2, 2, GL_FLOAT, GL_FALSE, sizeof(SimpleVertexData), (void *)offsetof(SimpleVertexData, TexCoords)}};
    }

    // VBO 및 IBO 객체를 VAO 객체에 연결
    vao.linkVBO(vbo, attributes);
    vao.linkIBO(ibo);

    // VAO, VBO, IBO 객체 바인딩 해제
    vao.unbind();
    vbo.unbind();
    ibo.unbind();
  }
};

#endif // MESH_HPP
