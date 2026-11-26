# Copyright (C) 2021 embeff GmbH
# SPDX-License-Identifier: Apache-2.0
# This file is part of the ExecutionPlatform distribution. More information at https://embeff.com/executionplatform/.

SET(SOURCES
        test_average_gtest.cpp
        )

add_gtest_example (
        NAME test_average_gtest
        SOURCES
            ${SOURCES}
        INCLUDE_DIRECTORIES
            ${CMAKE_CURRENT_SOURCE_DIR}
        )
target_compile_options(test_average_gtest PUBLIC "-O2;-g0")
