cmake_minimum_required(VERSION 3.4.1)
project(pusher)

ENABLE_LANGUAGE(ASM)

set(PLATFORM linux)
set(ABI x64)

set(LIBS_DIR ${CMAKE_SOURCE_DIR}/libs/)
set(LIBS_OS_DIR ${LIBS_DIR}/${PLATFORM}/${ABI}/) 
set(LIBS_INCLUDE_DIR ${LIBS_DIR}/include/) 
set(LIBS_SOURCES_DIR ${LIBS_DIR}/sources/) 

set(FASTEST_FLAG "-Ofast -Os -Oz ")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${FASTEST_FLAG}")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++${CMAKE_CXX_STANDARD} ${FASTEST_FLAG} ")

macro(add_dynamic_lib LIB_NAME)
    add_library(${LIB_NAME} SHARED IMPORTED)
    set_target_properties(${LIB_NAME} PROPERTIES IMPORTED_LOCATION ${LIBS_OS_DIR}/${LIB_NAME}.so)
    message("Add dynamic library: ${LIB_NAME}")
endmacro()

macro(add_static_lib LIB_NAME)
    add_library(${LIB_NAME} STATIC IMPORTED)
    set_target_properties(${LIB_NAME} PROPERTIES IMPORTED_LOCATION ${LIBS_OS_DIR}/${LIB_NAME}.a)
    message("Add static library: ${LIB_NAME}")
endmacro()

macro(add_lib_source SOURCE_DIR LIB_EXT)
    string(REPLACE " " "_" LIB_NAME ${SOURCE_DIR})
    add_subdirectory(${LIBS_SOURCES_DIR}/${SOURCE_DIR} ${LIBS_SOURCES_DIR}/output/${SOURCE_DIR})
    set_target_properties(lib${LIB_NAME} PROPERTIES IMPORTED_LOCATION ${LIBS_SOURCES_DIR}/output/${SOURCE_DIR}/lib${LIB_NAME}.${LIB_EXT})
endmacro()

macro(add_static_lib_source SOURCE_DIR)
    set(BUILD_SHARED_LIBS OFF)
    set(BUILD_STATIC_LIBS ON)

    add_lib_source(${SOURCE_DIR} "a")

    set(BUILD_SHARED_LIBS OFF)
    set(BUILD_STATIC_LIBS OFF)
endmacro()

macro(add_dynamic_lib_source SOURCE_DIR)
    set(BUILD_SHARED_LIBS ON)
    set(BUILD_STATIC_LIBS OFF)

    add_lib_source(${SOURCE_DIR} "so")

    set(BUILD_SHARED_LIBS OFF)
    set(BUILD_STATIC_LIBS OFF)
endmacro()

macro(include_directory DIR_PATH)
    include_directories(${DIR_PATH})
    message("Include Directory: ${DIR_PATH}")
endmacro()


include_directory(${CMAKE_SOURCE_DIR})
include_directory(${LIBS_INCLUDE_DIR})
include_directory(${LIBS_SOURCES_DIR})

file(GLOB_RECURSE NATIVE_SOURCES ./native/*.cpp)

#aux_source_directory(./*.cpp CPP_SOURCES)

add_library(
    ${PROJECT_NAME} SHARED
    ${NATIVE_SOURCES}
)

# add_dynamic_lib(libsrt)
# add_static_lib(libsrt)
# add_static_lib(libssl)
# add_static_lib(libcrypto)
# find_library(EGL-lib EGL)

add_static_lib_source(sigslot)

target_link_libraries(
        ${PROJECT_NAME}
        
        libsigslot
        
)

add_executable(Test_Push  ${CMAKE_SOURCE_DIR}/test/test_push.cpp)
target_link_libraries(Test_Push  PRIVATE ${PROJECT_NAME})
