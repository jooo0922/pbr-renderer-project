#include <algorithm> // std::min(), std::max() 를 사용하기 위해 포함한 라이브러리

// 행렬 및 벡터 계산에서 사용할 Header Only 라이브러리 include
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader/shader.hpp"
#include "camera/camera.hpp"
#include "glfw_impl/glfw_impl.hpp"
#include "renderable_objects/sphere.hpp"
#include "renderable_objects/cube.hpp"
#include "renderable_objects/quad.hpp"
#include "gl_objects/texture.hpp"
#include "gl_objects/cube_texture.hpp"
#include "gl_objects/frame_buffer_object.hpp"
#include "gl_objects/render_buffer_object.hpp"
#include "gl_context/gl_context.hpp"

#include "features/material_feature.hpp"

#include <iostream>
#include <spdlog/spdlog.h>

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

  // GLContext 싱글턴 인스턴스 접근
  GLContext &glContext = GLContext::getInstance();

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

  /* Feature 객체 생성 */
  MaterialFeature materialFeature;

  /* Feature 객체 초기화 */
  materialFeature.setPbrShader(&pbrShader);
  materialFeature.initialize();

  /* 각 구체에 공통으로 적용할 PBR Parameter 들을 쉐이더 프로그램에 전송 */

  // PBR 쉐이더 프로그램 바인딩
  pbrShader.use();

  // irradiance map 큐브맵 텍스쳐를 바인딩할 0번 texture unit 위치값 전송
  pbrShader.setInt("irradianceMap", 0);

  // pre-filtered env map 큐브맵 텍스쳐를 바인딩할 1번 texture unit 위치값 전송
  pbrShader.setInt("prefilterMap", 1);

  // BRDF Integration map 텍스쳐를 바인딩할 2번 texture unit 위치값 전송
  pbrShader.setInt("brdfLUT", 2);

  /* skybox 에 적용할 uniform 변수들을 쉐이더 프로그램에 전송 */

  // skybox 쉐이더 프로그램 바인딩
  backgroundShader.use();

  // HDR 이미지 데이터가 렌더링된 큐브맵 텍스쳐를 바인딩할 0번 texture unit 위치값 전송
  backgroundShader.setInt("environmentMap", 0);

  /* 광원 데이터 초기화 */
  // TODO : Light 클래스 추상화 -> Light 관련 Feature 클래스 구현 시 추상화할 것.
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

  /** renderable objects 초기화 */

  // Sphere 객체 생성
  Sphere sphere;

  // Cube 객체 생성
  Cube cube;

  // Quad 객체 생성
  Quad quad;

  /* Equirectangular HDR 파일 > Cubemap 변환 시 필요한 버퍼 생성 및 바인딩 */

  // Equirectangular HDR 파일을 샘플링하여 렌더링할 FBO(FrameBufferObject) 객체 및 RBO(RenderBufferObject) 생성
  FrameBufferObject captureFBO;
  RenderBufferObject captureRBO;

  // 생성한 FBO 객체 및 RBO 객체 바인딩
  captureFBO.bind();
  captureRBO.bind();

  // Renderbuffer 해상도를 Cubemap 각 면의 해상도인 512 * 512 로 맞춤.
  captureRBO.setStorage(512, 512);

  // FBO 객체에 생성한 RBO 객체 attach
  captureFBO.attachRenderBuffer(captureRBO.getID());

  /* .hdr 이미지 로드하여 텍스쳐 객체 생성 */
  Texture hdrTexture("resources/textures/hdr/newport_loft.hdr", GL_RGB16F, GL_RGB);

  /* HDR 이미지 텍스쳐를 Cubemap 형태로 변환할 color buffer 로써 Cubemap 텍스쳐 객체 생성 */
  CubeTexture envCubemap(512, 512, GL_RGB16F, GL_RGB);
  envCubemap.setMinFilter(GL_LINEAR_MIPMAP_LINEAR);

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

  // HDR 이미지 텍스쳐를 0번 texture unit 에 바인딩해서 사용
  hdrTexture.use(GL_TEXTURE0);

  /* 렌더링 루프 진입 이전에 Cubemap 버퍼에 HDR 이미지 렌더링 */

  // Cubemap 버퍼의 각 면의 해상도 512 * 512 에 맞춰 viewport 해상도 설정
  glContext.resize(512, 512);

  // Cubemap 버퍼의 각 면을 attach 할 FBO 객체 바인딩
  captureFBO.bind();

  // HDR 이미지가 적용된 단위 큐브의 각 면을 바라보도록 카메라를 회전시키며 6번 렌더링
  for (unsigned int i = 0; i < 6; i++)
  {
    // 쉐이더 객체에 단위 큐브의 각 면을 바라보도록 계산하는 뷰 행렬 전송
    equirectangularToCubemapShader.setMat4("view", captureViews[i]);

    // Cubemap 버퍼의 각 면을 현재 바인딩된 FBO 객체에 돌아가며 attach
    captureFBO.attachTexture(envCubemap.getID(), GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);

    // 단위 큐브를 attach 된 Cubemap 버퍼에 렌더링하기 전, 색상 버퍼와 깊이 버퍼를 깨끗하게 비워줌
    glContext.clear();

    // 단위 큐브 렌더링 -> Point Shadow 에서는 Cubemap 버퍼 각 면에 렌더링해주는 작업을 geometry shader 에서 처리해줬었지!
    cube.draw(equirectangularToCubemapShader);
  }

  // Cubemap 버퍼에 렌더링 완료 후, 기본 프레임버퍼로 바인딩 초기화
  captureFBO.unbind();

  /* Bright dot artifact 해결을 위해 원본 HDR Cubemap 의 mipmap 생성 */
  envCubemap.generateMipmap();

  /* diffuse term 적분식의 결과값(= irradiance)를 렌더링할 color buffer 로써 Cubemap 텍스쳐 객체 생성 */
  CubeTexture irradianceMap(32, 32, GL_RGB16F, GL_RGB);

  // irradiance map 을 렌더링할 때 사용할 FBO 객체 및 RBO 객체 바인딩
  captureFBO.bind();
  captureRBO.bind();

  // Renderbuffer 해상도를 Cubemap 각 면의 해상도인 32 * 32 로 맞춤.
  captureRBO.setStorage(32, 32);

  /* irradianceShader 에 텍스쳐 및 행렬 전달 */

  // irradianceShader 쉐이더 바인딩
  irradianceShader.use();

  // HDR 큐브맵 텍스쳐를 바인딩할 0번 texture unit 위치값 전송
  irradianceShader.setInt("environmentMap", 0);

  // fov(시야각)이 90로 고정된 투영행렬 전송
  irradianceShader.setMat4("projection", captureProjection);

  // HDR 큐브맵 텍스쳐를 0번 texture unit 에 바인딩하여 사용
  envCubemap.use(GL_TEXTURE0);

  /* 렌더링 루프 진입 이전에 Cubemap 버퍼에 irradiance map 렌더링 */

  // Cubemap 버퍼의 각 면의 해상도 32 * 32 에 맞춰 viewport 해상도 설정
  glContext.resize(32, 32);

  // Cubemap 버퍼의 각 면을 attach 할 FBO 객체 바인딩
  captureFBO.bind();

  // irradiance map 을 렌더링할 단위 큐브의 각 면을 바라보도록 카메라를 회전시키며 6번 렌더링
  for (unsigned int i = 0; i < 6; i++)
  {
    // 쉐이더 객체에 단위 큐브의 각 면을 바라보도록 계산하는 뷰 행렬 전송
    irradianceShader.setMat4("view", captureViews[i]);

    // Cubemap 버퍼의 각 면을 현재 바인딩된 FBO 객체에 돌아가며 attach
    captureFBO.attachTexture(irradianceMap.getID(), GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);

    // 단위 큐브를 attach 된 Cubemap 버퍼에 렌더링하기 전, 색상 버퍼와 깊이 버퍼를 깨끗하게 비워줌
    glContext.clear();

    // 단위 큐브 렌더링 -> irradianceShader 에서 적분식을 풀면서 각 프래그먼트 지점의 irradiance 를 Cubemap 버퍼에 저장함.
    cube.draw(irradianceShader);
  }

  // Cubemap 버퍼에 렌더링 완료 후, 기본 프레임버퍼로 바인딩 초기화
  captureFBO.unbind();

  /*
    split-sum approximation(= specular term 적분식)에서
    첫 번째 적분식의 결과값(= pre-filtered environment map)를 렌더링할 color buffer 로써
    Cubemap 텍스쳐 객체 생성
  */
  CubeTexture prefilterMap(128, 128, GL_RGB16F, GL_RGB);
  prefilterMap.generateMipmap();

  /* prefilterShader 에 텍스쳐 및 행렬 전달 */

  // prefilterShader 쉐이더 바인딩
  prefilterShader.use();

  // HDR 큐브맵 텍스쳐를 바인딩할 0번 texture unit 위치값 전송
  prefilterShader.setInt("environmentMap", 0);

  // fov(시야각)이 90로 고정된 투영행렬 전송
  prefilterShader.setMat4("projection", captureProjection);

  // HDR 큐브맵 텍스쳐를 0번 texture unit 에 바인딩하여 사용
  envCubemap.use(GL_TEXTURE0);

  /* 렌더링 루프 진입 이전에 Cubemap 버퍼에 각 mip level 마다 pre-filtered env map 렌더링 */

  // Cubemap 버퍼의 각 면을 attach 할 FBO 객체 바인딩
  captureFBO.bind();

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
    captureRBO.bind();

    // Renderbuffer 해상도를 각 mipmap 의 해상도로 맞춤.
    captureRBO.setStorage(mipWidth, mipHeight);

    // Cubemap 버퍼의 각 면의 해상도를 각 mipmap 의 해상도로 맞춰 viewport 해상도 설정
    glContext.resize(mipWidth, mipHeight);

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
      captureFBO.attachTexture(prefilterMap.getID(), GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, mip);

      // 단위 큐브를 attach 된 Cubemap 버퍼에 렌더링하기 전, 색상 버퍼와 깊이 버퍼를 깨끗하게 비워줌
      glContext.clear();

      // 단위 큐브 렌더링 -> prefilterShader 에서 split sum approximation 의 첫 번째 적분식의 결과값을 풀어 Cubemap 버퍼에 저장함.
      cube.draw(prefilterShader);
    }
  }

  // Cubemap 버퍼에 렌더링 완료 후, 기본 프레임버퍼로 바인딩 초기화
  captureFBO.unbind();

  /*
    split-sum approximation(= specular term 적분식)에서
    두 번째 적분식의 결과값(= BRDF Integration map)를 렌더링할 color buffer 로써 2D 텍스쳐 객체 생성

    -> split-sum approximation 의 두 번째 적분식의 scale, bias 값만 r, g 채널에 각각 저장하기 위해 GL_RG16F 포맷으로 생성
  */
  Texture brdfLUTTexture(512, 512, GL_RG16F, GL_RG);

  /* 렌더링 루프 진입 이전에 2D 텍스쳐 버퍼에 BRDF Integration map 렌더링 */

  // BRDF Integration map 버퍼를 attach 할 FBO 객체 바인딩
  captureFBO.bind();

  // BRDF Integration map 을 렌더링할 때 사용할 RBO 객체 바인딩
  captureRBO.bind();

  // Renderbuffer 해상도를 BRDF Integration map 의 해상도로 맞춤.
  captureRBO.setStorage(512, 512);

  // BRDF Integration map 을 현재 바인딩된 FBO 객체에 attach
  captureFBO.attachTexture(brdfLUTTexture.getID(), GL_TEXTURE_2D);

  // BRDF Integration map 의 해상도에 맞춰 viewport 해상도 설정
  glContext.resize(512, 512);

  // brdfShader 쉐이더 바인딩
  brdfShader.use();

  // attach 된 BRDF Integration map 버퍼에 렌더링하기 전, 색상 버퍼와 깊이 버퍼를 깨끗하게 비워줌
  glContext.clear();

  // 단일 QuadMesh 렌더링 -> brdfShader 에서 split sum approximation 의 두 번째 적분식의 결과값을 풀어 BRDF Integration map 버퍼에 저장함.
  quad.draw(brdfShader);

  // BRDF Integration map 버퍼에 렌더링 완료 후, 기본 프레임버퍼로 바인딩 초기화
  captureFBO.unbind();

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

    // 색상 버퍼 및 깊이 버퍼 초기화
    glContext.clear();

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
    irradianceMap.use(GL_TEXTURE0);

    /* 미리 계산된 split-sum approximation 의 첫 번째 적분식 결과값이 저장되어 있는 pre-filtered env map 을 바인딩 */
    prefilterMap.use(GL_TEXTURE1);

    /* 미리 계산된 split-sum approximation 의 두 번째 적분식 결과값이 저장되어 있는 BRDF Integration map 을 바인딩 */
    brdfLUTTexture.use(GL_TEXTURE2);

    /* 각 Feature 클래스들의 렌더링 루프 작업 수행 */
    materialFeature.process();

    /* 각 Sphere 에 적용할 모델행렬 계산 및 Sphere 렌더링 */

    // 모델행렬을 단위행렬로 초기화
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.0f));

    // 계산된 모델행렬을 쉐이더 프로그램에 전송
    pbrShader.setMat4("model", model);

    /*
      쉐이더 코드에서 노멀벡터를 World Space 로 변환할 때
      사용할 노멀행렬을 각 구체의 계산된 모델행렬로부터 계산 후,
      쉐이더 코드에 전송
    */
    pbrShader.setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));

    // 구체 렌더링
    sphere.draw(pbrShader);

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
    }

    /* skybox 렌더링 */

    // skybox 쉐이더 프로그램 바인딩 및 현재 카메라의 view 행렬 전송
    backgroundShader.use();
    backgroundShader.setMat4("view", view);

    // HDR 큐브맵 텍스쳐를 0번 texture unit 에 바인딩하여 skybox 텍스쳐로 사용
    envCubemap.use(GL_TEXTURE0);

    // skybox 렌더링
    cube.draw(backgroundShader);

    // BRDF Integration map 을 실제로 화면에 렌더링해서 제대로 생성되었는지 확인
    // brdfShader.use();
    // quad.draw(brdfShader);

    glfwImpl.swapBuffers();
    glfwImpl.pollEvents();
  }

  /* 각 Feature 클래스들 종료 */
  materialFeature.finalize();

  return 0;
}

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
