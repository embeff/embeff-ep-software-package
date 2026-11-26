// Copyright (C) 2024 embeff GmbH
// SPDX-License-Identifier: Apache-2.0
// This file is part of the ExecutionPlatform distribution. More information at https://embeff.com/executionplatform/.

// Used from user code. Exposes commands to establish communication between EP and DUT.

#pragma once

#include "entry.h"

#ifdef __cplusplus
  #include <cstdint>
  extern "C" {
#endif

    #include <stdint.h>
    #include <stdbool.h>

    // Aborts the execution. The reason is shown to the user.
    void ep_abort(char const* reason);

    typedef int32_t (*Callback)(uint8_t const* payload, int len);

    void ep_register(const char* method_name, Callback function);

    // Endless loop that proccceses all incoming invoke commands
    //
    // Call only after all functions to be invoked are registered with ep_register
    //
    int ep_process_loop(void);


#ifdef __cplusplus
}
#endif
