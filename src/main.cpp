#include <algorithm> // std::min(), std::max() 를 사용하기 위해 포함한 라이브러리

// 이미지 파일 로드 라이브러리 include (관련 설명 하단 참고)
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

// 행렬 및 벡터 계산에서 사용할 Header Only 라이브러리 include
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader/shader.hpp"
#include "camera/camera.hpp"
#include "glfw_impl/glfw_impl.hpp"

#include <iostream>
#include <spdlog/spdlog.h>

/* 콜백함수 전방선언 */

// 텍스쳐 이미지 로드 및 객체 생성 함수 선언 (텍스쳐 객체 참조 id 반환)
unsigned int loadTexture(const char *path);

// 구체 렌더링 함수 선언
void renderSphere();

// 큐브 렌더링 함수 선언
void renderCube();

// QuadMesh 렌더링 함수 선언
void renderQuad();

int main()
{
  // 앱 시작
  spdlog::info("Starting the application");

  // 카메라 클래스 생성 (카메라 위치값만 매개변수로 전달함.)
  Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

  // GLFWImpl 객체 생성
  GLFWImpl glfwImpl(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_NAME, &camera);

  // GLFWImpl 초기화
  if (glfwImpl.init() != 0)
  {
    spdlog::error("Failed to initialize GLFWImpl");
    return -1;
  }

  /* OpenGL 전역 상태값 설정 */

  // Depth Test(깊이 테스팅) 상태를 활성화함
  glEnable(GL_DEPTH_TEST);

  // 깊이 테스트 함수 변경 (skybox 렌더링 목적)
  glDepthFunc(GL_LEQUAL);

  // Cubemap 의 각 face 사이의 seam line 방지 활성화 (하단 필기 참고)
  glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

  /* PBR 구현에 필요한 쉐이더 객체 생성 및 컴파일 */

  // 구체 렌더링 시 적용할 PBR 쉐이더 객체 생성
  Shader pbrShader("resources/shaders/pbr.vs", "resources/shaders/pbr.fs");

  // 단위 큐브에 적용한 HDR 이미지를 Cubemap 버퍼에 렌더링하는 쉐이더 객체 생성
  Shader equirectangularToCubemapShader("resources/shaders/cubemap.vs", "resources/shaders/equirectangular_to_cubemap.fs");

  // HDR 큐브맵을 샘플링하여 계산한 diffuse term 적분식의 결과값(= irradiance)을 새로운 Cubemap 버퍼에 렌더링하는 쉐이더 객체 생성
  Shader irradianceShader("resources/shaders/cubemap.vs", "resources/shaders/irradiance_convolution.fs");

  /*
    HDR 큐브맵을 샘플링하여 계산한 split sum approximation 의 첫 번째 적분식의 결과값(= pre-filtered env map)을
    roughness level 에 따라 5단계의 mipmap 메모리 공간이 할당된 Cubemap 버퍼에 렌더링하는 쉐이더 객체 생성
  */
  Shader prefilterShader("resources/shaders/cubemap.vs", "resources/shaders/prefilter.fs");

  // split sum approximation 의 두 번째 적분식의 결과값(= BRDF Integration map)을 LUTTexture 버퍼에 렌더링하는 쉐이더 객체 생성
  Shader brdfShader("resources/shaders/brdf.vs", "resources/shaders/brdf.fs");

  // 배경에 적용할 skybox 를 렌더링하는 쉐이더 객체 생성
  Shader backgroundShader("resources/shaders/background.vs", "resources/shaders/background.fs");

  /* 각 구체에 공통으로 적용할 PBR Parameter 들을 쉐이더 프로그램에 전송 */

  // PBR 쉐이더 프로그램 바인딩
  pbrShader.use();

  // irradiance map 큐브맵 텍스쳐를 바인딩할 0번 texture unit 위치값 전송
  pbrShader.setInt("irradianceMap", 0);

  // pre-filtered env map 큐브맵 텍스쳐를 바인딩할 1번 texture unit 위치값 전송
  pbrShader.setInt("prefilterMap", 1);

  // BRDF Integration map 텍스쳐를 바인딩할 2번 texture unit 위치값 전송
  pbrShader.setInt("brdfLUT", 2);

  // 표면 밖으로 빠져나온 diffuse light 색상값을 쉐이더 프로그램에 전송
  pbrShader.setVec3("albedo", 0.5f, 0.0f, 0.0f);

  // 각 프래그먼트의 ambient occlusion(환경광 차폐) factor 를 1로 지정 -> 즉, 환경광이 차폐되는 영역이 없음!
  pbrShader.setFloat("ao", 1.0f);

  /* skybox 에 적용할 uniform 변수들을 쉐이더 프로그램에 전송 */

  // skybox 쉐이더 프로그램 바인딩
  backgroundShader.use();

  // HDR 이미지 데이터가 렌더링된 큐브맵 텍스쳐를 바인딩할 0번 texture unit 위치값 전송
  backgroundShader.setInt("environmentMap", 0);

  /* 광원 데이터 초기화 */

  // 광원 위치값이 담긴 정적 배열 초기화
  glm::vec3 lightPositions[] = {
      glm::vec3(-10.0f, 10.0f, 10.0f),
      glm::vec3(10.0f, 10.0f, 10.0f),
      glm::vec3(-10.0f, -10.0f, 10.0f),
      glm::vec3(10.0f, -10.0f, 10.0f),
  };

  // 광원 색상값이 담긴 정적 배열 초기화
  glm::vec3 lightColors[] = {
      glm::vec3(300.0f, 300.0f, 300.0f),
      glm::vec3(300.0f, 300.0f, 300.0f),
      glm::vec3(300.0f, 300.0f, 300.0f),
      glm::vec3(300.0f, 300.0f, 300.0f),
  };

  // 각 구체의 모델 행렬 계산 시 사용할 구체의 행 수, 열 수, 간격값 초기화
  int nrRows = 7;
  int nrColumns = 7;
  float spacing = 2.5;

  /* Equirectangular HDR 파일 > Cubemap 변환 시 필요한 버퍼 생성 및 바인딩 */

  // Equirectangular HDR 파일을 샘플링하여 렌더링할 FBO(FrameBufferObject) 객체 및 RBO(RenderBufferObject) 생성
  unsigned int captureFBO;
  unsigned int captureRBO;
  glGenFramebuffers(1, &captureFBO);
  glGenRenderbuffers(1, &captureRBO);

  // 생성한 FBO 객체 및 RBO 객체 바인딩
  glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
  glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);

  // RBO 객체 메모리 공간 할당 -> 단일 Renderbuffer 에 depth 값만 저장하는 데이터 포맷 지정(GL_DEPTH_COMPONENT24)
  // Renderbuffer 해상도를 Cubemap 각 면의 해상도인 512 * 512 로 맞춤.
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);

  // FBO 객체에 생성한 RBO 객체 attach
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

  /* stb_image 라이브러리로 .hdr 파일 로드하여 텍스쳐 객체 생성 */

  // 텍스쳐 이미지 로드 후, y축 방향으로 뒤집어 줌 > OpenGL 이 텍스쳐 좌표를 읽는 방향과 이미지의 픽셀 좌표가 반대라서!
  stbi_set_flip_vertically_on_load(true);

  // 로드한 .hdr 이미지의 width, height, 색상 채널 개수를 저장할 변수 선언
  int width, height, nrComponents;

  // 이미지 데이터 가져와서 float 타입의 bytes 데이터로 저장.
  // 이미지 width, height, 색상 채널 변수의 주소값도 넘겨줌으로써, 해당 함수 내부에서 값을 변경. -> 출력변수 역할
  float *data = stbi_loadf("resources/textures/hdr/newport_loft.hdr", &width, &height, &nrComponents, 0);

  // 텍스쳐 객체(object) 참조 id 를 저장할 변수 선언
  unsigned int hdrTexture;

  if (data)
  {
    // 텍스쳐 객체 생성 및 바인딩
    glGenTextures(1, &hdrTexture);
    glBindTexture(GL_TEXTURE_2D, hdrTexture);

    /*
      [0, 1] 범위를 넘어선 HDR 이미지 데이터(data)들을 온전히 저장하기 위해,
      GL_RGB16F floating point(부동 소수점) 포맷으로 프레임버퍼의 내부 색상 포맷 지정
      (하단 Floating point framebuffer 관련 필기 참고)
    */
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);

    // 현재 GL_TEXTURE_2D 상태에 바인딩된 텍스쳐 객체 설정하기
    // Texture Wrapping 모드를 반복 모드로 설정 ([(0, 0), (1, 1)] 범위를 벗어나는 텍스쳐 좌표에 대한 처리)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // 텍스쳐 축소/확대 및 Mipmap 교체 시 Texture Filtering (텍셀 필터링(보간)) 모드 설정
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // 텍스쳐 객체에 이미지 데이터를 전달하고, 밉맵까지 생성 완료했다면, 로드한 이미지 데이터는 항상 메모리 해제할 것!
    stbi_image_free(data);
  }
  else
  {
    // HDR 이미지 데이터 로드 실패 시 처리
    spdlog::error("Failed to load HDR image.");
  }

  /* HDR 이미지 텍스쳐를 렌더링할 color buffer 로써 Cubemap 텍스쳐 객체 생성 */

  // Cubemap 텍스쳐 생성 및 바인딩
  unsigned int envCubemap;
  glGenTextures(1, &envCubemap);
  glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

  // 반복문을 순회하며 Cubemap 각 6면에 이미지 데이터를 저장할 메모리 할당
  for (unsigned int i = 0; i < 6; i++)
  {
    /*
      HDR 이미지 텍스쳐 생성할 때와 마찬가지로, Cubemap 텍스쳐 또한
      [0, 1] 범위를 넘어선 HDR 이미지 데이터(data)들을 온전히 저장하기 위해,

      GL_RGB16F floating point(부동 소수점) 포맷으로 프레임버퍼의 내부 색상 포맷 지정
      (하단 Floating point framebuffer 관련 필기 참고)
    */
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 512, 512, 0, GL_RGB, GL_FLOAT, nullptr);
  }

  // 현재 GL_TEXTURE_2D 상태에 바인딩된 텍스쳐 객체 설정하기
  // Texture Wrapping 모드를 반복 모드로 설정 ([(0, 0), (1, 1)] 범위를 벗어나는 텍스쳐 좌표에 대한 처리)
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  // 텍스쳐 축소/확대 및 Mipmap 교체 시 Texture Filtering (텍셀 필터링(보간)) 모드 설정
  /*
    Bright dot artifact 해결을 위해 원본 HDR Cubemap 버퍼에서 mipmap 을 생성하므로,
    MIN_FILTER 모드를 GL_LINEAR_MIPMAP_LINEAR 로 지정해서
    LOD 에 따라 mipmap 사이의 trilinear interpolation 을 적용함. (노션 IBL 필기 참고)
  */
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  /* Cubemap 텍스쳐의 각 면에 HDR 이미지 데이터를 렌더링할 때 적용할 행렬값 초기화 */

  // 투영행렬 초기화 -> 투영행렬의 fov(시야각)은 반드시 90도로 설정 (관련 기법 하단 필기 참고)
  glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);

  // HDR 이미지가 입혀진 단위 큐브의 각 면을 바라보도록 계산되는 6개의 LookAt 행렬(= 뷰 행렬) 초기화
  // 유사한 기법을 이미 Point Shadow 챕터에서 사용했었음. https://github.com/jooo0922/opengl-study/blob/main/AdvancedLighting/Point_Shadows/point_shadows.cpp 참고
  glm::mat4 captureViews[] =
      {
          glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
          glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
          glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
          glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
          glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
          glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))};

  /* equirectangularToCubemapShader 에 텍스쳐 및 행렬 전달 */

  // equirectangularToCubemapShader 쉐이더 바인딩
  equirectangularToCubemapShader.use();

  // HDR 이미지 텍스쳐를 바인딩할 0번 texture unit 위치값 전송
  equirectangularToCubemapShader.setInt("equirectangularMap", 0);

  // fov(시야각)이 90로 고정된 투영행렬 전송
  equirectangularToCubemapShader.setMat4("projection", captureProjection);

  // HDR 이미지 텍스쳐를 바인딩할 0번 texture unit 활성화
  glActiveTexture(GL_TEXTURE0);

  // 0번 texture unit 에 HDR 이미지 텍스쳐 바인딩
  glBindTexture(GL_TEXTURE_2D, hdrTexture);

  /* 렌더링 루프 진입 이전에 Cubemap 버퍼에 HDR 이미지 렌더링 */

  // Cubemap 버퍼의 각 면의 해상도 512 * 512 에 맞춰 viewport 해상도 설정
  glViewport(0, 0, 512, 512);

  // Cubemap 버퍼의 각 면을 attach 할 FBO 객체 바인딩
  glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);

  // HDR 이미지가 적용된 단위 큐브의 각 면을 바라보도록 카메라를 회전시키며 6번 렌더링
  for (unsigned int i = 0; i < 6; i++)
  {
    // 쉐이더 객체에 단위 큐브의 각 면을 바라보도록 계산하는 뷰 행렬 전송
    equirectangularToCubemapShader.setMat4("view", captureViews[i]);

    // Cubemap 버퍼의 각 면을 현재 바인딩된 FBO 객체에 돌아가며 attach
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);

    // 단위 큐브를 attach 된 Cubemap 버퍼에 렌더링하기 전, 색상 버퍼와 깊이 버퍼를 깨끗하게 비워줌
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 단위 큐브 렌더링 -> Point Shadow 에서는 Cubemap 버퍼 각 면에 렌더링해주는 작업을 geometry shader 에서 처리해줬었지!
    renderCube();
  }

  // Cubemap 버퍼에 렌더링 완료 후, 기본 프레임버퍼로 바인딩 초기화
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  /* Bright dot artifact 해결을 위해 원본 HDR Cubemap 의 mipmap 생성 */

  // mipmap 을 생성할 원본 HDR Cubemap 바인딩
  glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

  // 현재 바인딩된 원본 HDR Cubemap 에 대해서 mipmap 메모리 공간 할당
  glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

  /* diffuse term 적분식의 결과값(= irradiance)를 렌더링할 color buffer 로써 Cubemap 텍스쳐 객체 생성 */

  // Cubemap 텍스쳐 생성 및 바인딩
  unsigned int irradianceMap;
  glGenTextures(1, &irradianceMap);
  glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);

  // 반복문을 순회하며 Cubemap 각 6면에 이미지 데이터를 저장할 메모리 할당
  for (unsigned int i = 0; i < 6; i++)
  {
    /*
      [0, 1] 범위를 넘어선 HDR 이미지 데이터(data)들을 온전히 저장하기 위해,
      GL_RGB16F floating point(부동 소수점) 포맷으로 프레임버퍼의 내부 색상 포맷 지정.

      또한, irradiance map 은 HDR 큐브맵을 convolution 하여 만든 결과물이 저장되므로,
      HDR 큐브맵을 흐릿하게 blur 처리한 것처럼 보임.

      -> 그렇다면, 어차피 흐릿해지는 irradiance cubemap 을 만들기 위해
      굳이 고해상도 큐브맵은 필요하지 않겠지.

      그래서, irradiance Cubemap 버퍼의 각 면의 해상도를 32 * 32 정도로 낮게 설정함.
    */
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
  }

  // 현재 GL_TEXTURE_2D 상태에 바인딩된 텍스쳐 객체 설정하기
  // Texture Wrapping 모드를 반복 모드로 설정 ([(0, 0), (1, 1)] 범위를 벗어나는 텍스쳐 좌표에 대한 처리)
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  // 텍스쳐 축소/확대 및 Mipmap 교체 시 Texture Filtering (텍셀 필터링(보간)) 모드 설정
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // irradiance map 을 렌더링할 때 사용할 FBO 객체 및 RBO 객체 바인딩
  glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
  glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);

  // RBO 객체 메모리 공간 할당 -> 단일 Renderbuffer 에 depth 값만 저장하는 데이터 포맷 지정(GL_DEPTH_COMPONENT24)
  // Renderbuffer 해상도를 Cubemap 각 면의 해상도인 32 * 32 로 맞춤.
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);

  /* irradianceShader 에 텍스쳐 및 행렬 전달 */

  // irradianceShader 쉐이더 바인딩
  irradianceShader.use();

  // HDR 큐브맵 텍스쳐를 바인딩할 0번 texture unit 위치값 전송
  irradianceShader.setInt("environmentMap", 0);

  // fov(시야각)이 90로 고정된 투영행렬 전송
  irradianceShader.setMat4("projection", captureProjection);

  // HDR 큐브맵 텍스쳐를 바인딩할 0번 texture unit 활성화
  glActiveTexture(GL_TEXTURE0);

  // 0번 texture unit 에 HDR 큐브맵 텍스쳐 바인딩
  glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

  /* 렌더링 루프 진입 이전에 Cubemap 버퍼에 irradiance map 렌더링 */

  // Cubemap 버퍼의 각 면의 해상도 32 * 32 에 맞춰 viewport 해상도 설정
  glViewport(0, 0, 32, 32);

  // Cubemap 버퍼의 각 면을 attach 할 FBO 객체 바인딩
  glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);

  // irradiance map 을 렌더링할 단위 큐브의 각 면을 바라보도록 카메라를 회전시키며 6번 렌더링
  for (unsigned int i = 0; i < 6; i++)
  {
    // 쉐이더 객체에 단위 큐브의 각 면을 바라보도록 계산하는 뷰 행렬 전송
    irradianceShader.setMat4("view", captureViews[i]);

    // Cubemap 버퍼의 각 면을 현재 바인딩된 FBO 객체에 돌아가며 attach
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0);

    // 단위 큐브를 attach 된 Cubemap 버퍼에 렌더링하기 전, 색상 버퍼와 깊이 버퍼를 깨끗하게 비워줌
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 단위 큐브 렌더링 -> irradianceShader 에서 적분식을 풀면서 각 프래그먼트 지점의 irradiance 를 Cubemap 버퍼에 저장함.
    renderCube();
  }

  // Cubemap 버퍼에 렌더링 완료 후, 기본 프레임버퍼로 바인딩 초기화
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  /*
    split-sum approximation(= specular term 적분식)에서
    첫 번째 적분식의 결과값(= pre-filtered environment map)를 렌더링할 color buffer 로써
    Cubemap 텍스쳐 객체 생성
  */

  // Cubemap 텍스쳐 생성 및 바인딩
  unsigned int prefilterMap;
  glGenTextures(1, &prefilterMap);
  glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);

  // 반복문을 순회하며 Cubemap 각 6면에 이미지 데이터를 저장할 메모리 할당
  for (unsigned int i = 0; i < 6; i++)
  {
    /*
      [0, 1] 범위를 넘어선 HDR 이미지 데이터(data)들을 온전히 저장하기 위해,
      GL_RGB16F floating point(부동 소수점) 포맷으로 프레임버퍼의 내부 색상 포맷 지정.

      또한, pre-filtered enviroment map 또한 irradiance map 과 유사하게
      HDR 큐브맵을 convolution 하여 만든 결과물이 저장되므로,
      HDR 큐브맵을 흐릿하게 blur 처리한 것처럼 보임.

      그래서 굳이 고해상도의 큐브맵은 필요하지 않지만,
      roughness level 에 따라 5단계의 mipmap 에 저장할 것이므로,
      가장 해상도가 낮은 mip level 이 irradiance map 의 해상도와 동일한 32 * 32 로 생성되도록
      base mip level 의 해상도는 그것의 4배 정도가 적당할 것임.

      그래서, pre-filtered enviroment map 버퍼의 각 면의 해상도를 128 * 128 정도로 낮게 설정함.
    */
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 128, 128, 0, GL_RGB, GL_FLOAT, nullptr);
  }

  // 현재 GL_TEXTURE_2D 상태에 바인딩된 텍스쳐 객체 설정하기
  // Texture Wrapping 모드를 반복 모드로 설정 ([(0, 0), (1, 1)] 범위를 벗어나는 텍스쳐 좌표에 대한 처리)
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  // 텍스쳐 축소/확대 및 Mipmap 교체 시 Texture Filtering (텍셀 필터링(보간)) 모드 설정
  // 텍스쳐 축소 시, trilinear filtering 기법 적용을 위해 GL_LINEAR_MIPMAP_LINEAR 모드로 설정 (노션 필기 참고)
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // 현재 바인딩된 Cubemap 에 대해서 roughness level 에 따른 pre-filtered envmap 을 저장할 mipmap 메모리 공간 할당
  glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

  /* prefilterShader 에 텍스쳐 및 행렬 전달 */

  // prefilterShader 쉐이더 바인딩
  prefilterShader.use();

  // HDR 큐브맵 텍스쳐를 바인딩할 0번 texture unit 위치값 전송
  prefilterShader.setInt("environmentMap", 0);

  // fov(시야각)이 90로 고정된 투영행렬 전송
  prefilterShader.setMat4("projection", captureProjection);

  // HDR 큐브맵 텍스쳐를 바인딩할 0번 texture unit 활성화
  glActiveTexture(GL_TEXTURE0);

  // 0번 texture unit 에 HDR 큐브맵 텍스쳐 바인딩
  glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

  /* 렌더링 루프 진입 이전에 Cubemap 버퍼에 각 mip level 마다 pre-filtered env map 렌더링 */

  // Cubemap 버퍼의 각 면을 attach 할 FBO 객체 바인딩
  glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);

  // 최대 mip level 변수 초기화
  unsigned int maxMipLevels = 5;

  // 각 mip level 을 순회하며 Cubemap 버퍼에 pre-filtered env map 렌더링
  for (unsigned int mip = 0; mip < maxMipLevels; mip++)
  {
    /*
      각 mip level 에 따라 128^(1 / 2^n) 형태로
      mipmap 의 최대 해상도 128 의 2^n 번째 거듭제곱근을 계산하여
      각 mip level 에서 사용할 프레임버퍼와 viewport 의 해상도를 결정함.
    */
    unsigned int mipWidth = 128 * std::pow(0.5, mip);
    unsigned int mipHeight = 128 * std::pow(0.5, mip);

    // pre-filtered env map 을 렌더링할 때 사용할 RBO 객체 바인딩
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);

    // RBO 객체 메모리 공간 할당 -> 단일 Renderbuffer 에 depth 값만 저장하는 데이터 포맷 지정(GL_DEPTH_COMPONENT24)
    // Renderbuffer 해상도를 각 mipmap 의 해상도로 맞춤.
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);

    // Cubemap 버퍼의 각 면의 해상도를 각 mipmap 의 해상도로 맞춰 viewport 해상도 설정
    glViewport(0, 0, mipWidth, mipHeight);

    /*
      각 mip level 에 따라 prefilterShader 쉐이더 객체에 전송할 [0.0, 1.0] 사이의 roughness 값 계산
      -> mip level 이 높을수록 mipmap 의 해상도가 줄어들기 때문에, roughness 값이 그만큼 커지도록 계산함.
    */
    float roughness = (float)mip / (float)(maxMipLevels - 1);
    prefilterShader.setFloat("roughness", roughness);

    // pre-filtered env map 을 렌더링할 단위 큐브의 각 면을 바라보도록 카메라를 회전시키며 6번 렌더링
    for (unsigned int i = 0; i < 6; i++)
    {
      // 쉐이더 객체에 단위 큐브의 각 면을 바라보도록 계산하는 뷰 행렬 전송
      prefilterShader.setMat4("view", captureViews[i]);

      // Cubemap 버퍼의 각 면을 현재 바인딩된 FBO 객체에 돌아가며 attach
      // glFramebufferTexture2D() 의 마지막 매개변수는 현재 바인딩된 프레임버퍼에 attach 할 Cubemap 의 mip level 을 전달함.
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap, mip);

      // 단위 큐브를 attach 된 Cubemap 버퍼에 렌더링하기 전, 색상 버퍼와 깊이 버퍼를 깨끗하게 비워줌
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      // 단위 큐브 렌더링 -> prefilterShader 에서 split sum approximation 의 첫 번째 적분식의 결과값을 풀어 Cubemap 버퍼에 저장함.
      renderCube();
    }
  }

  // Cubemap 버퍼에 렌더링 완료 후, 기본 프레임버퍼로 바인딩 초기화
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  /*
    split-sum approximation(= specular term 적분식)에서
    두 번째 적분식의 결과값(= BRDF Integration map)를 렌더링할 color buffer 로써 2D 텍스쳐 객체 생성
  */

  // 텍스쳐 객체(object) 참조 id 를 저장할 변수 선언
  unsigned int brdfLUTTexture;

  // 텍스쳐 객체 생성 및 바인딩
  glGenTextures(1, &brdfLUTTexture);
  glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);

  /*
    [0, 1] 범위를 넘어선 HDR 이미지 데이터(data)들을 온전히 저장하고,
    split-sum approximation 의 두 번째 적분식의 scale, bias 값만 r, g 채널에 각각 저장하기 위해
    GL_RG16F floating point(부동 소수점) 포맷으로 프레임버퍼의 내부 색상 포맷 지정
    (하단 Floating point framebuffer 관련 필기 참고)
  */
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);

  // 현재 GL_TEXTURE_2D 상태에 바인딩된 텍스쳐 객체 설정하기
  // Texture Wrapping 모드를 반복 모드로 설정 ([(0, 0), (1, 1)] 범위를 벗어나는 텍스쳐 좌표에 대한 처리)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  // 텍스쳐 축소/확대 및 Mipmap 교체 시 Texture Filtering (텍셀 필터링(보간)) 모드 설정
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  /* 렌더링 루프 진입 이전에 2D 텍스쳐 버퍼에 BRDF Integration map 렌더링 */

  // BRDF Integration map 버퍼를 attach 할 FBO 객체 바인딩
  glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);

  // BRDF Integration map 을 렌더링할 때 사용할 RBO 객체 바인딩
  glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);

  // RBO 객체 메모리 공간 할당 -> 단일 Renderbuffer 에 depth 값만 저장하는 데이터 포맷 지정(GL_DEPTH_COMPONENT24)
  // Renderbuffer 해상도를 BRDF Integration map 의 해상도로 맞춤.
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);

  // BRDF Integration map 을 현재 바인딩된 FBO 객체에 attach
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUTTexture, 0);

  // BRDF Integration map 의 해상도에 맞춰 viewport 해상도 설정
  glViewport(0, 0, 512, 512);

  // brdfShader 쉐이더 바인딩
  brdfShader.use();

  // attach 된 BRDF Integration map 버퍼에 렌더링하기 전, 색상 버퍼와 깊이 버퍼를 깨끗하게 비워줌
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // 단일 QuadMesh 렌더링 -> brdfShader 에서 split sum approximation 의 두 번째 적분식의 결과값을 풀어 BRDF Integration map 버퍼에 저장함.
  renderQuad();

  // BRDF Integration map 버퍼에 렌더링 완료 후, 기본 프레임버퍼로 바인딩 초기화
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  /*
    투영행렬을 렌더링 루프 이전에 미리 계산
    -> why? camera zoom-in/out 미적용 시, 투영행렬 재계산 불필요!
  */

  // 카메라의 zoom 값으로부터 투영 행렬 계산
  glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);

  // 변환행렬을 전송할 PBR 쉐이더 프로그램 바인딩
  pbrShader.use();

  // 계산된 투영행렬을 쉐이더 프로그램에 전송
  pbrShader.setMat4("projection", projection);

  // 변환행렬을 전송할 skybox 쉐이더 프로그램 바인딩
  backgroundShader.use();

  // 계산된 투영행렬을 쉐이더 프로그램에 전송
  backgroundShader.setMat4("projection", projection);

  glfwImpl.restoreViewport();

  // while 문으로 렌더링 루프 구현
  while (!glfwImpl.shouldClose())
  {
    // 렌더링 루프에서의 glfw 처리
    glfwImpl.process();

    // 현재까지 저장되어 있는 프레임 버퍼(그 중에서도 색상 버퍼) 초기화하기
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    // 색상 버퍼 및 깊이 버퍼 초기화
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* 여기서부터 루프에서 실행시킬 모든 렌더링 명령(rendering commands)을 작성함. */

    /* 변환행렬 계산 및 쉐이더 객체에 전송 */

    // 변환행렬을 전송할 쉐이더 프로그램 바인딩
    pbrShader.use();

    // 카메라 클래스로부터 뷰 행렬(= LookAt 행렬) 가져오기
    glm::mat4 view = camera.GetViewMatrix();

    // 계산된 뷰 행렬을 쉐이더 프로그램에 전송
    pbrShader.setMat4("view", view);

    /* 기타 uniform 변수들 쉐이더 객체에 전송 */

    // 카메라 위치값 쉐이더 프로그램에 전송
    pbrShader.setVec3("camPos", camera.Position);

    /* 미리 계산된 irradiance 가 저장되어 있는 irradianceMap 을 바인딩 */

    // irradianceMap 이 렌더링된 큐브맵 텍스쳐를 바인딩할 0번 texture unit 활성화
    glActiveTexture(GL_TEXTURE0);

    // irradianceMap 큐브맵 텍스쳐 바인딩
    glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);

    /* 미리 계산된 split-sum approximation 의 첫 번째 적분식 결과값이 저장되어 있는 pre-filtered env map 을 바인딩 */

    // pre-filtered env map 이 렌더링된 큐브맵 텍스쳐를 바인딩할 1번 texture unit 활성화
    glActiveTexture(GL_TEXTURE1);

    // prefilterMap 큐브맵 텍스쳐 바인딩
    glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);

    /* 미리 계산된 split-sum approximation 의 두 번째 적분식 결과값이 저장되어 있는 BRDF Integration map 을 바인딩 */

    // BRDF Integration map 이 렌더링된 2D 텍스쳐를 바인딩할 2번 texture unit 활성화
    glActiveTexture(GL_TEXTURE2);

    // brdfLUTTexture 큐브맵 텍스쳐 바인딩
    glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);

    /* 각 Sphere 에 적용할 모델행렬 계산 및 Sphere 렌더링 */

    // 모델행렬을 단위행렬로 초기화
    glm::mat4 model = glm::mat4(1.0f);

    // 각 행과 열을 이중 for-loop 로 순회하며 각 구체의 모델행렬 계산
    for (int row = 0; row < nrRows; ++row)
    {
      // 각 행의 구체끼리 동일한 metallic 값([0, 1] 범위 사이)을 계산하여 전송
      pbrShader.setFloat("metallic", (float)row / (float)nrRows);

      for (int col = 0; col < nrColumns; ++col)
      {
        // 각 열의 구체끼리 동일한 roughness 값([0.05, 1] 범위 사이)을 계산하여 전송
        // roughness 값이 0.0 이면 약간 이상해보여서 최소값을 0.05 로 clamping 했다고 함!
        pbrShader.setFloat("roughness", glm::clamp((float)col / (float)nrColumns, 0.05f, 1.0f));

        /*
          원점을 기준으로 현재 순회중인 행과 열을 계산하고,
          spacing 간격 만큼 떨어트려 x, y 위치값을 구해 모델행렬 계산
        */
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(
                                          (float)(col - (nrColumns / 2)) * spacing,
                                          (float)(row - (nrRows / 2)) * spacing,
                                          -2.0f));

        // 계산된 모델행렬을 쉐이더 프로그램에 전송
        pbrShader.setMat4("model", model);

        /*
          쉐이더 코드에서 노멀벡터를 World Space 로 변환할 때
          사용할 노멀행렬을 각 구체의 계산된 모델행렬로부터 계산 후,
          쉐이더 코드에 전송
        */
        pbrShader.setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));

        // 구체 렌더링
        renderSphere();
      }
    }

    /* 광원 정보 쉐이더 전송 및 광원 위치 시각화를 위한 구체 렌더링 */

    // 광원 데이터 개수만큼 for-loop 순회
    for (unsigned int i = 0; i < sizeof(lightPositions) / sizeof(lightPositions[0]); ++i)
    {
      // 시간에 따라 각 광원을 x 축 방향으로 [-5, 5] 범위 내에서 이동시키기 위한 위치값 재계산
      glm::vec3 newPos = lightPositions[i] + glm::vec3(std::sin(glfwImpl.getTime() * 5.0) * 5.0, 0.0, 0.0);

      // 광원 위치를 이동시키고 싶다면 아래의 기존 위치 재할당 코드 주석 처리
      newPos = lightPositions[i];

      // 광원 위치 및 색상 데이터를 쉐이더 프로그램에 전송
      pbrShader.setVec3("lightPositions[" + std::to_string(i) + "]", newPos);
      pbrShader.setVec3("lightColors[" + std::to_string(i) + "]", lightColors[i]);

      // 광원 위치 시각화를 위해 해당 위치에 구체 렌더링
      model = glm::mat4(1.0f);
      model = glm::translate(model, newPos);
      model = glm::scale(model, glm::vec3(0.5f));
      pbrShader.setMat4("model", model);
      pbrShader.setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));
      renderSphere();
    }

    /* skybox 렌더링 */

    // skybox 쉐이더 프로그램 바인딩 및 현재 카메라의 view 행렬 전송
    backgroundShader.use();
    backgroundShader.setMat4("view", view);

    // HDR 이미지 데이터가 렌더링된 큐브맵 텍스쳐를 바인딩할 0번 texture unit 활성화
    glActiveTexture(GL_TEXTURE0);

    // skybox 에 적용할 큐브맵 텍스쳐 바인딩
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

    // skybox 렌더링
    renderCube();

    // BRDF Integration map 을 실제로 화면에 렌더링해서 제대로 생성되었는지 확인
    // brdfShader.use();
    // renderQuad();

    glfwImpl.swapBuffers();
    glfwImpl.pollEvents();
  }

  return 0;
}

