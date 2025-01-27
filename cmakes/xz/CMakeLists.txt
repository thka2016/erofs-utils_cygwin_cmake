# 220724
project(xz C)
set(source_dir ${CMAKE_SOURCE_DIR}/xz-5.3.2alpha)
add_library(lzma
        ${source_dir}/src/common/tuklib_cpucores.c
        ${source_dir}/src/common/tuklib_exit.c
        ${source_dir}/src/common/tuklib_mbstr_fw.c
        ${source_dir}/src/common/tuklib_mbstr_width.c
        ${source_dir}/src/common/tuklib_open_stdxxx.c
        ${source_dir}/src/common/tuklib_physmem.c
        ${source_dir}/src/common/tuklib_progname.c
        ${source_dir}/src/liblzma/check/check.c
        ${source_dir}/src/liblzma/check/crc32_fast.c
        ${source_dir}/src/liblzma/check/crc32_table.c
        ${source_dir}/src/liblzma/check/crc64_fast.c
        ${source_dir}/src/liblzma/check/crc64_table.c
        ${source_dir}/src/liblzma/check/sha256.c
        ${source_dir}/src/liblzma/common/alone_decoder.c
        ${source_dir}/src/liblzma/common/alone_encoder.c
        ${source_dir}/src/liblzma/common/auto_decoder.c
        ${source_dir}/src/liblzma/common/block_buffer_decoder.c
        ${source_dir}/src/liblzma/common/block_buffer_encoder.c
        ${source_dir}/src/liblzma/common/block_decoder.c
        ${source_dir}/src/liblzma/common/block_encoder.c
        ${source_dir}/src/liblzma/common/block_header_decoder.c
        ${source_dir}/src/liblzma/common/block_header_encoder.c
        ${source_dir}/src/liblzma/common/block_util.c
        ${source_dir}/src/liblzma/common/common.c
        ${source_dir}/src/liblzma/common/easy_buffer_encoder.c
        ${source_dir}/src/liblzma/common/easy_decoder_memusage.c
        ${source_dir}/src/liblzma/common/easy_encoder.c
        ${source_dir}/src/liblzma/common/easy_encoder_memusage.c
        ${source_dir}/src/liblzma/common/easy_preset.c
        ${source_dir}/src/liblzma/common/filter_buffer_decoder.c
        ${source_dir}/src/liblzma/common/filter_buffer_encoder.c
        ${source_dir}/src/liblzma/common/filter_common.c
        ${source_dir}/src/liblzma/common/filter_decoder.c
        ${source_dir}/src/liblzma/common/filter_encoder.c
        ${source_dir}/src/liblzma/common/filter_flags_decoder.c
        ${source_dir}/src/liblzma/common/filter_flags_encoder.c
        ${source_dir}/src/liblzma/common/hardware_cputhreads.c
        ${source_dir}/src/liblzma/common/hardware_physmem.c
        ${source_dir}/src/liblzma/common/index.c
        ${source_dir}/src/liblzma/common/index_decoder.c
        ${source_dir}/src/liblzma/common/index_encoder.c
        ${source_dir}/src/liblzma/common/index_hash.c
        ${source_dir}/src/liblzma/common/outqueue.c
        ${source_dir}/src/liblzma/common/stream_buffer_decoder.c
        ${source_dir}/src/liblzma/common/stream_buffer_encoder.c
        ${source_dir}/src/liblzma/common/stream_decoder.c
        ${source_dir}/src/liblzma/common/stream_encoder.c
        ${source_dir}/src/liblzma/common/stream_encoder_mt.c
        ${source_dir}/src/liblzma/common/stream_flags_common.c
        ${source_dir}/src/liblzma/common/stream_flags_decoder.c
        ${source_dir}/src/liblzma/common/stream_flags_encoder.c
        ${source_dir}/src/liblzma/common/vli_decoder.c
        ${source_dir}/src/liblzma/common/vli_encoder.c
        ${source_dir}/src/liblzma/common/vli_size.c
        ${source_dir}/src/liblzma/delta/delta_common.c
        ${source_dir}/src/liblzma/delta/delta_decoder.c
        ${source_dir}/src/liblzma/delta/delta_encoder.c
        ${source_dir}/src/liblzma/lz/lz_decoder.c
        ${source_dir}/src/liblzma/lz/lz_encoder.c
        ${source_dir}/src/liblzma/lz/lz_encoder_mf.c
        ${source_dir}/src/liblzma/lzma/fastpos_table.c
        ${source_dir}/src/liblzma/lzma/fastpos_tablegen.c
        ${source_dir}/src/liblzma/lzma/lzma2_decoder.c
        ${source_dir}/src/liblzma/lzma/lzma2_encoder.c
        ${source_dir}/src/liblzma/lzma/lzma_decoder.c
        ${source_dir}/src/liblzma/lzma/lzma_encoder.c
        ${source_dir}/src/liblzma/lzma/lzma_encoder_optimum_fast.c
        ${source_dir}/src/liblzma/lzma/lzma_encoder_optimum_normal.c
        ${source_dir}/src/liblzma/lzma/lzma_encoder_presets.c
        ${source_dir}/src/liblzma/rangecoder/price_table.c
        ${source_dir}/src/liblzma/rangecoder/price_tablegen.c
        ${source_dir}/src/liblzma/simple/arm.c
        ${source_dir}/src/liblzma/simple/armthumb.c
        ${source_dir}/src/liblzma/simple/ia64.c
        ${source_dir}/src/liblzma/simple/powerpc.c
        ${source_dir}/src/liblzma/simple/simple_coder.c
        ${source_dir}/src/liblzma/simple/simple_decoder.c
        ${source_dir}/src/liblzma/simple/simple_encoder.c
        ${source_dir}/src/liblzma/simple/sparc.c
        ${source_dir}/src/liblzma/simple/x86.c

        ${source_dir}/src/liblzma/common/microlzma_decoder.c
        ${source_dir}/src/liblzma/common/microlzma_encoder.c)
target_compile_options(lzma PRIVATE
        -DTUKLIB_GETTEXT=0
        -DSIZE_MAX=UINT64_MAX
        -DHAVE__BOOL
        -DMYTHREAD_POSIX
        -Wno-implicit-function-declaration
        -DHAVE_MF_HC3
        -DHAVE_MF_HC4
        -DHAVE_MF_BT2
        -DHAVE_MF_BT3
        -DHAVE_MF_BT4)
target_include_directories(lzma PRIVATE
        ${source_dir}/src/common
        ${source_dir}/src/liblzma/api
        ${source_dir}/src/liblzma/check
        ${source_dir}/src/liblzma/common
        ${source_dir}/src/liblzma/delta
        ${source_dir}/src/liblzma/lz
        ${source_dir}/src/liblzma/lzma
        ${source_dir}/src/liblzma/rangecoder
        ${source_dir}/src/liblzma/simple
        ${source_dir}/src/liblzma)