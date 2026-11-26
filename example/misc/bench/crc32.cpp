// Copyright (C) 2021 embeff GmbH
// SPDX-License-Identifier: Apache-2.0
// This file is part of the ExecutionPlatform distribution. More information at https://embeff.com/executionplatform/.

#include <cstdint>

// This is the function that is benchmarked in crc32_bench.robot

uint32_t crc32(int byteCount, uint8_t const* buffer) {
    const uint32_t POLY = 0x04C11DB7;
    uint32_t crc = -1;

    while( byteCount-- ) {
        uint32_t shiftValue = *buffer++;
        shiftValue <<= 24;
        crc = crc ^ shiftValue;
        for( int bit = 0; bit < 8; bit++ )
        {
            if( crc & (1L << 31)) crc = (crc << 1) ^ POLY;
            else                  crc = (crc << 1);
        }
    }
    return crc;
}