// 전방선언된 콜백함수 정의

/* 구체 렌더링 함수 구현 */

// 'Sphere VAO 객체(object) 참조 id 를 저장할 변수' 및 '구체의 정점 인덱스 버퍼 갯수 저장할 변수' 전역 선언
// (-> why? renderSphere() 중복 호출 시, 초기에 생성된 값을 참조하기 위해 전역 변수에 캐싱해 둔 것!)
unsigned int sphereVAO = 0;
unsigned int indexCount;

// 구체 렌더링 함수 구현부
void renderSphere()
{
  /*
    VAO 참조 ID 가 아직 할당되지 않았을 경우,
    Sphere 의 VAO(Vertex Array Object), VBO(Vertex Buffer Object), EBO(Element Buffer Object) 생성 및 바인딩(하단 VAO 관련 필기 참고)
  */
  if (sphereVAO == 0)
  {
    // VAO(Vertex Array Object) 객체 생성
    glGenVertexArrays(1, &sphereVAO);

    // VBO(Vertex Buffer Object) 및 EBO(Element Buffer Object) 객체 생성 -> Indexed Drawing 으로 그리겠군!
    unsigned int vbo, ebo;
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    // 구체의 정점 position, uv, normal, index 를 각각 계산하여 저장할 동적 배열 생성
    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> uv;
    std::vector<glm::vec3> normals;
    std::vector<unsigned int> indices;

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

        // 구체 정점의 position, normal, uv 동적 배열에 계산된 결과값을 추가
        positions.push_back(glm::vec3(xPos, yPos, zPos));
        uv.push_back(glm::vec2(xSegment, ySegment));
        normals.push_back(glm::vec3(xPos, yPos, zPos));
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

    // 구체의 정점 인덱스가 담긴 std::vector 동적 배열의 개수 (= Indexed Drawing 에 사용할 정점 개수) 를 전역 변수에 저장
    indexCount = static_cast<unsigned int>(indices.size());

    // 구체의 정점 position, normal, uv 데이터를 하나의 동적 배열 안에 모아서 저장
    std::vector<float> data;
    for (unsigned int i = 0; i < positions.size(); ++i)
    {
      // 구체의 정점 position 데이터 먼저 저장
      data.push_back(positions[i].x);
      data.push_back(positions[i].y);
      data.push_back(positions[i].z);

      // 두 번째로 정점 normal 데이터 저장
      if (normals.size() > 0)
      {
        data.push_back(normals[i].x);
        data.push_back(normals[i].y);
        data.push_back(normals[i].z);
      }

      // 세 번째로 정점 uv 데이터 저장
      if (uv.size() > 0)
      {
        data.push_back(uv[i].x);
        data.push_back(uv[i].y);
      }
    }

    // VAO 객체 먼저 컨텍스트에 바인딩(연결)함.
    // -> 그래야 재사용할 여러 개의 VBO 객체들 및 설정 상태를 바인딩된 VAO 에 저장할 수 있음.
    glBindVertexArray(sphereVAO);

    // VBO 객체를 GL_ARRAY_BUFFER 타입의 버퍼 유형 상태에 바인딩.
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // 실제 정점 데이터를 OpenGL 컨텍스트에 바인딩된 VBO 객체에 덮어씀.
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);

    // EBO 객체를 GL_ELEMENT_ARRAY_BUFFER 타입의 버퍼 유형 상태에 바인딩.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    // 실제 정점의 인덱스 배열을 OpenGL 컨텍스트에 바인딩된 EBO 객체에 덮어씀.
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // 구체 정점의 각 데이터 해석 방식을 정의할 때 사용할 '정점 간 메모리 블록 간격(stride)' 계산
    unsigned int stride = (3 + 2 + 3) * sizeof(float);

    // 원래 버텍스 쉐이더의 모든 location 의 attribute 변수들은 사용 못하도록 디폴트 설정이 되어있음.
    // -> 그 중에서 0번 location 변수를 사용하도록 활성화
    glEnableVertexAttribArray(0);

    // 정점 위치 데이터(0번 location 입력변수 in vec3 aPos 에 전달할 데이터) 해석 방식 정의
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void *)0);

    // 1번 location 변수를 사용하도록 활성화
    glEnableVertexAttribArray(1);

    // 정점 노멀 데이터(1번 location 입력변수 in vec3 aNormal 에 전달할 데이터) 해석 방식 정의
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void *)(3 * sizeof(float)));

    // 2번 location 변수를 사용하도록 활성화
    glEnableVertexAttribArray(2);

    // 정점 UV 데이터(2번 location 입력변수 in vec2 aTexCoords 에 전달할 데이터) 해석 방식 정의
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void *)(6 * sizeof(float)));
  }

  // 구체에 적용할 VAO 객체를 바인딩하여, 해당 객체에 저장된 VBO, EBO 객체와 설정대로 그리도록 명령
  glBindVertexArray(sphereVAO);

  // 바인딩된 VAO 객체에 저장된 EBO 객체로부터 Indexed Drawing
  // 각 파라미터는 (삼각형 모드, 그리고 싶은 정점 개수, 인덱스들의 타입, EBO 버퍼 offset)
  glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
}

