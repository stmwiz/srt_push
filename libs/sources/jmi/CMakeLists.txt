
# 加入REQUIRED 参数,如果找不到JNI,就报错退出
find_package(JNI REQUIRED)
# 加入jni支持
include_directories(${JAVA_INCLUDE_PATH})
include_directories(${JAVA_INCLUDE_PATH2})

get_filename_component(LIB_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
string(REPLACE " " "_" LIB_NAME ${LIB_NAME})

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} ${LIB_NAME}_SOURCE_CODE)

set(LIB_TYPE "")

if (BUILD_STATIC_LIBS)
    set(LIB_TYPE STATIC)
endif ()

if (BUILD_SHARED_LIBS)
    set(LIB_TYPE SHARED)
endif ()

add_library(lib${LIB_NAME} ${LIB_TYPE} ${${LIB_NAME}_SOURCE_CODE})
set_target_properties(lib${LIB_NAME} PROPERTIES OUTPUT_NAME ${LIB_NAME})
