# Copyright (C) 2023 embeff GmbH
# SPDX-License-Identifier: Apache-2.0
# This file is part of the ExecutionPlatform distribution. More information at https://embeff.com

*** Settings ***
Library           EPLibrary   ${ep_url}    ${ep_binary_dir}/gpio.ep-config.zip
Resource          ${ep_app_dir}/bench/bench.resource
Suite Setup       Suite Setup
Test Tags         app-bench
Resource          ${ep_app_dir}/cmake.resource

Documentation     These tests measure the runtime of  a crc function for two different input arguments

...               These tests show how the ExecutionPlatform can be used to benchmark functions.
...               They gain insights into the runtime behavior.
...               In a productive setting, you would typically ensure in the test that the runtime
...               is within an expected range. If a compiler change or something similar changes
...               the runtime, these tests will then catch a problematic regression.

*** Keywords ***
Suite Setup
    CMake build           ${ep_binary_dir}    example_bench_crc32
    DUT Flash Firmware    ${ep_binary_dir}/example/example_bench_crc32.hex

*** Test Cases ***
Benchmark Cr32 Function With 4 Bytes
    ${runtime_ns}    ${retval} =    Do Benchmark    bench_crc32    abcd
    Log To Console    Function execution time: ${runtime_ns} ns

Benchmark Cr32 Function With 8 Bytes
    ${runtime_ns}    ${retval} =    Do Benchmark    bench_crc32    ABCD1234
    Log To Console    Function execution time: ${runtime_ns} ns
