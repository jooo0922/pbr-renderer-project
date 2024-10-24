#ifndef VERTEX_ARRAY_OBJECT_HPP
#define VERTEX_ARRAY_OBJECT_HPP

/*
  #ifndef ~ #endif 전처리기는
  헤더파일의 헤더가드를 처리하여 중복 include 방지해 줌!
*/

#include <glad/glad.h> // OpenGL 함수를 초기화하기 위한 헤더
#include <gl_objects/gl_object.hpp>
#include <gl_objects/vertex_buffer_object.hpp>
#include <gl_objects/index_buffer_object.hpp>
#include <vector>
#include <tuple>

/**
 * VertexArrayObject 클래스
 *
 * VAO 객체를 추상화한 클래스
 */
class VertexArrayObject : public IGLObject
{
public:
  VertexArrayObject();

  ~VertexArrayObject();

  // interleaved buffer 구조를 고려하여 VBO 객체 연결 함수 구현
  void linkVBO(const VertexBufferObject &vbo, const std::vector<std::tuple<GLuint, GLint, GLenum, GLboolean, GLsizei, const void *>> &attributes);

  void linkIBO(const IndexBufferObject &ibo);

  GLuint getID() const;

  void bind() const override;

  void unbind() const override;

  void destroy() override;

private:
  GLuint ID;
};

#endif /* VERTEX_ARRAY_OBJECT_HPP */

/*
  VAO 는 왜 만드는걸까?

  VBO 객체를 생성 및 바인딩 후,
  해당 버퍼에 정점 데이터를 쓰고,
  버퍼에 쓰여진 데이터를 버텍스 쉐이더의 몇번 location 의 변수에서 사용할 지,
  해당 데이터를 몇 묶음으로 해석할 지 등의 해석 방식을 정의하고,
  해당 버퍼에 쓰여진 데이터를 사용하는 location 의 변수를 활성화하는 등의 작업은 이해가 가지?

  모두 GPU 메모리 상에 저장된 정점 버퍼의 데이터를
  버텍스 쉐이더가 어떻게 가져다 쓸 지 정의하기 위한 과정이지.

  그런데, 만약 서로 다른 오브젝트가 100개 존재하고,
  각 오브젝트에 5개의 vertex attribute 를 사용한다면?
  이런 식으로 VBO 를 구성하고 데이터 해석 방식을 설정하는 작업을
  그리기 명령이 발생할 때마다 500번씩 매번 해야된다는 소리...

  그런데, VAO 객체를 사용하면, 거기에다가
  VAO 안에 VBO 객체와 데이터 해석 방식, 해당 location 변수 활성화 여부 등의
  설정 상태를 모두 저장해두고 그리기 명령을 호출할 때마다
  필요한 VAO 객체를 교체하거나 꺼내쓸 수 있다.

  즉, 저런 번거로운 VBO 객체 생성 및 설정 작업을 반복하지 않아도 된다는 뜻!
*/
