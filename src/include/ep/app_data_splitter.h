// Copyright (C) 2021 embeff GmbH
// SPDX-License-Identifier: Apache-2.0
// This file is part of the ExecutionPlatform distribution. More information at https://embeff.com

#pragma once

#include <ep/app.h>

#ifdef __cplusplus
#include <cstdint>
extern "C" {
#endif

void ep_internal_resp_splitted(enum ep_resp_code code, void const* payload, int len);
void ep_internal_resp_splitted_string(enum ep_resp_code code, char const* string);

#ifdef __cplusplus
}
#endif