/* 큐브 렌더링 함수 구현 */

// Cube VBO, VAO 객체(object) 참조 id 를 저장할 변수 전역 선언 (why? 다른 함수들에서도 참조)
unsigned int cubeVAO = 0;
unsigned int cubeVBO = 0;

void renderCube()
{
  /*
    VAO 참조 ID 가 아직 할당되지 않았을 경우,
    큐브의 VAO(Vertex Array Object), VBO(Vertex Buffer Object) 생성 및 바인딩(하단 VAO 관련 필기 참고)
  */
  if (cubeVAO == 0)
  {
    // 큐브의 정점 데이터 정적 배열 초기화
    float vertices[] = {
        // back face
        -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
        1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,   // top-right
        1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,  // bottom-right
        1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,   // top-right
        -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
        -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,  // top-left
        // front face
        -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
        1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,  // bottom-right
        1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,   // top-right
        1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,   // top-right
        -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,  // top-left
        -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
        // left face
        -1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,   // top-right
        -1.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,  // top-left
        -1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
        -1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
        -1.0f, -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,  // bottom-right
        -1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,   // top-right
                                                            // right face
        1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,     // top-left
        1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,   // bottom-right
        1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,    // top-right
        1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,   // bottom-right
        1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,     // top-left
        1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,    // bottom-left
        // bottom face
        -1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
        1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,  // top-left
        1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,   // bottom-left
        1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,   // bottom-left
        -1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,  // bottom-right
        -1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
        // top face
        -1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
        1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,   // bottom-right
        1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,  // top-right
        1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,   // bottom-right
        -1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
        -1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f   // bottom-left
    };

    // VAO(Vertex Array Object) 객체 생성
    glGenVertexArrays(1, &cubeVAO);

    // VBO(Vertex Buffer Object) 객체 생성
    glGenBuffers(1, &cubeVBO);

    // VAO 객체 먼저 컨텍스트에 바인딩(연결)함.
    // -> 그래야 재사용할 여러 개의 VBO 객체들 및 설정 상태를 바인딩된 VAO 에 저장할 수 있음.
    glBindVertexArray(cubeVAO);

    // VBO 객체는 GL_ARRAY_BUFFER 타입의 버퍼 유형 상태에 바인딩되어야 함.
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);

    // 실제 정점 데이터를 생성 및 OpenGL 컨텍스트에 바인딩된 VBO 객체에 덮어씀.
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 원래 버텍스 쉐이더의 모든 location 의 attribute 변수들은 사용 못하도록 디폴트 설정이 되어있음.
    // -> 그 중에서 0번 location 변수를 사용하도록 활성화
    glEnableVertexAttribArray(0);

    // 정점 위치 데이터(0번 location 입력변수 in vec3 aPos 에 전달할 데이터) 해석 방식 정의
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);

    // 1번 location 변수를 사용하도록 활성화
    glEnableVertexAttribArray(1);

    // 정점 노멀 데이터(1번 location 입력변수 in vec3 aNormal 에 전달할 데이터) 해석 방식 정의
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));

    // 2번 location 변수를 사용하도록 활성화
    glEnableVertexAttribArray(2);

    // 정점 UV 데이터(2번 location 입력변수 in vec2 aTexCoords 에 전달할 데이터) 해석 방식 정의
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));

    // VBO 객체 설정을 끝마쳤으므로, OpenGL 컨텍스트로부터 바인딩 해제
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // 마찬가지로, VAO 객체도 OpenGL 컨텍스트로부터 바인딩 해제
    glBindVertexArray(0);
  }

  /* 큐브 그리기 */

  // 큐브에 적용할 VAO 객체를 바인딩하여, 해당 객체에 저장된 VBO 객체와 설정대로 그리도록 명령
  glBindVertexArray(cubeVAO);

  // 큐브 그리기 명령
  glDrawArrays(GL_TRIANGLES, 0, 36);

  // 그리기 명령 종료 후, VAO 객체 바인딩 해제
  glBindVertexArray(0);
}

