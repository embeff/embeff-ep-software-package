<!-- The keyword sections of this page are generated from the Python docstrings; see tools/docs/generate_docs.py -->
## SPI-Slave Endpoint
The SPI-Slave endpoint makes it possible to test SPI-Master implementations.
The endpoint implements a slave spi device that can be controlled and observed.

## Technicals
- Maximum frequency: 20 MHz.
- Maximum number of configured endpoints: 2.
- Internal buffer size for data: 256 bytes.
- SPI word length: 8 bit.
- SPI data is transmitted MSB first.

## Configuration
This section lists the properties that can be configured for each `SPI` endpoint.

| Setting      | Description | Values |
| ----------- | ----------- | ------- |
| MOSI      | Package number for the MOSI pin.  | DUT-dependent |
| MISO      | Package number for the MISO pin.  | DUT-dependent |
| SCK      | Package number for the SCK pin. Signal has Pull-Up in Endpoint | DUT-dependent |
| nCS0      | Package number for the active-low CS0 pin. Signal has Pull-Up in Endpoint | DUT-dependent |

### Example
```yaml
SPI:
  SPI1:
    SCK: 35
    MISO: 42
    MOSI: 43
    nCS0: 40
```

### SPI timing
![SPI timing, Creative Commons 3.0, https://en.wikipedia.org/wiki/File:SPI_timing_diagram2.svg ](SPI_timing_diagram.svg "SPI timing")

## General behavior
The endpoint supports a strict mode that ensures tests are failing when the endpoint is used in a way not anticipated by the test.
Strict mode will be invalidated when:
- clocks are received on SCKL but no (sufficient long) response was set with `set_response_bytes`
- incomplete byte was received (not 8 bits)

Automatic Strict mode Check is enabled by default and will fail in teardown, it can be disabled with `disable_automatic_strict_mode_check`.

## Setup & Teardown
`reset` is automatically called after each test.

## Test frontend

Setup, controlling and observing functions are shown by their name. [Robot Framework](../TestFrontend/RobotFramework.md) and [pytest](../TestFrontend/pytest.md) explain how to call them on an endpoint instance.

### Setup functions

<a id="reset-keyword"></a>`reset()`
- Resets the endpoint to initial conditions.
- No clock periods measured.
- No bytes received.
- No response set.
- Use /CS0.
- Strict Mode enabled.
- Mode: `CPOL=0`, `CPHA=0`.

<a id="set-mode-keyword"></a>`set_mode(CPOL, CPHA)`
- Sets mode according to parameters.
- `CPOL`: Clock polarity. `CPOL=0`: Clock idle is low, `CPOL=1`: Clock idle high.
- `CPHA`: Clock phase. Data is applied at the first (`CPHA=0`) or second (`CPHA=1`) edge after CS0 is pulled low.

<a id="set-response-bytes-keyword"></a>`set_response_bytes(data: ByteData)`
- The endpoint will respond with this `data` on MISO if clocks are received on SCKL.
- The `data` is interpreted as [ByteData](../Endpoints.md#byte-data).
- The maximum `data` length equals the internal buffer size.
- If strict mode is enabled and clocks are received on SCKL but no (sufficient long) response was set, strict mode is invalidated.
- Example to instruct the endpoint to respond with 0xAB on a 1 byte read code with 1 arbitrary address byte:

    ```
    SPI1 Set Response Bytes     \x00\x00\xAB
    ```
    Here the response to the address (first byte received) is 0, as is the response to the read code (second byte received). The actual response begins only when the DUT starts clocking out the third byte.

<a id="set-chip-select-use-to-keyword"></a>`set_chip_select_use_to(CS)`
- `CS`: `Ignore` | `/CS0`.
- When using `Ignore`, the chip select is not evaluated. Data is only read/written with regard to SCKL.
- When using a specific `CS`, data is read/written with regard to SCKL and the respective CS line.

<a id="disable-automatic-strict-mode-check-keyword"></a>`disable_automatic_strict_mode_check()`
- Disable the strict mode check in Teardown for the current test.

### Controlling functions

The SPI-Slave endpoint has no controlling functions.

### Observing functions

<a id="read-written-bytes-keyword"></a>`read_written_bytes()`
- Returns a list of bytes written by the DUT.
- The return value is a [bytes type](../Endpoints.md#byte-data).
- The maximum length equals the internal buffer size. If more data is written, the bytes written first are returned.

<a id="read-minimal-clock-period-keyword"></a>`read_minimal_clock_period(ReturnType="s")`
- Returns the length of the shortest cycle period observed on SCKL.
- The length is returned as a float in the given `ReturnType` unit.
- Time resolution: 10ns.
- Measurement accuracy < 20ns.

<a id="read-maximal-clock-period-keyword"></a>`read_maximal_clock_period(ReturnType="s")`
- Returns the length of the longest cycle period observed on SCKL.
- The length is returned as a float in the given `ReturnType` unit.
- Note that this may include (longer) pauses between data when multiple bytes are sent by the DUT.
- Time resolution: 10ns.
- Measurement accuracy < 20ns.

<a id="check-strict-mode-keyword"></a>`check_strict_mode()`
- Fails test if strict mode is invalid.

## Model functions

The SPI-Slave endpoint has no model functions.

## Callbacks

The SPI-Slave endpoint has no callbacks.

# Interaction with other endpoints
1. You may use an additional GPIO endpoint instance on the chip select pins to test error scenarios.

    In normal operation the chip select signal is only controlled by the SPI master. Some DUTs support monitoring the CS line for errors. For these devices, manually overriding the signal level to LOW can be used to trigger and test against such an error condition. To implement such a test, you may use the GPIO endpoints `set_low` or `GPIO Generate Low Pulse On Pin` functions.

2. You may use an additional GPIO endpoint instance on the MISO pin to enable a certain pull resistor.

    When no data is set with `set_response_bytes` and strict mode is disabled, the endpoint will respond to read requests with high impedance. If the DUT does not support a physical pullup/pulldown resistor, you can use the GPIO endpoints pull configuration to ensure defined signal levels.
