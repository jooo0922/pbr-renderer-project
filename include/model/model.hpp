#ifndef MODEL_HPP
#define MODEL_HPP

#include <string>
#include <vector>
#include <iostream>
#include <memory>

// 운영체제별 OpenGL 함수 포인터 포함
#include <glad/glad.h>

// 3D 모델 포맷 로딩을 위한 Assimp 라이브러리 포함
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "mesh/mesh.hpp"
#include "shader/shader.hpp"

class Model
{
public:
  // 생성자 함수 선언 및 구현
  Model(const std::string &path);

  // Model 클래스 내에 저장된 모든 Mesh 클래스 인스턴스의 Draw() 명령 호출 멤버 함수
  void draw(Shader &shader);

  // model data 관련 public 멤버 선언
  std::vector<TextureData> textures_loaded;              // 텍스쳐 객체 중복 생성 방지를 위해 이미 로드된 텍스쳐 구조체를 동적 배열에 저장해두는 멤버
  std::vector<std::shared_ptr<Mesh<VertexData>>> meshes; // Model 클래스에 사용되는 Mesh 클래스 인스턴스들을 동적 배열에 저장하는 멤버
  std::string directory;                                 // 3D 모델 파일이 위치하는 디렉토리 경로를 저장하는 멤버

private:
  void loadModel(const std::string &path);

  // Assimp Scene 구조에 따라 RootNode 부터 시작해서 재귀적으로 하위 aiNode 들을 처리하는 멤버 함수
  void processNode(aiNode *node, const aiScene *scene);

  // aiMesh 를 파싱하여 실제 Mesh 클래스 인스턴스로 반환해주는 멤버 함수
  void processMesh(aiMesh *mesh, const aiScene *scene);

  // aiMaterial 에 저장된 특정 타입의 텍스쳐들을 Texture 구조체 배열로 파싱하여 반환하는 멤버 함수
  std::vector<TextureData> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
};

#endif/* MODEL_HPP */