/* BRDF Integration map 로 생성할 QuadMesh 를 렌더링하는 함수 구현 */

// QuadMesh VBO, VAO 객체(object) 참조 id 를 저장할 변수 전역 선언 (why? 다른 함수들에서도 참조)
unsigned int quadVAO = 0;
unsigned int quadVBO = 0;

void renderQuad()
{
  /*
    VAO 참조 ID 가 아직 할당되지 않았을 경우,
    QuadMesh 의 VAO(Vertex Array Object), VBO(Vertex Buffer Object) 생성 및 바인딩(하단 VAO 관련 필기 참고)
  */
  if (quadVAO == 0)
  {
    // QuadMesh 의 정점 데이터 정적 배열 초기화
    float quadVertices[] = {
        // positions        // texture Coords
        -1.0f,
        1.0f,
        0.0f,
        0.0f,
        1.0f,
        -1.0f,
        -1.0f,
        0.0f,
        0.0f,
        0.0f,
        1.0f,
        1.0f,
        0.0f,
        1.0f,
        1.0f,
        1.0f,
        -1.0f,
        0.0f,
        1.0f,
        0.0f,
    };

    // VAO(Vertex Array Object) 객체 생성
    glGenVertexArrays(1, &quadVAO);

    // VBO(Vertex Buffer Object) 객체 생성
    glGenBuffers(1, &quadVBO);

    // VAO 객체 먼저 컨텍스트에 바인딩(연결)함.
    // -> 그래야 재사용할 여러 개의 VBO 객체들 및 설정 상태를 바인딩된 VAO 에 저장할 수 있음.
    glBindVertexArray(quadVAO);

    // VBO 객체는 GL_ARRAY_BUFFER 타입의 버퍼 유형 상태에 바인딩되어야 함.
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);

    // 실제 정점 데이터를 생성 및 OpenGL 컨텍스트에 바인딩된 VBO 객체에 덮어씀.
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    // 원래 버텍스 쉐이더의 모든 location 의 attribute 변수들은 사용 못하도록 디폴트 설정이 되어있음.
    // -> 그 중에서 0번 location 변수를 사용하도록 활성화
    glEnableVertexAttribArray(0);

    // 정점 위치 데이터(0번 location 입력변수 in vec3 aPos 에 전달할 데이터) 해석 방식 정의
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);

    // 1번 location 변수를 사용하도록 활성화
    glEnableVertexAttribArray(1);

    // 정점 UV 데이터(1번 location 입력변수 in vec2 aTexCoords 에 전달할 데이터) 해석 방식 정의
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));

    // VBO 객체 설정을 끝마쳤으므로, OpenGL 컨텍스트로부터 바인딩 해제
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // 마찬가지로, VAO 객체도 OpenGL 컨텍스트로부터 바인딩 해제
    glBindVertexArray(0);
  }

  /* QuadMesh 그리기 */

  // QuadMesh 에 적용할 VAO 객체를 바인딩하여, 해당 객체에 저장된 VBO 객체와 설정대로 그리도록 명령
  glBindVertexArray(quadVAO);

  // QuadMesh 그리기 명령
  // (Quad 를 그리려면 2개의 삼각형(== 6개의 정점)이 정의되어야 하지만,
  // 위에서 4개의 정점 데이터만 정의했으므로, 정점을 공유하여 삼각형을 조립하는 GL_TRIANGLE_STRIP 모드로 렌더링한다.)
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  // 그리기 명령 종료 후, VAO 객체 바인딩 해제
  glBindVertexArray(0);
}

