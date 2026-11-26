<!-- The keyword sections of this page are generated from the Python docstrings; see tools/docs/generate_docs.py -->
## I2C-Target Endpoint
The I2C-Target endpoint makes it possible to test I²C-Controller implementations.
The endpoint implements a I²C-Target that can record write transfers, respond to read transfers, cause error scenarios and observe general bus parameters.

:information_source: This document uses terminology from the I²C Specification 7.0, in particular the terms _controller_ (master) and _target_ (slave).

## Technicals
- Maximum number of configured endpoints: 3.
- Act as a I²C-Target.
- Supported speeds
    - Standard-mode (Sm), with a bit rate up to 100 kBit/s
    - Fast-mode (Fm), with a bit rate up to 400 kBit/s
    - Fast-mode Plus (Fm+), with a bit rate up to 1 MBit/s
- 7-bit address mode.
- Number and individual size of response buffers: 2 / 200 bytes.
- Maximum number of data for single write transfer: 200 bytes.
- Maximum number of recorded transfers: Unlimited.
- Maximum number of total data in recorded write transfers: Unlimited.
- Generate _bus error_ and _arbitration lost_ error scenarios.


## Configuration
This section lists the properties that can be configured for each `I2C_TARGET` endpoint.

| Setting      | Description | Values |
| ----------- | ----------- | ------- |
| SCL      | Package number for the SCL pin the DUT.  | DUT-dependent |
| SDA      | Package number for the SDA pin on the DUT.  | DUT-dependent |

### Example
```yaml
I2C_TARGET:
  I2C1:
    SCL: 129
    SDA: 128
```


## General behavior
The user can prepare a response buffer so that the endpoint will respond to read-transfer by the controller with predefined data.
The user can inspect a transmit buffer that is filled by write-transfer from the controller.

The endpoint supports a strict mode that ensures tests are failing when the endpoint is used in a way not anticipated by the test.
Strict mode will be invalidated when:
- A read or write transfer occurred to an address not listed in `AckedAddresses`.
- Write transfers exceeded 200 bytes at once.
- A read transfer occurred but no (or not enough) data was prepared with `respond_to_read`.

Automatic Strict mode check is enabled by default and will fail in teardown. It can be disabled with `disable_automatic_strict_mode_check`.

## Setup & Teardown
`stop` is automatically called and all recorded transfers are cleared after each test.

## Test frontend

Setup, controlling and observing functions are shown by their name. [Robot Framework](../TestFrontend/RobotFramework.md) and [pytest](../TestFrontend/pytest.md) explain how to call them on an endpoint instance.

### Setup functions

<a id="start-keyword"></a>`start(AckedAddresses: List)`
- `AckedAddresses` is a list of 7-bit I2C device addresses. The endpoint will record all written data to any of these addresses.
- The endpoint acknowledges read transfers to any address in `AckedAddresses`. Use `respond_to_read` to define the returned data. Without a response, or once it is exhausted, the endpoint returns `0xFF` for every byte read and invalidates strict mode.
- The response and recorded transmit buffers are cleared.

<a id="stop-keyword"></a>`stop()`
- No more participation on the bus (transfers are not acknowledged and not stored).

<a id="respond-to-read-keyword"></a>`respond_to_read(Address, ResponseData: ByteData)`
- Prepares the endpoint to respond with the given data if the DUT initiates a read transfer to the given address.
- If multiple read transfers occur, the response data will be subsequently delivered.
- You may check how many data bytes were actually read by calling `get_read_transfers`.
- If `Address` was not listed in `AckedAddresses`, a `EndpointUsageError` is raised.

<a id="cause-arbitration-loss-keyword"></a>`cause_arbitration_loss()`
- Definition: An arbitration loss is the situation when a high level is sent on the SDA line, but a low level is sampled on the SCL rising edge
- During the next acknowledged write-transfer, the endpoint will forcibly hold-down SDA for all bits in the first data byte.
- Note: You must ensure that the data byte is not 0. Only then will an arbitration loss result.

<a id="cause-bus-error-keyword"></a>`cause_bus_error()`
- Definition: A bus error is the situation when the interface detects an SDAs rising or falling edge while SCL is high, occurring in a non-valid position during a byte transfer.
- During the next acknowledged write-transfer, the endpoint will forcibly cause a falling edge on SDA after the LSB of the first data byte. This happens between LSB and the subsequent bit while SCL is high.

