# 220724
cmake_minimum_required(VERSION 3.20)
set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)
project(erofs_utils_cygwin_cmake C CXX)

set(CMAKE_CXX_STANDARD 17)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
#set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin/lib)

add_subdirectory(pcre-8.45)
add_subdirectory(cmakes/selinux)
add_subdirectory(cmakes/e2fsprog)
add_subdirectory(cmakes/cutils)
#add_subdirectory(libcutils)
add_subdirectory(cmakes/lz4)
add_subdirectory(cmakes/xz)
add_subdirectory(cmakes/base) # strings.cpp 49-83, 87-90
add_subdirectory(cmakes/erofs-utils)

add_custom_target(erofs)
add_dependencies(erofs mkfs.erofs dump.erofs fsck.erofs)
