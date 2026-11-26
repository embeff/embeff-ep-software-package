# Copyright (C) 2026 embeff GmbH
# SPDX-License-Identifier: Apache-2.0
# This file is part of the ExecutionPlatform distribution. More information at https://embeff.com

"""These tests measure a cosine function for different input arguments.

Running the standard cosine shows that the runtime differs a lot for different angles.
In some embedded contexts, a stable execution time is important. To realize that,
a cosine approximation is also benchmarked: cos_32 (3.2 digit approx) gives a much more
stable runtime. To try different optimization levels, see CMakeLists.txt.

In a productive setting, you would typically ensure in the test that the runtime is within
an expected range, so that a compiler change or similar that changes the runtime is caught.
"""

import os

import pytest
from cmake_helper import cmake_build
from bench.bench import do_benchmark
from gpio.test import EP    # imports gpio.ep-config.zip

BINARY_DIR = os.environ["EP_BINARY_DIR"]

ep = EP.fixture()


@pytest.fixture(scope="module", autouse=True)
def suite_lifecycle(ep: EP):
    cmake_build(BINARY_DIR, "example_bench_cos")
    ep.DUT.flash_firmware(f"{BINARY_DIR}/example/example_bench_cos.hex")
    yield


@pytest.mark.parametrize("function", ["bench_cos", "bench_cos_32"])
def test_benchmark_cosine(ep: EP, function):
    for deg in range(0, 360, 30):
        runtime_ns, retval = do_benchmark(ep, function, str(deg))
        print(f"\n{function}({deg}°)={retval} (took {runtime_ns} ns)")
