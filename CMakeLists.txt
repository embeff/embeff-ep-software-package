# Copyright (C) 2025 embeff GmbH
# SPDX-License-Identifier: Apache-2.0
# This file is part of the ExecutionPlatform distribution. More information at https://embeff.com/executionplatform/.

cmake_minimum_required(VERSION 3.23)
project(ep-target LANGUAGES C CXX ASM)
cmake_policy(SET CMP0048 NEW)
cmake_policy(SET CMP0076 NEW)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")

include("cmake/BuildHelper.cmake")
include("cmake/OpenModelicaHelper.cmake")

if (NOT DEFINED BSP_DIR)
    message(FATAL_ERROR "You need to specify the directory containing the bsp to be used with -DBSP_DIR=<Path>")
endif()
message("Selected BSP ${BSP_DIR}")

add_subdirectory(${BSP_DIR} ${CMAKE_CURRENT_BINARY_DIR}/bsp)
add_subdirectory(src)

get_target_property(EP_BSP_APPS_TESTED target_bsp EP_BSP_APPS_TESTED)
message("Supported apps: ${EP_BSP_APPS_TESTED}")
foreach(EP_APP  ${EP_BSP_APPS_TESTED})
    message("- Enabling support for app ${EP_APP}")
    add_subdirectory(app/${EP_APP})
endforeach()
get_target_property(EP_BSP_META_INFO target_bsp EP_BSP_META_INFO)
configure_file(ep_build_info.yaml.in ep_build_info.yaml)

add_subdirectory(example)

if (INCLUDE_TESTS)
    add_subdirectory(tests)
endif()
