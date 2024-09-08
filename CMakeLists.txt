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

# 헤더 파일 및 3rd party 디렉토리 추가
target_include_directories(${PROJECT_NAME} PRIVATE
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/3rdparty
)

# src/... 하위의 *.cpp 파일들을 재귀적으로 검색하여 찾기
file(GLOB_RECURSE CPP_SOURCES ${CMAKE_SOURCE_DIR}/src/*.cpp)

# glad.c 소스 파일을 명시하는 변수 설정
set(C_SOURCES ${CMAKE_SOURCE_DIR}/src/glad.c)

# C++ 및 C 소스 파일 목록을 결합하여 변수로 설정
set(SOURCE_FILES ${C_SOURCES} ${CPP_SOURCES})

# 타겟에 소스 파일 추가
target_sources(${PROJECT_NAME} PRIVATE ${SOURCE_FILES})

# 타겟에 라이브러리 링크
target_link_libraries(${PROJECT_NAME} PRIVATE glfw spdlog)

# 타겟에 매크로 컴파일 정의
target_compile_definitions(${PROJECT_NAME} PRIVATE
  WINDOW_NAME="${WINDOW_NAME}"
  WINDOW_WIDTH=${WINDOW_WIDTH}
  WINDOW_HEIGHT=${WINDOW_HEIGHT}
)