// 텍스쳐 이미지 로드 및 객체 생성 함수 구현부 (텍스쳐 객체 참조 id 반환)
unsigned int loadTexture(const char *path)
{
  unsigned int textureID;       // 텍스쳐 객체(object) 참조 id 를 저장할 변수 선언
  glGenTextures(1, &textureID); // 텍스쳐 객체 생성

  int width, height, nrComponents; // 로드한 이미지의 width, height, 색상 채널 개수를 저장할 변수 선언

  // 이미지 데이터 가져와서 char 타입의 bytes 데이터로 저장.
  // 이미지 width, height, 색상 채널 변수의 주소값도 넘겨줌으로써, 해당 함수 내부에서 값을 변경. -> 출력변수 역할
  unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
  if (data)
  {
    // 이미지 데이터 로드 성공 시 처리

    // 이미지 데이터의 색상 채널 개수에 따라 glTexImage2D() 에 넘겨줄 픽셀 데이터 포맷의 ENUM 값을 결정
    GLenum format;
    if (nrComponents == 1)
      format = GL_RED;
    else if (nrComponents == 3)
      format = GL_RGB;
    else if (nrComponents == 4)
      format = GL_RGBA;

    // 텍스쳐 객체 바인딩 및 로드한 이미지 데이터 쓰기
    glBindTexture(GL_TEXTURE_2D, textureID);                                                  // GL_TEXTURE_2D 타입의 상태에 텍스쳐 객체 바인딩 > 이후 텍스쳐 객체 설정 명령은 바인딩된 텍스쳐 객체에 적용.
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data); // 로드한 이미지 데이터를 현재 바인딩된 텍스쳐 객체에 덮어쓰기
    glGenerateMipmap(GL_TEXTURE_2D);                                                          // 현재 바인딩된 텍스쳐 객체에 필요한 모든 단계의 Mipmap 을 자동 생성함.

    // 현재 GL_TEXTURE_2D 상태에 바인딩된 텍스쳐 객체 설정하기
    // Texture Wrapping 모드를 반복 모드로 설정 ([(0, 0), (1, 1)] 범위를 벗어나는 텍스쳐 좌표에 대한 처리)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // 텍스쳐 축소/확대 및 Mipmap 교체 시 Texture Filtering (텍셀 필터링(보간)) 모드 설정 (관련 필기 정리 하단 참고)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  }
  else
  {
    // 이미지 데이터 로드 실패 시 처리
    std::cout << "Texture failed to load at path: " << path << std::endl;
  }

  // 텍스쳐 객체에 이미지 데이터를 전달하고, 밉맵까지 생성 완료했다면, 로드한 이미지 데이터는 항상 메모리 해제할 것!
  stbi_image_free(data);

  // 텍스쳐 객체 참조 ID 반환
  return textureID;
}

