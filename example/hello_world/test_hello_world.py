# Copyright (C) 2026 embeff GmbH
# SPDX-License-Identifier: Apache-2.0
# This file is part of the ExecutionPlatform distribution. More information at https://embeff.com

"""
Demonstrates using the ExecutionPlatform with pytest.

How the tests reach the ExecutionPlatform
- `from gpio.test import EP` imports the class of an ep-config. `EP.fixture()` creates a fixture that opens the EP with this configuration.
- Tests that take this fixture as an argument access the Code Interface (`ep.DUT`) and the endpoints of this configuration.
- Annotate the argument with the same class (`ep: EP`) for code completion in every editor. A fixture that opens another configuration fails the test.

Requirements for test execution and code-completion
- Install the embeff-ep package: `pip install -r requirements.txt`. Its pytest plugin registers itself.
- List every ep-config.zip the tests import in `pyproject.toml`: under `pythonpath` in `[tool.pytest]` (for pytest) and under
  `extraPaths` in `[tool.pyright]` (for code completion with Pylance or pyright). Both list `build/gpio.ep-config.zip`.
- Open `.env.example`, duplicate it to a new `.env` file and adapt variables to your usage.

Known issues
- Almost all arguments are as string. Next version will bring correct typing.
"""

import os

from cmake_helper import cmake_build
import pytest
from gpio.test import EP    # imports gpio.ep-config.zip

BINARY_DIR = os.environ["EP_BINARY_DIR"]

ep = EP.fixture()


@pytest.fixture(scope="module", autouse=True)
def suite_lifecycle(ep: EP):
    cmake_build(BINARY_DIR, "example_hello_world")
    ep.DUT.flash_firmware(f"{BINARY_DIR}/example/example_hello_world.hex")
    yield
    # suite teardown


def test_invoke_square_with_3_gets_9(ep: EP):
    assert ep.DUT.invoke("square", "3") == 9


def test_invoke_square_with_minus_5_gets_25(ep: EP):
    assert ep.DUT.invoke("square", "-5") == 25


def test_call_function_that_prints_mcu_related_output(ep: EP):
    ep.DUT.invoke("print_registers")
    out = ep.DUT.get_output()
    print("\n" + "\n".join(out))


def test_read_and_print_gpio1(ep: EP):
    # assume GPIO endpoint with name "GPIO1" is configured
    level = ep.GPIO1.read()
    print(f"GPIO1: {level}")
