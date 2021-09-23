# 添加第三方依赖包
include(FetchContent)
cmake_minimum_required(VERSION 3.14)

set(DEP_NAME  srt)
set(${DEP_NAME}_GIT_TAG  v1.4.3)  # 指定版本
set(${DEP_NAME}_GIT_URL  https://github.com/Haivision/srt.git)  # 指定git仓库地址

FetchContent_Declare(
  ${DEP_NAME}
  GIT_REPOSITORY    ${${DEP_NAME}_GIT_URL}
  GIT_TAG           ${${DEP_NAME}_GIT_TAG}
)

include_directories(${CMAKE_BINARY_DIR}/_deps/${DEP_NAME}-src/srtcore)

FetchContent_MakeAvailable(${DEP_NAME})
