cmake_minimum_required(VERSION 3.13)

# 기본 변수 설정
set(PROJECT_NAME pbr_renderer)
set(CMAKE_CXX_STANDARD 17)

# 매크로 변수 설정
set(WINDOW_NAME "PBR Renderer")
set(WINDOW_WIDTH 1280)
set(WINDOW_HEIGHT 720)

# FetchContent 모듈 포함
include(FetchContent)

# spdlog
FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG v1.x
)
FetchContent_MakeAvailable(spdlog)

# glfw
FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG 3.3.2
)
FetchContent_MakeAvailable(glfw)

# 실행 파일 정의
add_executable(${PROJECT_NAME} src/main.cpp)

# 타겟에 glad 파일 추가
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_sources(${PROJECT_NAME} PRIVATE
  ${CMAKE_SOURCE_DIR}/src/glad.c
  ${CMAKE_SOURCE_DIR}/src/shader/shader.cpp
  ${CMAKE_SOURCE_DIR}/src/camera/camera.cpp
  ${CMAKE_SOURCE_DIR}/src/glfw_impl/glfw_impl.cpp
)

# 타겟에 라이브러리 링크
target_link_libraries(${PROJECT_NAME} PRIVATE glfw spdlog)

# 타겟에 매크로 컴파일 정의
target_compile_definitions(${PROJECT_NAME} PRIVATE
  WINDOW_NAME="${WINDOW_NAME}"
  WINDOW_WIDTH=${WINDOW_WIDTH}
  WINDOW_HEIGHT=${WINDOW_HEIGHT}
)
