// Copyright (C) 2021 embeff GmbH
// SPDX-License-Identifier: Apache-2.0
// This file is part of the ExecutionPlatform distribution. More information at https://embeff.com/executionplatform/.

#include <ep/bench.h>
#include <stdlib.h>

// "crc32" is defined in another translation unit (otherwise compiler might optimize it)
// All functions that are used in ep_bench_invoke, need to be declared here.
ep_bench_register(uint32_t crc32(int, uint8_t const*));

int32_t bench_crc32(uint8_t const* payload, int payloadLength) {
    // We use the payload that is given by the user in bench.robot to call the crc32 function
    // The function call is benchmarked. Both execution time and result are delivered back to crc32_bench.robot.

    // Call "crc32" with first parameter payloadLength and second parameter payload
    ep_bench_invoke(crc32, payloadLength, payload);
    return 0;
}

int ep_app_main() {
    ep_register("bench_crc32", &bench_crc32);
    return ep_process_loop();
}
