# Copyright (C) 2026 embeff GmbH
# SPDX-License-Identifier: Apache-2.0
# This file is part of the ExecutionPlatform distribution. More information at https://embeff.com

"""These tests measure the runtime of a crc function for two different input arguments.

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
    cmake_build(BINARY_DIR, "example_bench_crc32")
    ep.DUT.flash_firmware(f"{BINARY_DIR}/example/example_bench_crc32.hex")
    yield


@pytest.mark.parametrize("payload", ["abcd", "ABCD1234"])
def test_benchmark_crc32(ep: EP, payload):
    runtime_ns, _ = do_benchmark(ep, "bench_crc32", payload)
    print(f"\nFunction execution time: {runtime_ns} ns")
