<!-- The keyword sections of this page are generated from the Python docstrings; see tools/docs/generate_docs.py -->
## UART Endpoint
The UART endpoint makes it possible to test universal asynchronous receivers and transmitters.
Both full-duplex and half-duplex scenarios are supported.


## Technicals
- Maximum number of configured endpoints: 3.
- Receive buffer size: Unlimited.
- Maximum transmit size: 400 bytes.
- Bitrate between 2400bit/s and 5Mbit/s.
- Data bits count: 5-8.
- Parity: None, Odd, Even.
- Stop Bits: 1, 2.

## Configuration
This section lists the properties that can be configured for each `UART` endpoint.

:information_source: For half-duplex usage, set `DUT_RX` and `DUT_TX` to the same pin.

| Setting      | Description | Values |
| ----------- | ----------- | ------- |
| DUT_RX      | Package number for the RX pin on the DUT.  | DUT-dependent |
| DUT_TX      | Package number for the TX pin on the DUT.  | DUT-dependent |

### Example
```yaml
UART:
  UART2:
    DUT_RX: 122
    DUT_TX: 119
```

## General behavior
All terms are referring to the endpoint. Example: If data is _received_, it was transmitted by the DUT and received in the endpoint.

For this endpoint, a _byte_ always refers to the contents between the Start and Stop condition.
If a data bit count less than 8 is used, this means that the most significant bits are always 0.

At test start, the endpoint is in stopped mode and must first enabled by calling `start`.

The endpoint supports a strict mode that ensures tests are failing when the endpoint is used in a way not anticipated by the test.
Strict mode will be invalidated when:
- Received data was not read by the user with `get_received` or `get_received_with_details`.

Automatic Strict mode check is enabled by default and will fail in teardown. It can be disabled with `disable_automatic_strict_mode_check`.

## Setup & Teardown
`stop` is automatically called and the receive buffer is cleared after each test.

## Test frontend

Setup, controlling and observing functions are shown by their name. [Robot Framework](../TestFrontend/RobotFramework.md) and [pytest](../TestFrontend/pytest.md) explain how to call them on an endpoint instance.

### Setup functions

<a id="disable-automatic-strict-mode-check-keyword"></a>`disable_automatic_strict_mode_check()`
- Disable the strict mode check in Teardown for the current test.

<a id="start-keyword"></a>`start(Bitrate, Parity, StopBits, DataBits)`
- The receive buffer is cleared.
- Data is received.
- You are allowed to transmit data.
- `Bitrate`: between 2400bit/s and 5Mbit/s, e.g. `115.2 kbit/s`.
- `Parity`: No | Even | Odd
- `StopBits`: 1 | 2
- `DataBits`: 5 | 6 | 7 | 8

<a id="stop-keyword"></a>`stop()`
- Receiving data stops.
- You are not allowed to transmit any data.
- Loopback mode is disabled.

<a id="enable-loopback-keyword"></a>`enable_loopback()`
- Loopback mode short-circuits the `DUT_RX` and `DUT_TX` pins.
- Transmitting any data in loopback mode is not allowed and will result in an `EndpointUsageError`.
- You must call `start` before. These settings are used to record all received data in the receive buffer. It can be read with `get_received`.
- You can disable the loopback mode by calling `stop`.
- Note: Strict mode is by default also active in Loopback mode. You might want to disable it with `disable_automatic_strict_mode_check`.

### Controlling functions

<a id="transmit-keyword"></a>`transmit(Data: ByteData)`
- Transmits the given data.
- Returns: The _test time_ when the first start bit was sent.
- Blocks until all data was sent. If you do not want blocking behavior, use `transmit_periodically` below with `WaitForCompletion=False`.