/*
  stb_image.h

  주요 이미지 파일 포맷을 로드할 수 있는
  싱글 헤더 이미지로드 라이브러리.

  #define 매크로 전처리기를 통해
  특정 매크로를 선언함으로써, 헤더파일 내에서
  해당 매크로 영역의 코드만 include 할 수 있도록 함.

  실제로 stb_image.h 안에 보면

  #ifdef STB_IMAGE_IMPLEMENTATION
  ~
  #endif

  요렇게 전처리기가 정의되어 있는 부분이 있음.
  이 부분의 코드들만 include 하겠다는 것이지!
*/

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

/*
  .hdr 파일이란 무엇인가?


  PBR 파이프라인에서 계산되는 모든 값들은
  실제 물리적인 측정치와 일치해야 더 정확하게 결과물을 렌더링할 수 있음!

  그래서, 이전 PBR-Lighting 챕터에서도
  Reflectance Equation 의 최종 결과값이 [0.0, 1.0] 사이로 clamping 되지 않도록,
  HDR -> LDR 범위로 tone mapping 시킨 다음에 gl_FragColor 에 할당해줬었지?

  이처럼, PBR 파이프라인 계산의 결과값이 clamping 되어
  훼손되는 것을 방지하는 게 아주 중요한데,
  IBL 에서도 이는 마찬가지임.

  IBL 에서 사용되는 EnvMap 은 주변에서 오는 모든 간접광들에 대한
  radiance 를 저장한 Cubemap 텍스쳐라고 보면 될텐데,

  이러한 radiance 값이 일반적인 이미지 포맷으로 저장된 것으로부터 사용한다면,
  LDR 범위([0.0, 1.0])로 clamping 된 데이터를 사용하게 되겠지?

  이렇게 되면 당연히 부정확한 결과물이 나올 수밖에 없음.


  그래서, 이러한 indirect light 들에 대한
  radiance 를 HDR 범위([0.0, 1.0]을 넘어서는 범위)로 관리할 수 있도록,

  각 픽셀을 floating point value(부동 소수점) 포맷으로 저장한
  특별한 이미지 파일을 .hdr, 즉, HDR file 이라고 하는 것임!


  HDR 파일 포맷의 또 다른 특징이 있다면,
  기존 cubemap 의 각 6면을 하나의 이미지 안에 저장해둘 수 있도록,
  구체(sphere)를 평면(plane)에 투영한 등장방형도법('Equirectangular projection')으로
  저장되어 있음.

  그래서 겉으로 보기에는 약간 이미지가 왜곡되어 보이는 느낌이 들고,
  대부분 가로 해상도가 더 넓기 때문에, cubemap 6면에서 top, bottom 면에 해당하는
  영역의 픽셀 정보가 투영되는 과정에서 약간 유실된다는 단점이 있음.
*/

