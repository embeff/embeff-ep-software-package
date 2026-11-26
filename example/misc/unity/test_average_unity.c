// Copyright (C) 2019 embeff GmbH
// SPDX-License-Identifier: Apache-2.0
// This file is part of the ExecutionPlatform distribution. More information at https://embeff.com

// This file is part of the ExecutionPlatform distribution. More information at https://embeff.com/executionplatform/.
// Examples taken from http://www.throwtheswitch.org/unity

#include <unity.h>


int8_t AverageThreeBytes(int8_t a, int8_t b, int8_t c)
{
    return (int8_t)(((int16_t)a + (int16_t)b + (int16_t)c) / 3);
}

void test_AverageThreeBytes_should_AverageMidRangeValues(void)
{
    TEST_ASSERT_EQUAL_HEX8(40, AverageThreeBytes(30, 40, 50));
    TEST_ASSERT_EQUAL_HEX8(40, AverageThreeBytes(10, 70, 40));
    TEST_ASSERT_EQUAL_HEX8(33, AverageThreeBytes(33, 33, 33));
}

void setUp() { }
void tearDown() { }

int run_tests() {
    UNITY_BEGIN();
    RUN_TEST(test_AverageThreeBytes_should_AverageMidRangeValues);
    int testFailureCount = UNITY_END();
    return testFailureCount;
}
