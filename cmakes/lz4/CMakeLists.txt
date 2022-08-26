# 220724
project(lz4 C)
set(source_dir ${CMAKE_SOURCE_DIR}/lz4-1.9.3)
add_library(lz4
        ${source_dir}/lib/lz4.c
        ${source_dir}/lib/lz4hc.c
        ${source_dir}/lib/xxhash.c)