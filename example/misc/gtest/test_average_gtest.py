# Copyright (C) 2026 embeff GmbH
# SPDX-License-Identifier: Apache-2.0
# This file is part of the ExecutionPlatform distribution. More information at https://embeff.com

import os

from cmake_helper import cmake_build
from gtest.gtest import run_gtest_tests
from gpio.test import EP    # imports gpio.ep-config.zip

BINARY_DIR = os.environ["EP_BINARY_DIR"]

ep = EP.fixture()


def test_run_example_test_case_and_print_results(ep: EP):
    cmake_build(BINARY_DIR, "test_average_gtest")
    ep.DUT.flash_firmware(f"{BINARY_DIR}/example/test_average_gtest.hex")
    output = run_gtest_tests(ep, expect_failures=False)
    print("\n" + output)
