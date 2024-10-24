# glfw

FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG 3.3.2
)

FetchContent_MakeAvailable(glfw)

# glfw 헤더 파일 경로 변수 설정
set(glfw_INCLUDE ${glfw_SOURCE_DIR}/include)
