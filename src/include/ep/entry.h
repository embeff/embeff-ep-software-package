
// Copyright (C) 2021 embeff GmbH
// SPDX-License-Identifier: Apache-2.0
// This file is part of the ExecutionPlatform distribution. More information at https://embeff.com/executionplatform/.

#pragma once

#include <ep/bsp.h>

#ifdef __cplusplus
  #include <cstdint>
  extern "C" {
#endif

    #include <stdint.h>
    #include <stdbool.h>

    int ep_app_main(void);
    int ep_main(void); // If EP_CUSTOM_MAIN is defined, the user has to call this function from their custom main function.

#ifdef __cplusplus
}
#endif
