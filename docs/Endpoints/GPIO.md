<!-- The keyword sections of this page are generated from the Python docstrings; see tools/docs/generate_docs.py -->

# GPIO Endpoint
The GPIO endpoint makes it possible to test single/multiple GPIO pins.
All terms are referring to the endpoint. An input is an endpoint input, which will typically be connected to a DUT output.

## Technical data
- All DUT package pins are available as `<GPIOPin>`.
- Lists of `<GPIOPin>` can contain arbitrary long combinations of `<GPIOPin>`.
- Time resolution: 10ns.
- Measurement accuracy < 20ns.
- Maximum number of configured endpoints: 144.
- Max number of concurrent measurements: 4.
- Max number of concurrent pulse generations: 4.
- All measurements are relative to events observable outside the DUT.
- DC Input and Output Levels:
  * Input:
    | Low min | Low max | High min | High max |
    | ------- | ------- | -------- | -------- |
    |  -0.3V  |   0.8V  |   2.0V   |   3.4V   |
  * Output:
    | Low max | High min |
    | ------- | -------- |
    |   0.4V  |   2.9V   |

## Configuration
This section lists the properties that can be configured for each `GPIO` endpoint.

| Setting      | Description | Values |
| ----------- | ----------- | ------- |
| Pin      | Package number connected to the instance.  | DUT-dependent |
| Pull      | Enable a pullup or pulldown resistor within the endpoint. | `None` / `Up` / `Down` |

### Example
```yaml
GPIO:
  PA0:
    Pin: 34
    Pull: None
  PA1:
    Pin: 35
    Pull: None
```

If a pin is referenced that is not defined in the configuration, an error `EndpointUsageError: GPIO Endpoint '<PinName>' does not exist.` will be thrown.

## Setup & Teardown
All measurements and puls generation are automatically stopped after test teardown.
All GPIOs are set to Tristate after test teardown.

:information_source: All measurement functions automatically restart the measurement when a new start-condition is detected.

## Test frontend

Setup, controlling and observing functions are shown by their name. [Robot Framework](../TestFrontend/RobotFramework.md) and [pytest](../TestFrontend/pytest.md) explain how to call them on an endpoint instance.

### Setup functions

