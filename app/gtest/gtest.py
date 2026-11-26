# Copyright (C) 2026 embeff GmbH
# SPDX-License-Identifier: Apache-2.0
# This file is part of the ExecutionPlatform distribution. More information at https://embeff.com

"""pytest helpers for the gtest app, the counterpart of gtest.resource.

    from gtest.gtest import run_gtest_tests
"""

import re


def run_gtest_tests(ep, expect_failures=False):
    """Run the gtest tests on the DUT and return their output.

    gtest only reports whether any test failed. Fails if that differs from ``expect_failures``.
    """
    result = ep.DUT.invoke("run_tests")
    output = "\n".join(ep.DUT.get_output())
    if expect_failures:
        assert result, f"Expecting failures but gtest return code is 0 (indicating no failed tests). Unit test output:\n{output}"
    else:
        assert not result, ("Expecting no failures but gtest return code is 1 (indicating any number of failures). "
                            f"Unit test output:\n{output}")
    return output


def run_gtest_tests_and_compare_output(ep, expected_output, expect_failures=False):
    """Run the gtest tests on the DUT and compare their output, ignoring differences in whitespace."""
    output = run_gtest_tests(ep, expect_failures)
    assert _collapse_spaces(output) == _collapse_spaces(expected_output), "Test output does not match expected output."


def _collapse_spaces(text):
    return re.sub(r"\s+", " ", text)