/*
  Floating point framebuffer


  HDR 을 구현하려면, [0, 1] 범위를 넘어서는 색상값들이
  프레임버퍼에 attach 된 텍스쳐 객체에 저장될 때,
  [0, 1] 사이로 clamping 되지 않고,

  원래의 색상값이 그대로 저장될 수 있어야 함.

  그러나, 일반적인 프레임버퍼에서 color 를 저장할 때,
  내부 포맷으로 사용하는 GL_RGB 같은 포맷은
  fixed point(고정 소수점) 포맷이기 때문에,

  OpenGL 에서 프레임버퍼에 색상값을 저장하기 전에
  자동으로 [0, 1] 사이의 값으로 clamping 해버리는 문제가 있음.


  이를 해결하기 위해,
  GL_RGB16F, GL_RGBA16F, GL_RGB32F, GL_RGBA32F 같은
  floating point(부동 소수점) 포맷으로
  프레임버퍼의 내부 색상 포맷을 변경하면,

  [0, 1] 범위를 벗어나는 값들에 대해서도
  부동 소수점 형태로 저장할 수 있도록 해줌!


  이때, 일반적인 프레임버퍼의 기본 색상 포맷인
  GL_RGB 같은 경우 하나의 컴포넌트 당 8 bits 메모리를 사용하는데,
  GL_RGB32F, GL_RGBA32F 같은 포맷은 하나의 컴포넌트 당 32 bits 의 메모리를 사용하기 때문에,
  우리는 이 정도로 많은 메모리를 필요로 하지는 않음.

  따라서, GL_RGB16F, GL_RGBA16F 같이
  한 컴포넌트 당 16 bits 정도의 메모리를 예약해서 사용하는
  적당한 크기의 색상 포맷으로 사용하는 게 좋겠지!
*/

