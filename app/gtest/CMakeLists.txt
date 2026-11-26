# Copyright (C) 2021 embeff GmbH
# SPDX-License-Identifier: Apache-2.0
# This file is part of the ExecutionPlatform distribution. More information at https://embeff.com/executionplatform/.

set(EP_APP_GTEST_VENDOR_DIR "../../third_party/googletest" CACHE STRING "Path to unmodified googletest sources.")

SET(gtest_disable_pthreads ON CACHE BOOL "Disable pthreads for arm" FORCE)
SET(INSTALL_GTEST OFF CACHE BOOL "Enable installation of googletest." FORCE)
add_subdirectory(${EP_APP_GTEST_VENDOR_DIR} ${CMAKE_CURRENT_BINARY_DIR}/googletest-vendor EXCLUDE_FROM_ALL)

target_compile_definitions(gtest PUBLIC -DGTEST_HAS_STREAM_REDIRECTION=0 -DGTEST_HAS_POSIX_RE=0 -DGTEST_HAS_CLONE=0 -DGTEST_HAS_EXCEPTIONS=0 -DGTEST_HAS_RTTI=0)
target_compile_options(gtest PRIVATE -Wno-psabi -Os -g0)
target_link_libraries(gtest PRIVATE target_bsp)
set_target_properties(gtest PROPERTIES CXX_EXTENSIONS ON)
set_target_properties(gmock PROPERTIES CXX_EXTENSIONS ON)

ep_app_create(ep_app_gtest ep_app.cpp)
ep_app_link(ep_app_gtest "gtest;gmock")
target_compile_options(ep_app_gtest INTERFACE "-Os")
