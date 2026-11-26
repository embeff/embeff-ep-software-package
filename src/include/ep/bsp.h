// Copyright (C) 2021 embeff GmbH
// SPDX-License-Identifier: Apache-2.0
// This file is part of the ExecutionPlatform distribution. More information at https://embeff.com/executionplatform/.

#pragma once

#ifdef __cplusplus
  #include <cstdint>
  extern "C" {
#endif

    #include <stdint.h>
    #include <stdbool.h>

    // These functions must be implemented by a conforming BSP.

    /// Initializes the UART periphery and the reference pin
    ///
    /// A conforming bsp must use the same UART hardware, pins and settings (baudrate, parity, ...) as the sample bsp.
    /// After this init the UART must be ready to receive and send data (ep_bsp_output_bytes, ep_bsp_receive_byte).
    /// The "reference pin" is a digital output that is used for benchmarks.
    /// The reference pin must be configured as a fast output (push-pull).
    /// The reference pin can be freely chosen and must not match the sample bsp.
    ///
    /// @retval 0: Initialization sucessful.
    /// @retval Other: Errors occured, abort processing.
    ///
    int ep_bsp_init(void);

    /// Returns the external name of the pin that is used as reference pin.
    ///
    /// The external name is the name that is also used from the robot / user side to access the GPIO Endpoint.
    /// For example "PA0"
    ///
    char const* ep_bsp_reference_pin_get_external_name(void);

    /// Set the reference pin to HIGH.
    ///
    void ep_bsp_reference_pin_set(void);

    /// Set the reference pin to LOW.
    ///
    void ep_bsp_reference_pin_unset(void);

    /// Sends bytes over UART
    ///
    void ep_bsp_output_bytes(uint8_t const* data, int dataLength);

    /// Reads a single byte from the UART
    ///
    uint8_t ep_bsp_receive_byte(void);

    /// Reset all available caches
    ///
    /// For proper microbenchmarking caches need to be cleared between measurements.
    /// This includes FLASH and SRAM caches.
    ///
    void ep_bsp_reset_cache(void);

    /// Flushes all pending buffers
    ///
    /// If the BSP uses a buffer-based implementation to redirect stdout to the EP,
    /// this function must be implemented accordingly. Can be left empty otherwise.
    ///
    void ep_bsp_flush(void);

    /// Disables default irqs such as systick interrupt
    ///
    /// All periodic interrupts must be disabled.
    /// This function is used for example in benchmarking. Systicks or other
    /// periodic interrupts would lead to non-deterministic results.
    ///
    void ep_bsp_disable_default_irqs(void);



#ifdef __cplusplus
}
#endif
