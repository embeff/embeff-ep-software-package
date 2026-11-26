// Copyright (C) 2021 embeff GmbH
// SPDX-License-Identifier: Apache-2.0
// This file is part of the ExecutionPlatform distribution. More information at https://embeff.com/executionplatform/.

// This file implements two functions that start the actual benchmarking.
// The functions defined here called from cos_bench.robot.

#include <ep/bench.h>
#include <array>
#include <stdlib.h>
#include <cmath>

// "cos_32" is defined in another translation unit (otherwise compiler might optimize it)
// All functions that are used in ep_bench_invoke, need to be declared here.
ep_bench_register(float cos_32(float));

float convert_payload_to_radiand(uint8_t const* payload) {
    static double const pi = 3.1415926535897932384626433;
    uint8_t deg = atoi((const char*)payload);
    double rad = deg * 2.0 * pi / 360.0; // Convert degrees to radians
    return static_cast<float>(rad);
}

int32_t bench_cos(uint8_t const* payload, int) {
    float radiand = convert_payload_to_radiand(payload);

    // The cosf function call is benchmarked. Both execution time and result are delivered back to cos_bench.robot.
    // The cosf (cosine-float) function is defined and implemented in the standard library.
    ep_bench_invoke(cosf, radiand);
    return 0;
}

int32_t bench_cos_32(uint8_t const* payload, int) {
    float radiand = convert_payload_to_radiand(payload);

    // The cos_32 function call is benchmarked. Both execution time and result are delivered back to cos_bench.robot.
    // The cos_32 (cosine-3.2 digit approximation) function is defined in cos.cpp
    ep_bench_invoke(cos_32, radiand);
    return 0;
}

int ep_app_main() {
    ep_register("bench_cos", &bench_cos);
    ep_register("bench_cos_32", &bench_cos_32);
    return ep_process_loop();
}
