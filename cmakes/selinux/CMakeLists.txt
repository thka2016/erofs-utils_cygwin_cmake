# 220724
project(selinux C)
set(source_dir ${CMAKE_SOURCE_DIR}/libselinux)
add_library(selinux
        ${source_dir}/src/booleans.c
        ${source_dir}/src/canonicalize_context.c
        ${source_dir}/src/disable.c
        ${source_dir}/src/enabled.c
        ${source_dir}/src/fgetfilecon.c
        ${source_dir}/src/fsetfilecon.c
        ${source_dir}/src/getenforce.c
        ${source_dir}/src/getfilecon.c
        ${source_dir}/src/getpeercon.c
        ${source_dir}/src/lgetfilecon.c
        ${source_dir}/src/load_policy.c
        ${source_dir}/src/lsetfilecon.c
        ${source_dir}/src/policyvers.c
        ${source_dir}/src/procattr.c
        ${source_dir}/src/setenforce.c
        ${source_dir}/src/setfilecon.c
        ${source_dir}/src/context.c
        ${source_dir}/src/mapping.c
        ${source_dir}/src/stringrep.c
        ${source_dir}/src/compute_create.c
        ${source_dir}/src/compute_av.c
        ${source_dir}/src/avc.c
        ${source_dir}/src/avc_sidtab.c
        ${source_dir}/src/get_initial_context.c
        ${source_dir}/src/sestatus.c
        ${source_dir}/src/deny_unknown.c
        ${source_dir}/src/callbacks.c
        ${source_dir}/src/check_context.c
        ${source_dir}/src/freecon.c
        ${source_dir}/src/init.c
        ${source_dir}/src/label.c
        ${source_dir}/src/label_file.c
        ${source_dir}/src/label_android_property.c
        ${source_dir}/src/label_support.c)
target_include_directories(selinux PRIVATE ${source_dir}/include ${CMAKE_SOURCE_DIR}/pcre-8.45)
