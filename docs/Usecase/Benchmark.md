# Benchmarks
The `bench` app provides you with insights about the runtime of given functions on your DUT.
The app generates a rising and falling gpio output around the code you invoke. These edges are recorded and measured with a resolution of less than 100ns.

Typical usage
- Determine the performance of functions or entire components quickly
- Ensure timing constraints
- Detect performance regression in real time, via refactoring, compiler changes and adjusted settings

The pin used for the edges is initialized in `bsp.c` and set/reset in `bsp_signals.h`. The name of this pin must be returned in `char const* ep_bsp_reference_pin_get_external_name()`.
Our sample BSP already provides all the definitions. You may change this definition to any DUT pin that is available as an GPIO endpoint.

## Create a binary

1. Add a blank C++ file and create a new CMake executable.
1. Link the executable against ```ep_app_bench```.
1. Include the core functions: ```#include <ep/bench.h>```.
1. Provide the ```int ep_app_main()```function.
    - This function is automatically executed. Do not provide `main`.
    - The last command must be ```return ep_process_loop();```.
1. :warning: The function that is to be benchmarked must be defined in another translation unit (e.g. a separate C++ or C file).
1. Add the macro `ep_bench_register()` with the name and signature of the function to be benchmarked.
1. Add a function and register it with `ep_register` (see [generic](../CodeInterface.md)). This function must extract the relevant parameters out of the payload and call `ep_bench_invoke` for the actual function to be measured.


## Start benchmark

<!-- tabs:start -->

#### **Robot Framework**

1. In your robot test sequence include the corresponding resource file:
    ```Resource          ${ep_app_dir}/bench/bench.resource```
1. Call `Do Benchmark` with the method name of your registered function and the payload.
1. `Do Benchmark` returns both the runtime and return value.
1. Check the runtime against a given maximum or log it for later reference.
1. You may use an arbitrary number of `Do Benchmark` calls in a single test case.

#### **pytest**

1. In your test module import the helper: `from bench.bench import do_benchmark`
1. Call `do_benchmark(ep, function, args)` with the method name of your registered function and the payload.
1. `do_benchmark` returns both the runtime in nanoseconds and the return value.
1. Check the runtime against a given maximum or log it for later reference.
1. You may use an arbitrary number of `do_benchmark` calls in a single test.

<!-- tabs:end -->

## Example
This example determines how long it takes to calculate the cosine of several angles. Via ```ep_bench_invoke```, the actual measurement on the microcontroller, for a specific set of arguments, is initiated. The results show the time each call required.

```cpp
#include <ep/bench.h>

// Functions to be measured defined in other translation unit
ep_bench_register(float bench_cos(float));

int32_t invoke_bench_cos(uint8_t const* payload, int) {
    uint8_t deg = atoi((const char*)payload);
    double const pi = 3.1415926535897932384626433;
    double rad = deg * 2.0 * pi / 360.0; // Convert degrees to radians
    float arg = static_cast<float>(rad);
    ep_bench_invoke(bench_cos, arg);
    return 0;
}

int ep_app_main() {
    ep_register("invoke_bench_cos", &invoke_bench_cos);
    return ep_process_loop();
}
```

<!-- tabs:start -->

#### **Robot Framework**

```robot
*** Settings ***
Library           EPLibrary   ${ep_url}    ${ep_binary_dir}/gpio.ep-config.zip
Resource          ${ep_app_dir}/bench/bench.resource

*** Test Cases ***
Benchmark Cosine For Various Degrees
    DUT Flash Firmware    ${ep_binary_dir}/example/example_bench_cos.hex

    FOR    ${deg}    IN RANGE    0    360    20
        ${deg_s} =     Convert To String    ${deg}
        ${runtime_ns}    ${retval} =    Do Benchmark    invoke_bench_cos    ${deg_s}
        Log Many    ${deg}    ${runtime_ns}
    END
```

#### **pytest**

```python
import os

from bench.bench import do_benchmark
from gpio.test import EP    # imports gpio.ep-config.zip

BINARY_DIR = os.environ["EP_BINARY_DIR"]


ep = EP.fixture()


def test_benchmark_cosine_for_various_degrees(ep: EP):
    ep.DUT.flash_firmware(f"{BINARY_DIR}/example/example_bench_cos.hex")

    for deg in range(0, 360, 20):
        runtime_ns, retval = do_benchmark(ep, "invoke_bench_cos", str(deg))
        print(deg, runtime_ns)
```

<!-- tabs:end -->

The complete example is located in `example/misc/bench`.
