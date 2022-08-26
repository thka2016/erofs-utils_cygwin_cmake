# 220724
project(e2fsprog C)
set(source_dir ${CMAKE_SOURCE_DIR}/e2fsprog)
add_library(ext2_uuid
        ${source_dir}/lib/uuid/clear.c
        ${source_dir}/lib/uuid/compare.c
        ${source_dir}/lib/uuid/copy.c
        ${source_dir}/lib/uuid/gen_uuid.c
        ${source_dir}/lib/uuid/isnull.c
        ${source_dir}/lib/uuid/pack.c
        ${source_dir}/lib/uuid/parse.c
        ${source_dir}/lib/uuid/unpack.c
        ${source_dir}/lib/uuid/unparse.c
        ${source_dir}/lib/uuid/uuid_time.c)
target_include_directories(ext2_uuid PRIVATE
        ${CMAKE_SOURCE_DIR}/include # ext2_uuid clear.c:35:10 fatal error: 'config.h' file not found
        ${source_dir}/include
        ${source_dir}/lib
        ${source_dir}/lib/uuid)
target_compile_definitions(ext2_uuid PRIVATE -DHAVE_INTTYPES_H)

