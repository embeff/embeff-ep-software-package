// Copyright (C) 2021 embeff GmbH
// SPDX-License-Identifier: Apache-2.0
// This file is part of the ExecutionPlatform distribution. More information at https://embeff.com/executionplatform/.

#include <ep/app.h>
#include "unity.h"

#define BufferSize 40
static char buffer[BufferSize];
static int bufferUsed = 0;

void ep_unity_flush(void) {
    if (bufferUsed > 0) {
        ep_internal_resp(EP_RESP_STDOUT, buffer, bufferUsed);
        bufferUsed = 0;
    }
}

void ep_unity_print(char c) {
    buffer[bufferUsed] = c;
    bufferUsed++;

    if (bufferUsed == BufferSize) {
        ep_unity_flush();
    }
}

int32_t run_tests_with_flush(uint8_t const* arg, int argc) {
    (void)arg;
    (void)argc;
    int32_t retCode = run_tests();
    ep_unity_flush();
    ep_bsp_flush();
    return retCode;
}

void ep_app_init(void){

}

int ep_app_main(void) {
    ep_register("run_tests", &run_tests_with_flush);
    return ep_process_loop();
}
