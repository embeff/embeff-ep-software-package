<!-- The keyword sections of this page are generated from the Python docstrings; see tools/docs/generate_docs.py -->
# Pin-Follow Endpoint

The Pin-Follow endpoint tracks changes on an input signal and follows them with a given delay on an output pin. If desired, the signal can be inverted.

:information_source: For this endpoint the System license package is required on your ExecutionPlatform.

## Technicals

- Maximum number of tracked level changes N<sub>max_changes</sub>=10
- Resolution: 10ns
- Maximum follow delay: 42s

## Configuration

This section lists the properties that can be configured for each `Pin-Follow` endpoint.

| Setting      | Description | Values |
| ------------ | ----------- | ------ |
| InSignal     | Package number for the pin that should be followed. | DUT-dependent |
| OutPin       | Package number for the output pin.                  | DUT-dependent |

### Example

```yaml
PIN_FOLLOW:
  FOLLOW1:
    InSignal: 2
    OutPin: 3
```

## General behavior

The Pin-Follow Endpoint can keep track of a maximum of N<sub>max_edges</sub> rising/falling edges at once. This means that there should never be more than N<sub>max_changes</sub> level changes within any timespan previously set with `Delay`(See `Setup functions`).
Any level changes exceeding N<sub>max_changes</sub> will be ignored until there is space in the queue. The number of skipped edges is always even.

![Example Signal with two skipped edges](Pin-Follow-signal.drawio.png)

The endpoint supports a strict mode that ensures tests are failing when too many level changes are observed on the input signal. Error message `EndpointStrictModeError: Strict mode invalidated (Too many level changes)`.
Automatic Strict mode Check is enabled by default and will fail in teardown, it can be disabled with `disable_automatic_strict_mode_check`.

When the Endpoint is not enabled, `OutPin` is set `LOW`.

## Setup & Teardown

`disable` is automatically called after each test.

## Test frontend

Setup, controlling and observing functions are shown by their name. [Robot Framework](../TestFrontend/RobotFramework.md) and [pytest](../TestFrontend/pytest.md) explain how to call them on an endpoint instance.

### Setup functions

<a id="enable-keyword"></a>`enable(Delay, Inverted=False)`
- The endpoint starts following the input signal.
- To reconfigure the endpoint on the fly, this function can be called repeatedly.

<a id="disable-keyword"></a>`disable()`
- The endpoint stops following the input signal.

<a id="disable-automatic-strict-mode-check-keyword"></a>`disable_automatic_strict_mode_check()`
- Disable the strict mode check in Teardown for the current test.

### Controlling functions

The Pin-Follow endpoint has no controlling functions.

### Observing functions

<a id="check-strict-mode-keyword"></a>`check_strict_mode()`
- Fails test if strict mode is invalid.

## Model functions

The Pin-Follow endpoint has no model functions.

## Callbacks

The Pin-Follow endpoint has no callbacks.
