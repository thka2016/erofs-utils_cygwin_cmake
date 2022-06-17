# Makefile by affggh
# fuse.erofs not suite on cygwin
# this binary will not be genrate
override CC = clang
override CXX = clang++
override STRIP = strip

override CFLAGS := $(CFLAGS) \
    -Wall \
    -Werror \
    -Wno-ignored-qualifiers \
    -Wno-pointer-arith \
    -Wno-unused-parameter \
    -Wno-unused-function \
	-Wno-unused-variable \
    -DHAVE_LIBSELINUX \
    -DHAVE_LIBUUID \
    -DLZ4_ENABLED \
    -DLZ4HC_ENABLED \
  	-DHAVE_LIBLZMA \
    -DWITH_ANDROID
override CXXFLAGS := $(CXXFLAGS) -std=c++17 -stdlib=libc++ \
    -Wall \
    -Werror \
    -Wno-ignored-qualifiers \
    -Wno-pointer-arith \
    -Wno-unused-parameter \
    -Wno-unused-function \
    -DHAVE_LIBSELINUX \
    -DHAVE_LIBUUID \
    -DLZ4_ENABLED \
    -DLZ4HC_ENABLED \
  	-DHAVE_LIBLZMA \
    -DWITH_ANDROID
override LDFLAGS := $(LDFLAGS) -L/usr/local/lib -llz4 -lpcre -llzma

SHELL = bash

INCLUDES = \
    -I./include \
    -I./include/erofs \
	-I./libselinux/include \
	-I./base/include \
	-I./libcutils/include \
	-I./e2fsprog/lib/uuid

.PHONY: all

all: check erofs-utils-version.h bin/mkfs.erofs.exe bin/dump.erofs.exe bin/fsck.erofs.exe
	@for i in $(shell for i in $$(cygcheck ./bin/mkfs.erofs.exe); do for j in $$(echo $$i | grep "cyg" | grep ".dll"); do cygpath $$j;done ;done); do \
    echo -e "\033[95m\tCOPY\t$$i\033[0m"; \
    cp -f $$i ./bin/; \
    done


USECUSTOM_VERSION = true
PACKAGE_VERSION = $(shell sed -n '1p' VERSION | tr -d '\n')

INCLUDES += -include"erofs-utils-version.h"

AUTHOR = "affggh"
ifeq ($(shell uname -o), Cygwin)
  BUILD = "cygwin"
  ifeq ($(shell uname -m), x86_64)
    BUILD = "cygwin64"
  endif
endif

check:
ifneq ($(shell lzma --version | sed -n '2p' | cut -d ' ' -f2), 5.3.2alpha)  # Check installed liblzma version via lzma command
	@echo -e "\033[91m  xz-utils not installed or liblzma version less than 5.3.2alpha  \033[0m"
	@exit 1
else
	@echo -e "\033[92m  xz-utils check pass  \033[0m"
endif
ifeq ($(shell [ "$(shell lz4 --version | cut -d ' ' -f7 | tr -d ',' | cut -d '.' -f2)" -ge "8" ];echo $$?), 0) # Same method check lz4 version via lz4 command
	@echo -e "\033[92m  lz4 check pass  \033[0m"
else
	@echo -e "\033[91m  liblz4 is too old to use or liblz4 not installed \033[0m"
	@exit 1
endif

LIBEROFS_SRC = \
	lib/dir.c \
	lib/sha256.c \
    lib/blobchunk.c \
	lib/compressor_liblzma.c \
	lib/exclude.c \
	lib/super.c \
    lib/block_list.c \
	lib/compressor_lz4.c \
	lib/hashmap.c \
	lib/xattr.c \
	lib/cache.c \
	lib/compressor_lz4hc.c \
	lib/inode.c \
	lib/zmap.c \
	lib/compress.c \
	lib/config.c \
	lib/io.c \
	lib/compress_hints.c \
	lib/data.c \
    lib/compressor.c \
	lib/decompress.c \
	lib/namei.c
LIBEROFS_OBJ = $(patsubst %.c,obj/%.o,$(LIBEROFS_SRC))

MKFS_SRC = mkfs/main.c
MKFS_OBJ = $(patsubst %.c,obj/%.o,$(MKFS_SRC))

DUMP_SRC = dump/main.c
DUMP_OBJ = $(patsubst %.c,obj/%.o,$(DUMP_SRC))

FSCK_SRC = fsck/main.c
FSCK_OBJ = $(patsubst %.c,obj/%.o,$(FSCK_SRC))

