# assimp

# 현재 BUILD_SHARED_LIBS 값을 백업
set(BACKUP_BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS})

FetchContent_Declare(
  assimp
  GIT_REPOSITORY https://github.com/assimp/assimp.git
  GIT_TAG v5.0.1
)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INJECT_DEBUG_POSTFIX OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(assimp)

# BUILD_SHARED_LIBS 값을 원래대로 복구
set(BUILD_SHARED_LIBS ${BACKUP_BUILD_SHARED_LIBS} CACHE BOOL "" FORCE)

# assimp 헤더 파일 경로 변수 설정
set(assimp_INCLUDE ${assimp_SOURCE_DIR}/include)
