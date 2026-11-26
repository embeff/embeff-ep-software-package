<!-- The keyword sections of this page are generated from the Python docstrings; see tools/docs/generate_docs.py -->
# Analog-In
The Analog-In endpoint captures analog waveforms. This makes it possible to test DAC implementations on the DUT.

## Technicals
- Available instances: 8.
- Input voltage range: DUT-dependent (defined by `analog_ref_in` voltage), 5V max.
- Resolution: 14-bit
- Sample Rate: 200kSPS
- Maximum number of captured values per instance: 200

## Configuration
Each `Analog-In` endpoint instance has to be configured with the DUT-dependent package number connected to the instance.

| Setting      | Description | Values |
| ----------- | ----------- | ------- |
| Pin      | Package number connected to the instance. The pin must be connected to an Analog_In label in the DUT schematic. | DUT-dependent |

### Example
```yaml
ANIN:
  DAC0:
    Pin: 3
  DAC1:
    Pin: 4
```
Note that the instance names (´DAC0´, ´DAC1´) are arbitrary. You can exchange them to your liking.

## General behavior

## Setup & Teardown
A running periodic measurement is automatically aborted and recorded values are discarded after each test.

## Test frontend

Setup, controlling and observing functions are shown by their name. [Robot Framework](../TestFrontend/RobotFramework.md) and [pytest](../TestFrontend/pytest.md) explain how to call them on an endpoint instance.

### Setup functions

<a id="start-periodic-measurement-keyword"></a>`start_periodic_measurement(Count, Distance, TriggerPin=None)`
- Starts sampling `Count` analog values with a time distance of `Distance` between each sampling point.
- For each endpoint, there can be only one active periodic measurement. If another measurement is still running, an `EndpointUsageError` is raised.
- If `TriggerPin` is `None`, the sampling starts immediately.
- If `TriggerPin` is set to a GPIO pin, the sampling starts on a rising edge of this pin.
    - The `TriggerPin` must be `LOW`. Otherwise an error `Fail to wait for rising edge on a pin that is already high` is raised. See section `Interaction with other endpoints` for more information.
    - The delay between trigger detection and first measurement is between 10us and 15us.
- The measurement results can later be retrieved with `get_measurement_results`.

### Controlling functions

The Analog-In endpoint has no controlling functions.

### Observing functions

<a id="single-measurement-keyword"></a>`single_measurement(ReturnType="V")`
- Returns the current voltage on the pin.
- If a periodic measurement is still running on this instance, an `EndpointUsageError` is raised.

<a id="get-measurement-results-keyword"></a>`get_measurement_results(Timeout="5s", ReturnType="V")`
- Returns a list of results for the most recent periodic measurement.
- The list contains the recorded voltage as a float in the given `ReturnType` unit.
- Throws an error `EndpointUsageError: Measurement not started.` when the keyword `start_periodic_measurement` has not been called yet.
- Throws an error `EndpointAssertionError: Measurement not started.` when the timeout elapsed and no start trigger occurred.
- Throws an error `EndpointTimeoutError: Measurement still running.` when the timeout elapsed but the measurement was not fully completed.
- If the timeout is set to `0s`, the call will block until the results are available.

## Model functions

The Analog-In endpoint has no model functions.

## Callbacks

The Analog-In endpoint has no callbacks.

# Interaction with other endpoints
## Synchronization
The waveform recording can be synchronized by using the `TriggerPin`. Multiple Analog-In endpoints may use the same trigger, so that waveforms capturing begins at the same time.

`TriggerPin` can be set to any GPIO endpoint instance.
The rising edge can be driven both from the endpoint (set pin high, pulse creation) or by setting it from the DUT.
