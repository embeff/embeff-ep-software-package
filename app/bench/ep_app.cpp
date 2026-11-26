// Copyright (C) 2021 embeff GmbH
// SPDX-License-Identifier: Apache-2.0
// This file is part of the ExecutionPlatform distribution. More information at https://embeff.com/executionplatform/.

#include <ep/app.h>
#include <ep/bsp_signals.h>
#include <stdio.h>

extern "C"{
    int32_t ep_print_reference_pin(uint8_t const*, int) {
        ep_internal_resp_string(EP_RESP_STDOUT, ep_bsp_reference_pin_get_external_name());
        ep_internal_resp_string(EP_RESP_STDOUT, "\n");
        return 0;
    }

    void ep_app_init(){
        // Recurring interrupts (such as systick) might happen during a benchmark. Disable them to make results deterministic.
        ep_register("ep_print_reference_pin", &ep_print_reference_pin);
        ep_bsp_disable_default_irqs();
    }
}
