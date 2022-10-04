# 220724
project(erofs-utils C)
set(source_dir ${CMAKE_SOURCE_DIR}/erofs-utils_cygwin)
file(STRINGS ${source_dir}/VERSION version LIMIT_COUNT 1)
#message("erofs-utils version: ${version}")

if (CMAKE_SYSTEM_NAME MATCHES "Linux" OR ANDROID)
    add_definitions(
            -DHAVE_LINUX_TYPES_H
            -DHAVE_SYS_SYSMACROS_H
    )
endif ()
add_definitions(
        -DPACKAGE_VERSION="${version}"
        #-DHAVE_FALLOCATE
        -DHAVE_LIBUUID
        -DWITH_ANDROID
        -DHAVE_LIBSELINUX
        -DLZ4HC_ENABLED
        -DLZ4_ENABLED

        -DHAVE_LIBLZMA
        -DFUSE_USE_VERSION=26
)
add_library(erofs-utils
        ${source_dir}/lib/cache.c
        ${source_dir}/lib/compress.c
        ${source_dir}/lib/compressor.c
        ${source_dir}/lib/compressor_lz4.c
        ${source_dir}/lib/compressor_lz4hc.c
        ${source_dir}/lib/config.c
        ${source_dir}/lib/data.c
        ${source_dir}/lib/decompress.c
        ${source_dir}/lib/exclude.c
        ${source_dir}/lib/inode.c
        ${source_dir}/lib/io.c
        ${source_dir}/lib/namei.c
        ${source_dir}/lib/super.c
        ${source_dir}/lib/xattr.c
        ${source_dir}/lib/zmap.c

        ${source_dir}/lib/blobchunk.c
        ${source_dir}/lib/block_list.c
        ${source_dir}/lib/compress_hints.c
        ${source_dir}/lib/compressor_liblzma.c
        ${source_dir}/lib/exclude.c
        ${source_dir}/lib/hashmap.c
        ${source_dir}/lib/sha256.c

        ${source_dir}/lib/dir.c)
target_include_directories(erofs-utils PRIVATE
        ${erofs-utils_inc}
        ${erofs-utils_inc}/erofs)


add_link_options(-static -s)
#add_link_options(-static -g)
include_directories(
        ${source_dir}/include
        ${source_dir}/include/erofs

        ${CMAKE_SOURCE_DIR}/libselinux/include
        ${CMAKE_SOURCE_DIR}/libcutils/include
        ${CMAKE_SOURCE_DIR}/e2fsprog/lib/uuid
        ${CMAKE_SOURCE_DIR}/xz-5.3.2alpha/src/liblzma/api
        ${CMAKE_SOURCE_DIR}/lz4-1.9.3/lib
)
set(erofs-utils_libs erofs-utils ext2_uuid selinux lz4 lzma pcre cutils base)

add_executable(mkfs.erofs ${source_dir}/mkfs/main.c)
target_link_libraries(mkfs.erofs ${erofs-utils_libs})

#add_executable(erofsfuse ${source_dir}/fuse/main.c)
#target_link_libraries(erofsfuse  ${erofs-utils_libs} fuse)
#target_include_directories(erofsfuse PRIVATE libfuse/include)

add_executable(dump.erofs ${source_dir}/dump/main.c)
target_link_libraries(dump.erofs erofs-utils ${erofs-utils_libs})

add_executable(fsck.erofs ${source_dir}/fsck/main.c)
target_link_libraries(fsck.erofs  ${erofs-utils_libs})

