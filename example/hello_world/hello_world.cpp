// Copyright (C) 2026 embeff GmbH
// SPDX-License-Identifier: Apache-2.0
// This file is part of the ExecutionPlatform distribution. More information at https://embeff.com

#include <ep/core.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>

// This is the function we want to test.
// Typically it will reside in some other translation unit.
int32_t square(int num) {
    int res = num*num;
    return res;
}

// This is glue code that the ExecutionPlatform calls with `len` bytes.
// The glue code converts this into the arguments needed for the square function.
// All functions invoked by DUT Invoke need to have the same signature as square_glue.
//
int32_t square_glue(uint8_t const* data, int len) {
    // Convert byte argument to integer
    if (len > 10) return -1;
    // First create null-terminated string
    char cstring[10] = {0};
    memcpy(cstring, data, len);
    int num = atoi(cstring);

    int res = square(num);
    return res;
}

int32_t print_registers(uint8_t const*, int) {
    // printf/stdout is captured by the ExecutionPlatform and can be viewed in the test logs.
    uint32_t value = 0x1234ABCD;
    printf("Analyzing variable 'value':\n");
    printf("- Content: 0x%08lx\n", value);
    void const* rawPtr = &value;
    printf("- Address: 0x%08lx\n", (uint32_t)rawPtr);
    return 0;
}

int ep_app_main() {
    ep_register("square", &square_glue);
    ep_register("print_registers", &print_registers);
    return ep_process_loop();
}
