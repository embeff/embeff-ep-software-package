// Copyright (C) 2021 embeff GmbH
// SPDX-License-Identifier: Apache-2.0
// This file is part of the ExecutionPlatform distribution. More information at https://embeff.com/executionplatform/.

#include <ep/app_data_splitter.h>
#include <gtest/gtest.h>
#include <sys/time.h>

extern "C" {
    int32_t run_tests_with_flush(uint8_t const*, int) {
        int argc = 2;
        char arg0[] = "dummy_target"; // usually contains the filename, not important here
        char arg1[] = "--gtest_print_time=0"; // do not print time flags
        char* args[] = {arg0, arg1};
        testing::InitGoogleTest(&argc, args);

        // retval
        // 0: all tests passed
        // 1: otherwise
        auto retval = RUN_ALL_TESTS();

        fflush(0); // some output buffers might be cached, force write
        ep_bsp_flush();

        return retval;
    }

    void ep_app_init(){

    }

    int ep_app_main() {
        ep_register("run_tests", &run_tests_with_flush);
        return ep_process_loop();
    }

    // This function is required by gtest but does not need to be functional
    void __sync_synchronize() {

    }

    // This function is required by gtest but does not need to be functional
    int	mkdir (const char *, mode_t) {
        return 0;
    }

    int _gettimeofday( struct timeval *tv, void *) {
        tv->tv_sec = 0;
        tv->tv_usec = 0;
        return 0;  // return non-zero for error
    }
}
