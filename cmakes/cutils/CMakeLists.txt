# 220724
project(cutils CXX)
set(source_dir ${CMAKE_SOURCE_DIR}/libcutils)
add_library(cutils
        ${source_dir}/android_get_control_file.cpp
        ${source_dir}/config_utils.cpp
        ${source_dir}/canned_fs_config.cpp
        ${source_dir}/iosched_policy.cpp
        ${source_dir}/native_handle.cpp
        ${source_dir}/record_stream.cpp
        ${source_dir}/ashmem-host.cpp
        #${source_dir}/trace-host.cpp
        ${source_dir}/socket_inaddr_any_server_unix.cpp
        ${source_dir}/socket_local_client_unix.cpp
        ${source_dir}/socket_local_server_unix.cpp
        ${source_dir}/socket_network_client_unix.cpp
        ${source_dir}/sockets_unix.cpp
        ${source_dir}/sockets.cpp
        ${source_dir}/fs_config.cpp)
target_include_directories(cutils PRIVATE
        ${source_dir}/include

        ${CMAKE_SOURCE_DIR}/base/include
        ${CMAKE_SOURCE_DIR}/liblog/include)
target_compile_options(cutils PRIVATE -fno-exceptions) # libcutils/fs_config.cpp undefined reference to `__imp___cxa_begin_catch'