#  echo -e "\033[94mtest\033[0m"

obj/%.o: %.c
	@mkdir -p `dirname $@`
	@echo -e "\033[94m\tCC\t$@\033[0m"
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

obj/%.o: %.cc
	@mkdir -p `dirname $@`
	@echo -e "\033[94m\tCPP\t$@\033[0m"
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -fno-exceptions -c $< -o $@

obj/%.o: %.cpp
	@mkdir -p `dirname $@`
	@echo -e "\033[94m\tCPP\t$@\033[0m"
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

erofs-utils-version.h: VERSION
	@echo -e "\033[94m\tGEN\t$@\033[0m"
ifeq ($(USECUSTOM_VERSION), true)
	@echo "#define PACKAGE_VERSION \"$(PACKAGE_VERSION)-$(AUTHOR)-$(BUILD)\"" > $@
else
	@echo "#define PACKAGE_VERSION \"$(PACKAGE_VERSION)-dirty\"" > $@
endif

.lib/liberofs.a: erofs-utils-version.h $(LIBEROFS_OBJ)
	@mkdir -p `dirname $@`
	@echo -e "\033[94m\tAR\t$@\033[0m"
	@$(AR) rcs $@ $^

libselinux/.lib/libselinux.a:
	@cd libselinux && $(MAKE)

base/.lib/libbase.a:
	@cd base && $(MAKE)

liblog/.lib/liblog.a:
	@cd liblog && $(MAKE)

libcutils/.lib/libcutils.a:
	@cd libcutils && $(MAKE)

e2fsprog/.lib/libext2_uuid.a:
	@cd e2fsprog && $(MAKE)

bin/mkfs.erofs.exe: $(MKFS_OBJ) \
    .lib/liberofs.a \
    libcutils/.lib/libcutils.a \
    e2fsprog/.lib/libext2_uuid.a \
    liblog/.lib/liblog.a \
    libselinux/.lib/libselinux.a \
    base/.lib/libbase.a 
	@mkdir -p bin
	@echo -e "\033[95m\tLD\t$@\033[0m"
	@$(CXX) $(CXXFLAGS) $(INCLUDES) $^ -o $@ $(LDFLAGS)
	@$(STRIP) --strip-all $@

bin/dump.erofs.exe: $(DUMP_OBJ) \
    .lib/liberofs.a \
    libcutils/.lib/libcutils.a \
    e2fsprog/.lib/libext2_uuid.a \
    liblog/.lib/liblog.a \
    libselinux/.lib/libselinux.a \
    base/.lib/libbase.a 
	@mkdir -p bin
	@echo -e "\033[95m\tLD\t$@\033[0m"
	@$(CXX) $(CXXFLAGS) $(INCLUDES) $^ -o $@ $(LDFLAGS)
	@$(STRIP) --strip-all $@

bin/fsck.erofs.exe: $(FSCK_OBJ) \
    .lib/liberofs.a \
    libcutils/.lib/libcutils.a \
    e2fsprog/.lib/libext2_uuid.a \
    liblog/.lib/liblog.a \
    libselinux/.lib/libselinux.a \
    base/.lib/libbase.a 
	@mkdir -p bin
	@echo -e "\033[95m\tLD\t$@\033[0m"
	@$(CXX) $(CXXFLAGS) $(INCLUDES) $^ -o $@ $(LDFLAGS)
	@$(STRIP) --strip-all $@

clean:
ifeq ($(shell [[ -f "erofs-utils-version.h" ]];echo $$?), 0)
	@echo -e "\033[94m\tRM\terofs-utils-version.h\033[0m"
	@rm -f "erofs-utils-version.h"
endif
ifeq ($(shell [[ -d "obj" ]];echo $$?), 0)
	@echo -e "\033[94m\tRM\tobj\033[0m"
	@rm -rf obj
endif
ifeq ($(shell [[ -d ".lib" ]];echo $$?), 0)
	@echo -e "\033[94m\tRM\t.lib\033[0m"
	@rm -rf .lib
endif
ifeq ($(shell [[ -d "bin" ]];echo $$?), 0)
	@echo -e "\033[94m\tRM\tbin\033[0m"
	@rm -rf bin
endif
	@cd libselinux && $(MAKE) clean
	@cd liblog && $(MAKE) clean
	@cd libcutils && $(MAKE) clean
	@cd e2fsprog && $(MAKE) clean
	@cd base && $(MAKE) clean