# Copyright (C) 2026 embeff GmbH
# SPDX-License-Identifier: Apache-2.0
# This file is part of the ExecutionPlatform distribution. More information at https://embeff.com

"""pytest helpers for the bench app, the counterpart of bench.resource.

    from bench.bench import do_benchmark
"""

import json


def do_benchmark(ep, function, args=""):
    """Measure the runtime of an ``ep_bench_invoke`` call on the DUT.

    ``function`` must be a registered method on the DUT that calls ``ep_bench_invoke``.
    ``args`` is a string that is supplied to ``function``. It is intended to be parsed on the DUT
    and applied as arguments to the benchmarked function.

    Returns the runtime in nanoseconds and the return value of the benchmarked function.
    """
    assert ep.DUT.invoke("ep_print_reference_pin") == 0, "Benchmark could not determine reference pin name."
    name_output = ep.DUT.get_output()
    assert len(name_output) == 1, ("DUT output for benchmark is corrupted. "
                                   "At this point it should contain just the reference pin name.")
    reference_pin = name_output[0]

    measurement = ep.GPIO.measure_between_rising_and_falling_edge(reference_pin, reference_pin)
    assert ep.DUT.invoke(function, args) == 0, "Benchmark invoke did not complete successfully."
    runtime_ns = ep.GPIO.get_measurement_result(measurement, ReturnType="ns")

    output = ep.DUT.get_output()
    assert len(output) == 1, "DUT output for benchmark is corrupted. It should contain only argument and return values."
    return runtime_ns, json.loads(output[0])["ret"]
