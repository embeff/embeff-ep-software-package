// Copyright (C) 2021 embeff GmbH
// SPDX-License-Identifier: Apache-2.0
// This file is part of the ExecutionPlatform distribution. More information at https://embeff.com/executionplatform/.

#include <gtest/gtest.h>


int8_t AverageThreeBytes(int8_t a, int8_t b, int8_t c)
{
    return (int8_t)(((int16_t)a + (int16_t)b + (int16_t)c) / 3);
}

TEST(AverageThreeBytes, AverageMidRangeValues_1) {
    EXPECT_EQ(40, AverageThreeBytes(30, 40, 50));
}

TEST(AverageThreeBytes, AverageMidRangeValues_2) {
    EXPECT_EQ(40, AverageThreeBytes(10, 70, 40));
}

TEST(AverageThreeBytes, AverageMidRangeValues_3) {
    EXPECT_EQ(33, AverageThreeBytes(33, 33, 33));
}
