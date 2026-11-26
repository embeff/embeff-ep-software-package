# Endpoints
This section explains important principles that apply for all endpoints.
Reading it carefully, it will help you in using these libraries effectively.

It covers
- The rationale behind endpoint function design.
- How endpoint functions are named and called.
- What differentiates the endpoint function categories.
- How physical units are handled.
- How byte data is handled.


## Design philosophy

All endpoints follows the unix philosophy in their function design:
- Each endpoint provides a minimal set of functions for the specific periphery.
- These functions can be combined in a flexible way.

You combine them into the steps your tests need. [Robot Framework](TestFrontend/RobotFramework.md) and [pytest](TestFrontend/pytest.md) show how to do this in your test frontend.

## Function names
The endpoint pages show each function by its name, for example `set_high()` on the GPIO page or `send_frame(Id, Data: ByteData, Extended=True, StoreInBuffer=True)` on the CAN page. The functions of a [static endpoint](#static-endpoints) are shown the same way, for example `set_pins(high=[], low=[], tristate=[])` of the static endpoint `GPIO`.
A test calls a function on an endpoint instance, which you name in the [ep-config](EP_Config.md): with a GPIO endpoint named `PA0`, Robot Framework calls `PA0 Set High` and pytest calls `ep.PA0.set_high()`. The [Test frontend](TestFrontend/RobotFramework.md) chapter explains the calls in detail.

### Static endpoints
Some endpoints have a fixed name. Their functions are called on this name instead of an instance name:
- `GPIO` holds the functions that act on several GPIO pins at once, for example `set_pins(high=[], low=[], tristate=[])` and `measure_between_rising_edges(start_io, stop_io)`. They take the names of the GPIO pins as arguments. Robot Framework calls `GPIO Set Pins`, pytest calls `ep.GPIO.set_pins()`.
- `DUT` holds the functions of the [Code Interface](CodeInterface.md), for example `invoke(method_name, data: ByteData)`.
- `SYSTEM` and `FMI` hold the functions of the environment models of the [System](Endpoints/System.md) and [FMI](Endpoints/FMI.md) endpoints.

`GPIO`, `DUT` and `SYSTEM` need no entry in the ep-config. `FMI` is available when the ep-config contains an `FMI` block.

The endpoint pages say which static endpoint a function is called on: once at the top of a page whose functions all belong to one static endpoint, and in the first line of each function where a page also documents instance functions, as on the GPIO page.

## Physical units
Some functions deal with physical units, such as seconds or volts. This section describes how these units are handled across all Endpoints.
For usability reasons, parameters and return values are handled differently.

### Parameters
Parameters must always be given as a string with a physical unit attached. Decimal types are supported. If a given parameter is exceeding its allowed range (sepcified for each function individually), a `RangeError` is raised.
A single space between number and unit is optional (`17mV` and `17 mV` are equal). Units are case-sensitive.

Examples
- 17 millivolt: `17mV`
- 2050 nanoseconds: `2.05us`.

| Type | Unit | Allowed units |
| -    | -    | - |
| Time | Second | `ns`, `us`, `ms`, `s`, `min` |
| Voltage | Volt | `mV`, `V`, `kV` |
| Frequency | Hertz | `Hz`, `kHz`, `MHz` |
| Bit rate | Bit per second | `bit/s`, `kbit/s`, `Mbit/s` |
| Ratio | Percent | `%` |
| Angle | Degree | `deg` |

| Prefix | Name | Decimal |
| - | -    | -     |
| M | mega | 1 000 000 |
| k | kilo | 1 000 |
| - |  | 1 |
| m | milli  | 0.001 |
| u | micro  | 0.000 001 |
| n | nano  | 0.000 000 001 |

For example, the GPIO endpoint provides a function to generate a pulse:
    `generate_high_pulse_on_pin(pin, length, WaitForCompletion=True)`

The parameter `length` accepts a time.
The following calls are all generating a 52 millisecond pulse on PB10:

<!-- tabs:start -->

#### **Robot Framework**

```robot
GPIO Generate High Pulse On Pin    PB10    52ms
GPIO Generate High Pulse On Pin    PB10    0.052s
GPIO Generate High Pulse On Pin    PB10    52000us
```

#### **pytest**

```python
ep.GPIO.generate_high_pulse_on_pin("PB10", "52ms")
ep.GPIO.generate_high_pulse_on_pin("PB10", "0.052s")
ep.GPIO.generate_high_pulse_on_pin("PB10", "52000us")
```

<!-- tabs:end -->

### Return values
Values are returned as a floating point. By default the base is one, which can be overwritten by using the ReturnType parameter.

Example:
- `get_measurement_result(id, timeout="5s", ReturnType="s")`

Let us assume the measurement is equal to 5250 microseconds. Depending on `ReturnType`, the function returns:

| `ReturnType` | Result |
| - | - |
| (default) or `s` | `0.005250` |
| `ms` | `5.250` |
| `us` | `5250` |

<!-- tabs:start -->

#### **Robot Framework**

```robot
${measurement_s} =      GPIO Get Measurement Result    ${id}                  # Returns 0.005250
${measurement_s} =      GPIO Get Measurement Result    ${id}  ReturnType=s    # Returns 0.005250
${measurement_ms} =     GPIO Get Measurement Result    ${id}  ReturnType=ms   # Returns 5.250
${measurement_us} =     GPIO Get Measurement Result    ${id}  ReturnType=us   # Returns 5250
```

Robot Framework converts numbers in its own way when it compares them. See [Robot Framework](TestFrontend/RobotFramework.md#compare-floating-point-results) before you compare small values.

#### **pytest**

```python
measurement_s = ep.GPIO.get_measurement_result(id)                    # Returns 0.005250
measurement_s = ep.GPIO.get_measurement_result(id, ReturnType="s")    # Returns 0.005250
measurement_ms = ep.GPIO.get_measurement_result(id, ReturnType="ms")  # Returns 5.250
measurement_us = ep.GPIO.get_measurement_result(id, ReturnType="us")  # Returns 5250
```

<!-- tabs:end -->

## Byte Data
Some functions use parameters or return values that handle raw byte sequences.

All functions handle byte sequences in the same way:
- Arguments can be passed as bytes, strings or lists. We call this argument type _ByteData_.
- Results are always returned as bytes.
- A string must only contain characters that fit into a single byte. An argument with other unicode characters is rejected.

The examples use `invoke` of the [Code Interface](CodeInterface.md), which accepts a _ByteData_ argument for its payload:

<!-- tabs:start -->

#### **Robot Framework**

```robot
DUT Invoke    print_payload    Hey
DUT Invoke    get_length       \xFF\x48\x65\x85\x79
```

Bytes, strings and lists are different types in Robot Framework. [Robot Framework](TestFrontend/RobotFramework.md#handle-byte-data) explains how you best choose a type and how you can handle them.

#### **pytest**

```python
ep.DUT.invoke("print_payload", "Hey")
ep.DUT.invoke("get_length", b"\xFF\x48\x65\x85\x79")
ep.DUT.invoke("get_length", [0xFF, 0x48, 0x65, 0x85, 0x79])
```

<!-- tabs:end -->


## Categories

### Test frontend
Setup, controlling and observing functions are called from the test sequences of your test frontend, [Robot Framework](TestFrontend/RobotFramework.md) or [pytest](TestFrontend/pytest.md). They can be used only from test sequences.

#### Setup functions
Setup functions are used to prepare a test. A setup function returns when the preparation is done.

#### Controlling functions
Controlling functions are used to actively modify the system state. They will either have an instantaneous effect or wait for completion.

The user can supply the `WaitForCompletion=False` argument to force an immediate (non-blocking) return.
If `WaitForCompletion=True` is not allowed in a specific setting (for example, because the controlling action would run indefinitely), a `EndpointUsageError` is raised.

#### Observing functions
Observing functions read back information about the system state.
- Typically the result is returned immediately.
- If the result is dependent on the DUT, it might block until the information is ready. In this case the function will support an optional `Timeout` argument. If the timeout elapses before the requested information is ready, an `EndpointTimeoutError` is raised.

### Model functions
Model functions are called directly by a model to drive or read the endpoint.

Model functions can be used only inside the [Python-based environment simulation](Endpoints/System.md).

### Callbacks
Callback functions make it possible to define a handler that is called for Endpoint-specific events.

Callbacks function can be used only inside the [Python-based environment simulation](Endpoints/System.md).

## Test time
Some functions return a relative time called *test time*.
The test time is automatically restarted when a new test begins.
It counts in steps of 10 ns and wraps after about 42.9 s (2^32 steps).
Note that there are no timing guarantees regarding the runtime of the test execution itself.
However you may use the difference between two or more *test times* to calculate a precise timing.
The same *test time* is used across all endpoints.

## Error handling
Endpoints report the following errors:
- `EndpointUsageError` - when keywords are not called as intended
- `EndpointAssertionError` - when conditions expected in keyword are not met
- `EndpointTimeoutError` - when operation in keyword timeouts
- `EndpointStrictModeError` - when strict mode is invalidated
- `EndpointRuntimeError` - when other error occurred during execution
