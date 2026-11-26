# Copyright (C) 2024 embeff GmbH
# SPDX-License-Identifier: Apache-2.0
# This file is part of the ExecutionPlatform distribution. More information at https://embeff.com/executionplatform/.

set(EP_APP_UNITY_VENDOR_DIR "../../third_party/Unity" CACHE STRING "Path to unmodified unity sources.")
set(EP_APP_UNITY_CONFIG_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include" CACHE STRING "Path to directory containing unity_config.h.")

add_library(unity STATIC ${EP_APP_UNITY_VENDOR_DIR}/src/unity.c)
target_include_directories(unity PUBLIC ${EP_APP_UNITY_VENDOR_DIR}/src)
target_compile_definitions(unity PUBLIC "UNITY_INCLUDE_CONFIG_H")
target_include_directories(unity PUBLIC ${EP_APP_UNITY_CONFIG_DIR})
target_link_libraries(unity PRIVATE target_bsp)

ep_app_create(ep_app_unity ep_app.c)
ep_app_link(ep_app_unity unity)
