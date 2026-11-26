// Copyright (C) 2024 embeff GmbH
// SPDX-License-Identifier: Apache-2.0
// This file is part of the ExecutionPlatform distribution. More information at https://embeff.com/executionplatform/.

// Used to implemented apps. Not required from user code.

#pragma once

#include <ep/core.h>

#ifdef __cplusplus
  #include <cstdint>
  extern "C" {
#endif

    #include <stdint.h>
    #include <stdbool.h>

    enum ep_resp_code {
        EP_RESP_BSP_LOADED = 10,
        EP_RESP_APP_LOADED = 20,
        EP_RESP_ABORT = 30,
        EP_RESP_STDOUT = 40,
        EP_RESP_APP_DATA_RAW = 70,
        EP_RESP_COVERAGE_RAW = 71,
    };

    void ep_internal_resp(enum ep_resp_code code, void const* payload, int payloudLength);
    void ep_internal_resp_string(enum ep_resp_code code, char const* payload);
    void ep_app_init(void);
#ifdef __cplusplus
}
#endif