<a id="disable-automatic-strict-mode-check-keyword"></a>`disable_automatic_strict_mode_check()`
- Disable the strict mode check in Teardown for the current test.

### Controlling functions

The I2C-Target endpoint has no controlling functions.

### Observing functions

<a id="get-all-written-data-keyword"></a>`get_all_written_data(Address)`
- Returns the combined data that was written with write transfers to the given `Address`.
- If `Address` is not an address that was given in `start`, a `EndpointUsageError` is raised.
- If there were multiple write transfers, this function aggregates all the data.
- If you need a fine-grained inspection, use `get_write_transfers` instead.

<a id="get-write-transfers-keyword"></a>`get_write_transfers()`
- Returns a list of all write transfers that were performed by the DUT.
- Each element in the list contains the following attributes:
    - `time`: The _test time_ when the _START_ or _REPEATED START_ condition was detected.
    - `address`: The device address.
    - `data`: All data bytes between _START_ / _REPEATED START_ and _STOP_.

<a id="wait-for-data-written-keyword"></a>`wait_for_data_written(Address, ExpectedCount, Timeout="100ms")`
- Waits until `ExpectedCount` data bytes are written to `Address`.
- Multiple transfers are aggregated in the count.
- There is no upper limit for `ExpectedCount`. Increase `Timeout` accordingly when waiting for a large number of bytes.
- Note: If this function returns, a subsequent call to `get_all_written_data` is guaranteed to return at least `ExpectCount` bytes.

<a id="wait-for-data-read-keyword"></a>`wait_for_data_read(Address, ExpectedCount, Timeout="100ms")`
- Waits until `ExpectedCount` data bytes are read from `Address`.
- Multiple transfers are aggregated in the count.
- There is no upper limit for `ExpectedCount`. Increase `Timeout` accordingly when waiting for a large number of bytes.

<a id="get-read-transfers-keyword"></a>`get_read_transfers()`
- Returns all transfers that were performed by the DUT in read-mode.
- Note: The endpoint records transfers to all addresses including those not in `AckedAddresses`. Data count for those will be `0`.
- Each element in the list contains the following attributes:
    - `address`: The device address.
    - `count`: Total number of data bytes read across all transfers.
    - `time`: The _test time_ when the _START_ or _REPEATED START_ condition was detected.

<a id="read-minimal-clock-period-keyword"></a>`read_minimal_clock_period(ReturnType="s")`
- Returns the length of the shortest cycle period observed on SCL. This includes all transfers that happened between `start` and `stop`, even when not acknowledged by this endpoint.
- The length is returned as a float in the given `ReturnType` unit.

<a id="read-maximal-clock-period-keyword"></a>`read_maximal_clock_period(ReturnType="s")`
- Returns the length of the longest cycle period observed on SCL. This includes all transfers that happened between `start` and `stop`, even when not acknowledged by this endpoint.
- The length is returned as a float in the given `ReturnType` unit.

<a id="check-strict-mode-keyword"></a>`check_strict_mode()`
- Fails test if strict mode is invalid.

## Model functions

The I2C-Target endpoint has no model functions.

## Callbacks

This endpoint provides callbacks that can be used inside the [Python-based environment simulation](System.md).

```python
def on_write_transfer(self, callback: Callable[[int, bytes], None])
```
- Registers a callback that is called when a write transfer completed.
- The callback receives the I2C address and bytes that were written.
- Note that all write transfers will still be available in test sequence through `get_write_transfers`.

```python
def on_read_transfer(self, callback: Callable[[int, int], None])
```
- Registers a callback that is called when a read transfer completed.
- The callback receives the I2C address and number of bytes that were read.
- Note that all read transfers will still be available in test sequence through `get_read_transfers`.

The following example shows the usage of this callback in the model for a I2C-Target Endpoint named `I2C0`.

```python
class SystemModel(config.Configuration):
    def __init__(self):
        self.outputs = {}

    def i2c0_on_write_transfer(self, address, data):
        pass

    def i2c0_on_read_transfer(self, address, number_of_bytes_read):
        pass

    def start(self):
        self.I2C0.on_read_transfer(self.i2c0_on_read_transfer)
        self.I2C0.on_write_transfer(self.i2c0_on_write_transfer)
```
