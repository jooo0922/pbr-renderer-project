#include "model/model.hpp"

// glm 라이브러리 사용을 위한 헤더파일 포함
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

Model::Model(const std::string &path)
{
  // 생성자에서 Assimp 로 모델 로드하는 함수 곧바로 호출
  loadModel(path);
}

void Model::draw(Shader &shader)
{
  for (unsigned int i = 0; i < meshes.size(); i++)
  {
    meshes[i]->draw(shader);
  }
}

void Model::loadModel(const std::string &path)
{
  // Assimp 로 Scene 노드 불러오기 (Assimp 모델 구조 참고)
  Assimp::Importer importer;

  // 비트플래그 연산을 통해, 3D 모델을 Scene 구조로 불러올 때의 여러 가지 옵션들을 지정함
  // 비트플래그 및 비트마스킹 연산 관련 https://github.com/jooo0922/cpp-study/blob/main/TBCppStudy/Chapter3_9/Chapter3_9.cpp 참고
  const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
  {
    /*
      Scene 구조가 다음 케이스 중 하나로 인해 불완전하게 로드되었을 때 에러를 출력함.

      1. Scene 구조의 포인터가 NULL 일 때
      2. AI_SCENE_FLAGS_INCOMPLETE 와의 비트마스킹 연산을 통해, 모델이 불완전하게 불러온 것이 확인되었을 때
      3. Scene 구조의 RootNode 가 존재하지 않을 때
    */
    std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
    return;
  }

  // 3D 모델 파일이 존재하는 디렉토리 경로를 멤버변수에 저장
  // 참고로, std::string.find_last_of('/')는 string 으로 저장된 문자열 상에서 마지막 '/' 문자가 저장된 위치를 반환함.
  // std::string.substr() 는 string 에서 지정된 시작 위치와 마지막 위치 사이의 부분 문자열을 반환함.
  directory = path.substr(0, path.find_last_of('/'));

  // Assimp Scene 구조를 따라 재귀적으로 하위 aiNode 들을 처리함
  processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode *node, const aiScene *scene)
{
  // 현재 aiNode 에 포함된 aiMesh 개수만큼 반복문을 돌림
  for (unsigned int i = 0; i < node->mNumMeshes; i++)
  {
    // aiNode->mMeshes 에는 각 aiMesh 가 저장된 인덱스만 들어있는 배열이고,
    // aiScene.mMeshes 에 실제 각 aiMesh 의 주소값들이 저장되어 있으므로, 이 배열에서 aiMesh 의 주소값을 얻어옴.
    aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];

    // aiMesh 를 파싱하여 우리가 만든 Mesh 클래스 인스턴스로 반환받고(processMesh()), 동적배열 멤버에 추가
    processMesh(mesh, scene);
  }

  // 현재 aiNode 의 mChildren 멤버에 저장된 자식노드들을 재귀적으로 순회해서 처리함
  for (unsigned int i = 0; i < node->mNumChildren; i++)
  {
    processNode(node->mChildren[i], scene);
  }
}

