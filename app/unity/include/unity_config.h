// Copyright (C) 2026 embeff GmbH
// SPDX-License-Identifier: Apache-2.0
// This file is part of the ExecutionPlatform distribution. More information at https://embeff.com

#pragma once

#include <ep/entry.h> // defined ep_app_main with C Linkage

int run_tests(void);    // Must be defined by each app

#define UNITY_OUTPUT_CHAR(a)                    ep_unity_print(a)
#define UNITY_OUTPUT_CHAR_HEADER_DECLARATION    ep_unity_print(char)
#define UNITY_OUTPUT_FLUSH()                    ep_unity_flush()
#define UNITY_OUTPUT_FLUSH_HEADER_DECLARATION   ep_unity_flush(void)