<a id="transmit-periodically-keyword"></a>`transmit_periodically(Data: ByteData, Count=None, Pause="0ms", WaitForCompletion=True)`
- The endpoint sends the same data repeatedly for `Count` times.
- If `Count` is not set, data is sent until `stop` is called. `WaitForCompletion` must be set to `False`.
- The pause determines the time distance between the end of one full transmission of bytes and the start of the next transmission. Minimal pause is 0us, maximal 1s.
- Returns: The _test time_ when the first start bit of the first transmission was sent.

<a id="transmit-with-different-setup-keyword"></a>`transmit_with_different_setup(Data: ByteData, Bitrate=None, Parity=None, StopBits=None, DataBits=None)`
- Transmits the given data with other settings.
- `Bitrate`, `Parity`, `StopBits` and `DataBits` are optional. If left as `None`, the active setup from `start` is used. You must specify at least one of these parameters with a differing value, otherwise an `EndpointUsageError` is raised.
- `Parity`: No | Even | Odd
- `StopBits`: 1 | 2
- `DataBits`: 5 | 6 | 7 | 8
- Note: This method is intended to test error detection mechanisms on the DUT.
- Returns: The _test time_ when the first start bit was sent.

### Observing functions

<a id="check-strict-mode-keyword"></a>`check_strict_mode()`
- Fails test if strict mode is invalid.

<a id="get-received-keyword"></a>`get_received()`
- Returns data that was transmitted by the DUT.
- The return value is a [bytes type](../Endpoints.md#byte-data).
- Returns all bytes currently available in the receive buffer.
- An error is raised when any received data has a parity mismatch (only if parity is used) or when any received data does not observe the stop bit(s).

<a id="get-received-with-details-keyword"></a>`get_received_with_details()`
- Same as `get_received` but does not perform any error checking and returns a list of elements that each contain the following attributes:
    - `time`: The _test time_ when the start bit of this byte was seen.
    - `value`: A single byte.
    - `frame_status`:
        - Good
        - StopBitsWereNotObserved
        - StartBitDitNotComplete
    - `parity_status`:
        - Good
        - ParityErrorDetected
- Note: `parity_status` and `value` might be corrupted as a subsequent error when `frame_status` is not `Good`.
- Note: This method can be used to establish a precise timing between transmitted and received data bytes. For example, you might want to test DUT latency in responding to transmitted data. You calculate that latency from the return value of `transmit` and the _test time_ of the first received data byte.

<a id="wait-for-received-buffer-keyword"></a>`wait_for_received_buffer(ExpectedCount, Timeout="100ms")`
- Waits until `ExpectedCount` bytes are available in the receive buffer.
- There is no upper limit for `ExpectedCount`. Increase `Timeout` accordingly when waiting for a large number of bytes.

## Model functions

The UART endpoint has no model functions.

## Callbacks

This endpoint provides callbacks that can be used inside the [Python-based environment simulation](System.md).

```python
def on_bytes_received(self, callback: Callable[[bytes], None])
```
- Registers a callback that is called when at least one byte was received.
- The callback receives the bytes as one `bytes` object, which can hold several bytes.
- Only called for bytes transmitted by the DUT. Bytes sent by the endpoint itself are not passed to the callback.
- Only called for bytes with a `Good` `frame_status` and `parity_status`. Bytes with a frame or parity error do not trigger the callback.
- Note that all received bytes, including any with a frame or parity error, are still available in the test sequence through `get_received_with_details`. `get_received` instead raises an error if any received byte has a frame or parity error.

The following example shows the usage of this callback in the model for a UART Endpoint named `UART2`. The test sequence reads the output variable from the `SYSTEM` endpoint, not from `UART2`: `SYSTEM Get Output` in Robot Framework, `ep.SYSTEM.get_output()` in pytest. See [System](System.md).

```python
class SystemModel(config.Configuration):
    def __init__(self):
        self.outputs = {}
        self.outputs["data_buffer"] = bytearray()

    def data_buffer_received(self, data):
        self.outputs["data_buffer"].extend(data)

    def start(self):
        self.UART2.on_bytes_received(self.data_buffer_received)
```
