cmake_minimum_required(VERSION 3.13)

# 기본 변수 설정
set(PROJECT_NAME pbr_renderer)
set(CMAKE_CXX_STANDARD 17)

# 매크로 변수 설정
set(WINDOW_NAME "PBR Renderer")
set(WINDOW_WIDTH 1280)
set(WINDOW_HEIGHT 720)

# Visual Studio 컴파일러가 한글 주석이 포함된 소스파일을 UTF-8 로 인코딩하는 옵션을 설정하여 C4819 warning 방지
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

# FetchContent 모듈 포함
include(FetchContent)

# 외부 라이브러리 설정 파일 포함
include(cmake/glfw.cmake)
include(cmake/spdlog.cmake)
include(cmake/assimp.cmake)

# ImGui 정적 라이브러리 정의 (코드 수정을 안하므로 정적 라이브러리로 빌드)
add_library(imgui STATIC
  ${CMAKE_SOURCE_DIR}/imgui/imgui.cpp
  ${CMAKE_SOURCE_DIR}/imgui/imgui_draw.cpp
  ${CMAKE_SOURCE_DIR}/imgui/imgui_tables.cpp
  ${CMAKE_SOURCE_DIR}/imgui/imgui_widgets.cpp
  ${CMAKE_SOURCE_DIR}/imgui/imgui_impl_glfw.cpp
  ${CMAKE_SOURCE_DIR}/imgui/imgui_impl_opengl3.cpp
)

# ImGui 정적 라이브러리에 헤더 파일 추가
target_include_directories(imgui PUBLIC
  ${CMAKE_SOURCE_DIR}/imgui
  ${glfw_INCLUDE} # glfw 헤더 파일 경로 추가
  ${CMAKE_SOURCE_DIR}/3rdparty # glad 헤더 파일 경로 추가
)

# 실행 파일 정의
add_executable(${PROJECT_NAME} src/main.cpp)

# 헤더 파일 및 3rd party 디렉토리 추가
target_include_directories(${PROJECT_NAME} PRIVATE
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/3rdparty
  ${assimp_INCLUDE}
)

# src/... 하위의 *.cpp 파일들을 재귀적으로 검색하여 찾기
file(GLOB_RECURSE CPP_SOURCES ${CMAKE_SOURCE_DIR}/src/*.cpp)

# glad.c 소스 파일을 명시하는 변수 설정
set(C_SOURCES ${CMAKE_SOURCE_DIR}/src/glad.c)

# C++ 및 C 소스 파일 목록을 결합하여 변수로 설정
set(SOURCE_FILES ${C_SOURCES} ${CPP_SOURCES})

# ImGui 정적 라이브러리 타겟에 glad 소스 파일 추가
target_sources(imgui PRIVATE ${C_SOURCES})

# ImGui 정적 라이브러리 타겟에 glfw 라이브러리 링크
target_link_libraries(imgui PRIVATE glfw)

# 타겟에 소스 파일 추가
target_sources(${PROJECT_NAME} PRIVATE ${SOURCE_FILES})

# 타겟에 라이브러리 링크
target_link_libraries(${PROJECT_NAME} PRIVATE glfw spdlog imgui assimp)

# 타겟에 매크로 컴파일 정의
target_compile_definitions(${PROJECT_NAME} PRIVATE
  WINDOW_NAME="${WINDOW_NAME}"
  WINDOW_WIDTH=${WINDOW_WIDTH}
  WINDOW_HEIGHT=${WINDOW_HEIGHT}
)
