# Copyright (C) 2021 embeff GmbH
# SPDX-License-Identifier: Apache-2.0
# This file is part of the ExecutionPlatform distribution. More information at https://embeff.com/executionplatform/.

if (TARGET ep_app_generic)
    add_subdirectory(hello_world)
endif()

if (TARGET ep_app_gtest)
    add_subdirectory(misc/gtest)
endif()

if (TARGET ep_app_unity)
    add_subdirectory(misc/unity)
endif()

if (TARGET ep_app_bench)
    add_subdirectory(misc/bench)
endif()
