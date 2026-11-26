# Copyright (C) 2026 embeff GmbH
# SPDX-License-Identifier: Apache-2.0
# This file is part of the ExecutionPlatform distribution. More information at https://embeff.com

"""pytest helpers for the unity app, the counterpart of unity.resource.

    from unity.unity import run_unity_tests
"""

import re


def run_unity_tests(ep, expected_failures=0):
    """Run the unity tests on the DUT and return their output.

    Fails if the number of failed tests differs from ``expected_failures``.
    """
    failures = ep.DUT.invoke("run_tests")
    output = "\n".join(ep.DUT.get_output())
    assert failures == expected_failures, (f"Expected {expected_failures} tests to fail but actual count is {failures}. "
                                           f"Unit test output:\n{output}")
    return output


def run_unity_tests_and_compare_output(ep, expected_output, expected_failures=0):
    """Run the unity tests on the DUT and compare their output, ignoring differences in whitespace."""
    output = run_unity_tests(ep, expected_failures)
    assert _collapse_spaces(output) == _collapse_spaces(expected_output), "Test output does not match expected output."


def _collapse_spaces(text):
    return re.sub(r"\s+", " ", text)
