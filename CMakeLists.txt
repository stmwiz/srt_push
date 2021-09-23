cmake_minimum_required(VERSION 3.14)
project(pusher)
ENABLE_LANGUAGE(ASM)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")

include(FindPkgConfig)
# XXX See 'if (MINGW)' condition below, may need fixing.
include(FindThreads)
include(CheckFunctionExists)
include(haiUtil)

# Platform shortcuts
string(TOLOWER      ${CMAKE_SYSTEM_NAME} SYSNAME_LC)
set_if(DARWIN	   (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
					OR (${CMAKE_SYSTEM_NAME} MATCHES "iOS")
					OR (${CMAKE_SYSTEM_NAME} MATCHES "tvOS")
					OR (${CMAKE_SYSTEM_NAME} MATCHES "watchOS"))
set_if(LINUX       ${CMAKE_SYSTEM_NAME} MATCHES "Linux")
set_if(BSD         ${SYSNAME_LC} MATCHES "bsd$")
set_if(MICROSOFT   WIN32 AND (NOT MINGW AND NOT CYGWIN))
set_if(GNU         ${CMAKE_SYSTEM_NAME} MATCHES "GNU")
set_if(ANDROID     ${SYSNAME_LC} MATCHES "android")
set_if(SUNOS       "${SYSNAME_LC}" MATCHES "sunos")
set_if(POSIX       LINUX OR DARWIN OR BSD OR SUNOS OR ANDROID OR (CYGWIN AND CYGWIN_USE_POSIX))
set_if(SYMLINKABLE LINUX OR DARWIN OR BSD OR SUNOS OR CYGWIN OR GNU)


if(POSIX)
set(DYNAMIC_LIB_SUFFIX .so)
set(STATIC_LIB_SUFFIX .a)
else()
set(DYNAMIC_LIB_SUFFIX .dll)
set(STATIC_LIB_SUFFIX .lib)
endif(POSIX)

set(ABI x64)
if(LINUX)
set(OS linux)
elseif(ANDROID) 
set(OS android)
elseif(WIN32)
set(OS win32)
elseif(DARWIN])
set(OS mac)
endif(LINUX)

set(LIBS_DIR ${CMAKE_SOURCE_DIR}/libs/)
set(LIBS_OS_DIR ${LIBS_DIR}/${OS}/${ABI}/) 
set(LIBS_INCLUDE_DIR ${LIBS_DIR}/include/) 
set(LIBS_SOURCES_DIR ${LIBS_DIR}/sources/) 

set(FASTEST_FLAG "-Ofast -Os -Oz ")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${FASTEST_FLAG}")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++${CMAKE_CXX_STANDARD} ${FASTEST_FLAG} -fPIC")


macro(add_dynamic_lib LIB_NAME)
    add_library(${LIB_NAME} SHARED IMPORTED)
    set_target_properties(${LIB_NAME} PROPERTIES IMPORTED_LOCATION ${LIBS_OS_DIR}/${LIB_NAME}${DYNAMIC_LIB_SUFFIX})
    message("Add dynamic library: ${LIB_NAME}")
endmacro()

macro(add_static_lib LIB_NAME)
    add_library(${LIB_NAME} STATIC IMPORTED)
    set_target_properties(${LIB_NAME} PROPERTIES IMPORTED_LOCATION ${LIBS_OS_DIR}/${LIB_NAME}${STATIC_LIB_SUFFIX})
    message("Add static library: ${LIB_NAME}")
endmacro()

macro(add_lib_source SOURCE_DIR LIB_EXT)
    string(REPLACE " " "_" LIB_NAME ${SOURCE_DIR})
    add_subdirectory(${LIBS_SOURCES_DIR}/${SOURCE_DIR} ${LIBS_SOURCES_DIR}/output/${SOURCE_DIR})
    set_target_properties(lib${LIB_NAME} PROPERTIES IMPORTED_LOCATION ${LIBS_SOURCES_DIR}/output/${SOURCE_DIR}/lib${LIB_NAME}${LIB_EXT})
endmacro()

macro(add_static_lib_source SOURCE_DIR)
    set(BUILD_SHARED_LIBS OFF)
    set(BUILD_STATIC_LIBS ON)

    add_lib_source(${SOURCE_DIR} ${STATIC_LIB_SUFFIX})

    set(BUILD_SHARED_LIBS OFF)
    set(BUILD_STATIC_LIBS OFF)
endmacro()

macro(add_dynamic_lib_source SOURCE_DIR)
    set(BUILD_SHARED_LIBS ON)
    set(BUILD_STATIC_LIBS OFF)

    add_lib_source(${SOURCE_DIR} ${DYNAMIC_LIB_SUFFIX})

    set(BUILD_SHARED_LIBS OFF)
    set(BUILD_STATIC_LIBS OFF)
endmacro()

macro(include_directory DIR_PATH)
    include_directories(${DIR_PATH})
    message("Include Directory: ${DIR_PATH}")
endmacro()

macro(target_link_add_static_lib_source SOURCE_DIR)
add_static_lib_source(${SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} lib${SOURCE_DIR})
endmacro()

macro(target_link_add_dynamic_lib_source SOURCE_DIR)
add_static_lib_source(${SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} lib${SOURCE_DIR})
endmacro()

macro(target_link_add_executable EXE_NAME SOURCE_PATH)
add_executable(${EXE_NAME}  ${SOURCE_PATH})
target_link_libraries(${EXE_NAME} PRIVATE ${PROJECT_NAME})
endmacro()

macro(target_link_add_moudle_lib MOUD_NAME)
include(${MOUD_NAME})
target_link_libraries(${PROJECT_NAME} ${MOUD_NAME})
endmacro()


set(NATIVE_DIR ${CMAKE_SOURCE_DIR}/native/) 

include_directory(${CMAKE_SOURCE_DIR})
include_directory(${LIBS_INCLUDE_DIR})
include_directory(${LIBS_SOURCES_DIR})
include_directory(${NATIVE_DIR})

file(GLOB_RECURSE NATIVE_SOURCES ${NATIVE_DIR}/*.cpp)

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

#target_link_add_static_lib_source(sigslot)

if(ANDROID)
    target_link_libraries(${PROJECT_NAME}  PRIVATE -landroid -llog)
endif()

if(ANDROID OR UNIX ) 
target_link_add_moudle_lib(jmi)
endif()
target_link_add_moudle_lib(spdlog)
target_link_add_moudle_lib(srt)
target_link_add_moudle_lib(sigslot)

set(TEST_DIR ${CMAKE_SOURCE_DIR}/test/) 
target_link_add_executable(Test_Push ${TEST_DIR}/test_push.cpp)