/*
  Cubemap 의 fov(시야각)은 왜 90도여야 할까?


  즉, 큐브맵으로 HDR 이미지를 변환하려면,

  큐브맵의 각 6면의 방향으로 바라본 시점에서
  각각의 HDR 이미지 데이터를 샘플링하여 cubemap 텍스쳐 객체 버퍼에 기록해야겠지?

  이때, shadow map 큐브맵의 각 6면이 서로 만나는
  모서리 지점에서 소위 '아다리가 딱딱 맞아 떨어지려면',

  카메라 위치에서 각 6면을 바라보는 시점에서의 원근 투영이
  90도의 fov(== 시야각)로 설정되어야 함.

  그래야 '><' 요런 모양으로 '앞면/뒷면/오른쪽 면/왼쪽 면'
  (또는 '윗면/아랫면/오른쪽 면/왼쪽 면' 또는 '앞면/뒷면/윗면/아랫면') 을
  투영변환할 때의 frustum 의 시야각이 '90 + 90 + 90 + 90 = 360' 으로 딱 떨어져서

  각 면을 바라보면서 투영변환된 Cubemap 을
  큐브맵 버퍼에 기록해서 나중에 맞춰보면

  각 면이 만나는 모서리 지점이
  퍼즐처럼 딱 맞아 떨어져서 이어지는 걸 볼 수 있음!
*/

/*
  skybox 렌더링 시, 깊이 테스트 함수를 GL_LEQUAL 로 설정하는 이유?


  skybox.vs 파일에서 관련 필기를 참고해보면,
  skybox 는 렌더링 최적화를 위해 깊이값이 모두 1 로 계산될 것임.

  그런데, depth buffer 의 초기값 또한 항상 1 로 설정되기 때문에,
  깊이테스트 기본 모드인 GL_LESS 로 수행하면,

  skybox 의 깊이값과 depth buffer 의 초기값이 동일하여
  skybox 의 모든 프래그먼트들이 discard 되어버리는 문제가 발생함.

  이를 방지하기 위해,
  깊이 테스트 모드를 GL_LEQUAL 로 설정함으로써,
  현재 depth buffer 의 값보다 '작거나 같으면'
  깊이 테스트를 통과할 수 있도록 변경한 것임!
*/

/*
  GL_TEXTURE_CUBE_MAP_SEAMLESS


  pre-filtered env map 의 mip level 이 점점 올라가면서 roughness 값이 커지면,
  convolution 할 때 샘플링하는 영역인 specular lobe 가 더 커지게 되는데,

  이로 인해 convolution 결과물이 원본 HDR 큐브맵과 점점 달라지게 됨.

  문제는, 이럴수록 Cubemap 의 각 face 사이의 경계선 부분에서 seam line 이 도드라져 보임.
  이는 OpenGL 이 기본적으로 Cubemap face 사이에서의 linear interpolation 처리를 해주지 않기 때문임.

  이때, GL_TEXTURE_CUBE_MAP_SEAMLESS 기능을 활성화해주면,
  OpenGL 에서 Cubemap 의 각 face 가 접하는 경계선 부근에서 샘플링을 처리할 때,
  각 면 사이의 texel 들을 linear interpolation 처리하여
  각 면의 경계에서 매끄러운 보간을 수행함.

  이를 통해 Cubemap face 경계 부근에서의 부자연스러운 seam line 을 방지할 수 있음.
*/
