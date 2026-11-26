# Copyright (C) 2022 embeff GmbH
# SPDX-License-Identifier: Apache-2.0
# This file is part of the ExecutionPlatform distribution. More information at https://embeff.com

target_sources(target_bsp PRIVATE ep_runtime.c)
target_include_directories(target_bsp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
