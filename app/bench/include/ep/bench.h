// Copyright (C) 2021 embeff GmbH
// SPDX-License-Identifier: Apache-2.0
// This file is part of the ExecutionPlatform distribution. More information at https://embeff.com/executionplatform/.

#pragma once

#include "bench_serializer.h"
#include <utility>
#include <cstdint>
#include <ep/bsp_signals.h>
#include <ep/core.h>

namespace ep_internal {
    template <typename T_Ret, typename... Args>
    inline void Report(T_Ret ret, Args... args)
    {
        ep_bench_print("{ \"args\": [ ");
        serialize(args...);
        ep_bench_print("], \"ret\": ");
        serialize(ret);
        ep_bench_print("}\n");
    }
    template <typename T_Ret>
    inline void Report(T_Ret ret)
    {
        ep_bench_print("{ \"args\": [ ");
        ep_bench_print("], \"ret\": ");
        serialize(ret);
        ep_bench_print("}\n");
    }
}


template<typename Fn>
void ep_bench_invoke(Fn fn) {
    ep_bsp_reset_cache();

    // No ABI assumptions possible here! (such as argument 0 in RO etc) because the functions are not in separate translation units
    ep_bsp_reference_pin_set();

    // Only here the arguments for func are copied to the actual registers
    // This means overhead of function call preperation on the caller side is included within the benchmark
    auto ret = fn();

    ep_bsp_reference_pin_unset();

    ep_internal::Report(ret);
    // The mapping between cycles and the reported argument/return values is implicit by execution order.
}

template<typename Fn, typename... Args>
void ep_bench_invoke(Fn fn, Args &&... args) {
    ep_bsp_reset_cache();

    // No ABI assumptions possible here! (such as argument 0 in RO etc) because the functions are not in separate translation units
    ep_bsp_reference_pin_set();

    // Only here the arguments for func are copied to the actual registers
    // This means overhead of function call preperation on the caller side is included within the benchmark
    auto ret = fn(std::forward<Args>(args)...);

    ep_bsp_reference_pin_unset();

    ep_internal::Report(ret, args...);
    // The mapping between cycles and the reported argument/return values is implicit by execution order.
}

// The user has to use this macro to define a function which he wants to benchmark.
// This macro currently just defines the prototype as supplied;
// This allows for arbitrary function names/prototpyes to be used.
#define ep_bench_register(prototype) prototype
