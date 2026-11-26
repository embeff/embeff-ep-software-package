# Copyright (C) 2023 embeff GmbH
# SPDX-License-Identifier: Apache-2.0
# This file is part of the ExecutionPlatform distribution. More information at https://embeff.com

*** Settings ***
Library           EPLibrary   ${ep_url}    ${ep_binary_dir}/gpio.ep-config.zip
Resource          ${ep_app_dir}/bench/bench.resource
Suite Setup       Suite Setup
Test Tags         app-bench
Resource          ${ep_app_dir}/cmake.resource

Documentation     These tests measure a cosine function for different input arguments.
...               Running the standard cosine shows that the differs a lot for different angles.
...               In some embedded contexts, a stable execution time is important.
...               To realize that, a cosine alternative (approximation) is also benchmarked.
...               The cos_32 (3.2 digit approx) gives a much more stable runtime.
...
...               If you want to play around with different optimization levels, see CMAkeLists.txt
...
...               These tests show how the ExecutionPlatform can be used to benchmark functions.
...               They gain insights into the runtime behaviour.
...               In a productive setting, you would typically ensure in the test that the runtime
...               is within an expected range. If a compiler change or something similiar changes
...               the runtime, these tests will then catch a problematic regression.

*** Keywords ***
Suite Setup
    CMake build           ${ep_binary_dir}    example_bench_cos
    DUT Flash Firmware    ${ep_binary_dir}/example/example_bench_cos.hex

*** Test Cases ***
Benchmark Standard Cosine
    FOR    ${deg}    IN RANGE    0    360    30
        ${deg_s} =     Convert To String    ${deg}
        ${runtime_ns}    ${retval} =    Do Benchmark    bench_cos    ${deg_s}
        Log Many    ${deg}    ${runtime_ns}
        Log To Console    cos(${deg}°)=${retval} (took ${runtime_ns} ns)
    END


Benchmark Cosine Approximation
    FOR    ${deg}    IN RANGE    0    360    30
        ${deg_s} =     Convert To String    ${deg}
        ${runtime_ns}    ${retval} =    Do Benchmark    bench_cos_32    ${deg_s}
        Log Many    ${deg}    ${runtime_ns}
        Log To Console    cos_32(${deg}°)=${retval} (took ${runtime_ns} ns)
    END