void Model::processMesh(aiMesh *mesh, const aiScene *scene)
{
  // Mesh 클래스 인스턴스 생성 시, 각 멤버에 채워넣을 동적배열 데이터 선언
  std::vector<VertexData> vertices;
  std::vector<unsigned int> indices;
  std::vector<TextureData> textures;

  // aiMesh 에 포함된 버텍스 개수만큼 반복문 순회
  for (unsigned int i = 0; i < mesh->mNumVertices; i++)
  {
    /* aiMesh 에 저장된 버텍스 데이터를 Vertex 구조체로 파싱 */
    VertexData vertex;

    /* position 데이터 파싱 */
    // Assimp 는 자체적으로 vector3 타입을 갖고있어, 호환성을 위해 glm::vec3 로 타입을 변환해서 파싱해줘야 함.
    glm::vec3 vector;
    vector.x = mesh->mVertices[i].x;
    vector.y = mesh->mVertices[i].y;
    vector.z = mesh->mVertices[i].z;
    vertex.Position = vector;

    /* normal 데이터 존재 여부 검사 및 파싱 */
    if (mesh->HasNormals())
    {
      vector.x = mesh->mNormals[i].x;
      vector.y = mesh->mNormals[i].y;
      vector.z = mesh->mNormals[i].z;
      vertex.Normal = vector;
    }

    /* uv 데이터 존재 여부 검사 및 파싱 */
    // 참고로, Assimp 는 최대 8개까지의 uv 데이터셋을 가질 수 있어, aiMesh->mTextureCoords 멤버가 2차원 배열로 구현되어 있음.
    // 그러나, 우리는 첫 번째 uv 데이터셋만 사용할 예정이므로, aiMesh->mTextureCoords[0] 번째 데이터셋만 가지고 파싱함.
    if (mesh->mTextureCoords[0])
    {
      glm::vec2 vec;
      vec.x = mesh->mTextureCoords[0][i].x;
      vec.y = mesh->mTextureCoords[0][i].y;
      vertex.TexCoords = vec;

      /* tangent 데이터 파싱 */
      vector.x = mesh->mTangents[i].x;
      vector.y = mesh->mTangents[i].y;
      vector.z = mesh->mTangents[i].z;
      vertex.Tangent = vector;

      /* bitangent 데이터 파싱 */
      vector.x = mesh->mBitangents[i].x;
      vector.y = mesh->mBitangents[i].y;
      vector.z = mesh->mBitangents[i].z;
      vertex.Bitangent = vector;
    }
    else
    {
      vertex.TexCoords = glm::vec2(0.0f, 0.0f);
    }

    // vertices 동적 배열에 파싱한 Vertex 구조체 추가
    vertices.push_back(vertex);
  }

  /* aiMesh 에 저장된 face 데이터를 indices 배열로 파싱 */
  // aiMesh 의 face 개수만큼 반복 순회
  for (unsigned int i = 0; i < mesh->mNumFaces; i++)
  {
    // face 데이터를 가져옴
    aiFace face = mesh->mFaces[i];

    // 삼각형 face(aiProcess_Triangulate 옵션에 의해...)를 구성하는 정점 인덱스 정보(aiFace.mIndices)가
    // aiFace 에 들어있으므로, 각 aiFace 의 정점 인덱스들을 indices 동적 배열에 순서대로 추가함
    for (unsigned int j = 0; j < face.mNumIndices; j++)
    {
      indices.push_back(face.mIndices[j]);
    }
  }

  // 현재 aiMesh 에서 사용할 aiMaterial 데이터 가져오기
  // aiMaterial 또한 인덱스 값만 aiMesh 에 저장되어 있고, 실제 주소값은 aiScene 이 갖고 있음
  aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

  /* aiMaterial 에 저장된 텍스쳐 경로를 로드하여 Texture 구조체로 파싱 */
  // uniform sampler 변수명을 '텍스쳐 타입 + 텍스쳐 번호' 형태의 convention 으로 선언할 것이므로,
  // 동일한 텍스쳐 타입끼리 Texture 구조체 동적 배열을 생성하여 이어붙일 것임 (std::vector.insert() 사용)
  // 1. diffuse maps
  std::vector<TextureData> diffuseMap = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffsue");
  textures.insert(textures.end(), diffuseMap.begin(), diffuseMap.end()); // textures 동적 배열 마지막에 diffuseMap 동적 배열 삽입(이어붙이기)

  // 2. specular maps
  std::vector<TextureData> specularMap = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
  textures.insert(textures.end(), specularMap.begin(), specularMap.end()); // textures 동적 배열 마지막에 specularMap 동적 배열 삽입(이어붙이기)

  // 3. normal maps
  std::vector<TextureData> normalMap = loadMaterialTextures(material, aiTextureType_NORMALS, "texture_normal");
  textures.insert(textures.end(), normalMap.begin(), normalMap.end()); // textures 동적 배열 마지막에 normalMap 동적 배열 삽입(이어붙이기)

  // 4. height maps
  std::vector<TextureData> heightMap = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_height");
  textures.insert(textures.end(), heightMap.begin(), heightMap.end()); // textures 동적 배열 마지막에 heightMap 동적 배열 삽입(이어붙이기)

  // Mesh 객체를 스마트 포인터로 생성 후 컨테이너에 주소값을 추가하여 의도치 않은 Mesh::~Mesh() 소멸자 호출 방지
  meshes.push_back(std::make_shared<Mesh<VertexData>>(mesh->mName.C_Str(), vertices, indices, textures));
}

std::vector<TextureData> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName)
{
  // 특정 타입의 Texture 구조체를 모아둘 동적 배열 선언
  std::vector<TextureData> textures;

  // aiMaterial 에 저장된 특정 타입의 텍스쳐 개수만큼 반복문 순회
  for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
  {
    aiString str;                   // 텍스쳐 파일 경로를 저장할 Assimp 자체 문자열 타입 변수 선언
    mat->GetTexture(type, i, &str); // aiMaterial 에 저장된 특정 타입의 i 번째 텍스쳐 파일 경로를 str 에 저장함

    // 지금 생성하려는 Texture 구조체가 이전에 이미 생성되었는지 검사
    bool skip = false;
    for (unsigned int j = 0; j < textures_loaded.size(); j++)
    {
      // std::strcmp() 함수로 텍스쳐 파일 경로 문자열이 동일한 지 비교 (두 문자열이 동일하면 0을 반환함.)
      // std::string.data() 는 std::string 타입의 문자열을 char* 타입의 c-style 문자열로 변환해주는 역할
      if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
      {
        // 만약, 이미 생성된 Texture 구조체가 존재한다면, 해당 구조체를 가져와서 textures 배열에 저장
        textures.push_back(textures_loaded[j]);

        // 새로운 Texture 구조체 파싱 여부를 검사하는 플래그를 true 로 설정하고 반복문 중단
        skip = true;
        break;
      }
    }

    // Texture 구조체 파싱
    if (!skip)
    {
      TextureData texture;
      texture.texturePtr = std::make_shared<Texture>(str.C_Str()); // 텍스쳐 객체 생성
      texture.id = texture.texturePtr->getID();                    // 텍스쳐 객체로부터 참조 ID 반환받아 저장
      texture.type = typeName;                                     // 텍스쳐 타입 이름 저장
      texture.path = str.C_Str();                                  // 텍스쳐 파일 경로를 c-style 문자열로 변환 후 저장 (중복 생성된 텍스쳐가 있는지 파일 경로로 검사하기 위해 추가)
      textures.push_back(texture);                                 // textures 동적 배열에 파싱한 Texture 구조체 추가
      textures_loaded.push_back(texture);                          // Texture 구조체 중복 생성 방지를 위해, 이미 로드된 텍스쳐를 저장하는 동적 배열에도 추가
    }
  }

  // 특정 타입의 Textures 구조체 동적 배열 반환
  return textures;
}
