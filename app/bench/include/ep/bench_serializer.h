// Copyright (C) 2021 embeff GmbH
// SPDX-License-Identifier: Apache-2.0
// This file is part of the ExecutionPlatform distribution. More information at https://embeff.com/executionplatform/.

#pragma once

#include <ep/bsp.h>
#include <ep/app_data_splitter.h>
#include <cstdio>

inline void ep_bench_print(char c) {
    ep_internal_resp(EP_RESP_STDOUT, &c, 1);
}

inline void ep_bench_print(char const* s) {
    ep_internal_resp_splitted_string(EP_RESP_STDOUT, s);
}

// Pre-defined serializer
template <typename T>
inline void ep_bench_serialize(T const&) {
    // Unknown serialization
    ep_bench_print('?');
}

// bool
template<>
inline void ep_bench_serialize(bool const& x) {
    ep_bench_print(x ? "true" : "false");
}

// Signed types
template<>
inline void ep_bench_serialize(char const& x) {
    char buf[20];
    snprintf(buf, 20, "%d", x);
    ep_bench_print(buf);
}

template<>
inline void ep_bench_serialize(short const& x) {
    char buf[20];
    snprintf(buf, 20, "%d", x);
    ep_bench_print(buf);
}

template<>
inline void ep_bench_serialize(int const& x) {
    char buf[20];
    snprintf(buf, 20, "%d", x);
    ep_bench_print(buf);
}

template<>
inline void ep_bench_serialize(long const& x) {
    char buf[20];
    snprintf(buf, 20, "%ld", x);
    ep_bench_print(buf);
}

// Unsigned types
template<>
inline void ep_bench_serialize(unsigned char const& x) {
    char buf[20];
    snprintf(buf, 20, "0x%02x", x);
    ep_bench_print(buf);
}

template<>
inline void ep_bench_serialize(unsigned short const& x) {
    char buf[20];
    snprintf(buf, 20, "0x%04x", x);
    ep_bench_print(buf);
}

template<>
inline void ep_bench_serialize(unsigned int const& x) {
    char buf[20];
    snprintf(buf, 20, "0x%08x", x);
    ep_bench_print(buf);
}

template<>
inline void ep_bench_serialize(unsigned long const& x) {
    char buf[20];
    snprintf(buf, 20, "0x%08lx", x);
    ep_bench_print(buf);
}

template<>
inline void ep_bench_serialize(float const& x) {
    // Distinction between float and double as in C++ "f" appendix for float, none for double
    char buf[20];
    snprintf(buf, 20, "%ff", static_cast<double>(x));
    ep_bench_print(buf);
}

template<>
inline void ep_bench_serialize(double const& x) {
    // Distinction between float and double as in C++ "f" appendix for float, none for double
    char buf[20];
    snprintf(buf, 20, "%f", x);
    ep_bench_print(buf);
}

template<>
inline void ep_bench_serialize(uint8_t const* const& x) {
    char buf[20];
    void const* addr = x;
    snprintf(buf, 20, "0x%p", addr);
    ep_bench_print(buf);
}

template<>
inline void ep_bench_serialize(uint8_t * const& x) {
    char buf[20];
    void const* addr = x;
    snprintf(buf, 20, "0x%p", addr);
    ep_bench_print(buf);
}

// End pre-defined serializer

namespace ep_internal {
    template<typename T>
    void serialize(T v) {
        ep_bench_print('\"');
        ep_bench_serialize<T>(v);
        ep_bench_print('\"');
    }

    template<typename T, typename... Args>
    void serialize(T first, Args... args) {
        serialize(first);
        ep_bench_print(", ");
        serialize(args...);
    }
}