<a id="measure-between-rising-edges-keyword"></a>`measure_between_rising_edges(start_io, stop_io)`
- Called on the static endpoint `GPIO`, which needs no entry in the ep-config. See [Static endpoints](../Endpoints.md#static-endpoints).
- Starts measuring when a rising edge occurs on `start_io`.
- Stops measuring when a rising edge occurs on `stop_io`.
- Returns a measurement ID that can later be used to read the result.
- Throws an error `EndpointUsageError: No more measurements available` when maximum number of concurrent measurements is exceeded.

<a id="measure-between-falling-edges-keyword"></a>`measure_between_falling_edges(start_io, stop_io)`
- Called on the static endpoint `GPIO`, which needs no entry in the ep-config. See [Static endpoints](../Endpoints.md#static-endpoints).
- Starts measuring when a falling edge occurs on `start_io`.
- Stops measuring when a falling edge occurs on `stop_io`.
- Returns a measurement ID that can later be used to read the result.
- Throws an error `EndpointUsageError: No more measurements available` when maximum number of concurrent measurements is exceeded.

<a id="measure-between-rising-and-falling-edge-keyword"></a>`measure_between_rising_and_falling_edge(start_io, stop_io)`
- Called on the static endpoint `GPIO`, which needs no entry in the ep-config. See [Static endpoints](../Endpoints.md#static-endpoints).
- Starts measuring when a rising edge occurs on `start_io`.
- Stops measuring when a falling edge occurs on `stop_io`.
- Returns a measurement ID that can later be used to read the result.
- Throws an error `EndpointUsageError: No more measurements available` when maximum number of concurrent measurements is exceeded.

<a id="measure-between-falling-and-rising-edge-keyword"></a>`measure_between_falling_and_rising_edge(start_io, stop_io)`
- Called on the static endpoint `GPIO`, which needs no entry in the ep-config. See [Static endpoints](../Endpoints.md#static-endpoints).
- Starts measuring when a falling edge occurs on `start_io`.
- Stops measuring when a rising edge occurs on `stop_io`.
- Returns a measurement ID that can later be used to read the result.
- Throws an error `EndpointUsageError: No more measurements available` when maximum number of concurrent measurements is exceeded.

### Controlling functions

<a id="set-high-keyword"></a>`set_high()`
- Sets the pin high immediately.
- Returns the test time when the pin sets high.

<a id="set-low-keyword"></a>`set_low()`
- Sets the pin low immediately.
- Returns the test time when the pin sets low.

<a id="set-tristate-keyword"></a>`set_tristate()`
- Sets the pin to a high impedance (input) immediately.
- Returns the test time when the pin sets to tri-state.

<a id="set-pins-keyword"></a>`set_pins(high=[], low=[], tristate=[])`
- Called on the static endpoint `GPIO`, which needs no entry in the ep-config. See [Static endpoints](../Endpoints.md#static-endpoints).
- All pins are set to their respective state immediately and synchronously.
- Returns the test time when the pins are set.

<a id="generate-high-pulse-on-pin-keyword"></a>`generate_high_pulse_on_pin(pin, length, WaitForCompletion=True)`
- Called on the static endpoint `GPIO`, which needs no entry in the ep-config. See [Static endpoints](../Endpoints.md#static-endpoints).
- Sets `pin` to high for the given `length`.
- It is required that `set_low` is called on `pin` prior to this call. Otherwise the test execution fails.
- The maximum pulse length is 42s.
- Throws an error `EndpointUsageError: Can't generate a pulse on <pin>: a pulse is already running.` when the previous pulse on `pin` has not finished yet.

<a id="generate-low-pulse-on-pin-keyword"></a>`generate_low_pulse_on_pin(pin, length, WaitForCompletion=True)`
- Called on the static endpoint `GPIO`, which needs no entry in the ep-config. See [Static endpoints](../Endpoints.md#static-endpoints).
- Sets `pin` to low for the given `length`.
- It is required that `set_high` is called on `pin` prior to this call. Otherwise the test execution fails.
- The maximum pulse length is 42s.
- Throws an error `EndpointUsageError: Can't generate a pulse on <pin>: a pulse is already running.` when the previous pulse on `pin` has not finished yet.

### Observing functions

<a id="read-keyword"></a>`read()`
- Returns `LOW` or `HIGH`.
- If the MCU does not drive the pin (tristate), the endpoints `Pull` configuration might determine the result.

<a id="read-pins-keyword"></a>`read_pins(pins: List=[])`
- Called on the static endpoint `GPIO`, which needs no entry in the ep-config. See [Static endpoints](../Endpoints.md#static-endpoints).
- Returns dictionary with each pin from `pins` with level (`LOW` or `HIGH`).
- The reading happens immediately and synchronously.
- If the MCU does not drive the pin (tristate), the endpoints `Pull` configuration might determine the result.

<a id="get-measurement-result-keyword"></a>`get_measurement_result(id, timeout="5s", ReturnType="s")`
- Called on the static endpoint `GPIO`, which needs no entry in the ep-config. See [Static endpoints](../Endpoints.md#static-endpoints).
- Returns the result of the given measurement.
- The time is returned as a float in the given `ReturnType` unit.
- Throws an error `RemoteError: Measurement not started` when the `timeout` elapsed and no start trigger occurred.
- Throws an error `RemoteError: Measurement still running` when the `timeout` elapsed and a start but no stop trigger occurred.
- If the `timeout` is set to 0s, the call will block until a measurement is available.

## Model functions

These functions can be called from the [Python-based environment simulation](System.md).

```python
def set_high(self)
```
- Sets the pin high immediately.

```python
def set_low(self)
```
- Sets the pin low immediately.

```python
def set_tristate(self)
```
- Sets the pin to a high impedance (input) immediately.

```python
def read_as_bool(self)
```
- Returns `True` if the pin reads `HIGH`, `False` otherwise.
- If the MCU does not drive the pin (tristate), the endpoints `Pull` configuration might determine the result.

## Callbacks

This endpoint provides callbacks that can be used inside the [Python-based environment simulation](System.md).

```python
def on_rising(self, callback: Callable[[], None])
```
- Registers a callback that is called when a rising edge occurs on the pin.

```python
def on_falling(self, callback: Callable[[], None])
```
- Registers a callback that is called when a falling edge occurs on the pin.

A simple example that implements a pin-following approach. GPIO1 follows changes in GPIO0.
Note that in practice you should use the [Pin-Follow Endpoint](Pin-Follow.md) to track/follow signals precisely.

```python
class SystemModel(config.Configuration):
    def __init__(self):
      pass

    def gpio0_on_rising(self):
      self.GPIO1.set_high()

    def gpio0_on_falling(self):
      self.GPIO1.set_low()

    def start(self):
        if self.GPIO0.read_as_bool():
            self.GPIO1.set_high()
        else:
            self.GPIO1.set_low()

        self.GPIO0.on_rising(self.gpio0_on_rising)
        self.GPIO0.on_falling(self.gpio0_on_falling)
```
