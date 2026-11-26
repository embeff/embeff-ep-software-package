# Copyright (C) 2021 embeff GmbH
# SPDX-License-Identifier: Apache-2.0
# This file is part of the ExecutionPlatform distribution. More information at https://embeff.com/executionplatform/.


# If desired, manually control the optimization level for the code that is benchmarked.
# SET_SOURCE_FILES_PROPERTIES(cos.cpp PROPERTIES COMPILE_FLAGS -O0)
# SET_SOURCE_FILES_PROPERTIES(crc32.cpp PROPERTIES COMPILE_FLAGS -O0)

ep_app_example( APP ep_app_bench
                NAME example_bench_crc32
                SOURCES crc32_bench.cpp crc32.cpp)

ep_app_example( APP ep_app_bench
                NAME example_bench_cos
                SOURCES cos_bench.cpp cos.cpp)
