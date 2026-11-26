# Copyright (C) 2021 embeff GmbH
# SPDX-License-Identifier: Apache-2.0
# This file is part of the ExecutionPlatform distribution. More information at https://embeff.com/executionplatform/.

ep_app_create(ep_app_bench ep_app.cpp)

target_link_libraries(ep_app_bench INTERFACE target_bsp)
target_include_directories(ep_app_bench INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Important, otherwise benchmark results are misleading
target_compile_options(ep_app_bench INTERFACE "-O2")
