<!-- The keyword sections of this page are generated from the Python docstrings; see tools/docs/generate_docs.py -->
# MEMORY-MAPPED-SPI
This endpoint simulates a SPI periphery that relies on an address-based memory model.

- The user can load memory contents that the DUT will receive when reading via SPI.
- The memory content is updated after each SPI write from the DUT.
- The user can read the current memory content at any time.

The endpoint is intended to be used for system tests where the target hardware contains SPI-based peripherals (such as gyroscope, RTC, ...) but the DUT does not. In this case, the DUT firmware can read/write registers on this peripheral.

:information_source: For this endpoint the System license package is required on your ExecutionPlatform.

## Technicals
- Maximum number of configured endpoints: 5.
- Up to 20 MHz SPI speed.

## Configuration
This section lists the properties that can be configured for each `MEMORY-MAPPED-SPI` endpoint.

| Setting      | Description | Values |
| ----------- | ----------- | ------- |
| SI        | DUT pin that drives the SI pin.  | DUT-dependent |
| SO        | DUT pin that reads the SO pin.  | DUT-dependent |
| SCK         | DUT pin that drives the SCK pin. Signal has Pull-Up in Endpoint. | DUT-dependent |
| nCS         | DUT pin that drives the active-low CS pin. Signal has Pull-Up in Endpoint. | DUT-dependent |
| ADDRESS_BITS_COUNT  | Number of bits that are used for the address. Excludes the RW_BIT. | 7,15 |
| REGISTER_WIDTH_BITS | The size of each register word. If they are of different size, use the longest. | 8,16,32 |
| REGISTER_COUNT | Number of registers. | 1-127 |
| RW_BIT_POSITION    | Position of the bit relative to the address that determines if the transfer is read or write. | `FIRST`, `LAST` |
| MODE_CPOL        | SPI Clock polarity. | 0,1 |
| MODE_CPHA        | SPI Clock phase. | 0,1 |

### Example
```yaml
MEMORY_MAPPED_SPI:
 GYRO:
  SI: 20
  SO: 21
  SCK: 24
  nCS: 25
  ADDRESS_BITS_COUNT: 7
  RW_BIT_POSITION: FIRST
  REGISTER_WIDTH_BITS: 8
  REGISTER_COUNT: 127
  MODE_CPOL: 0
  MODE_CPHA: 0
```

## General behavior
The endpoint expects that the DUT starts a SPI transfer by pulling nCS to LOW. Depending on `RW_BIT_POSITION` it then expects the DUT to clock the following content on the `SI` line
- `FIRST`: Transmit a read/write bit, followed `ADDRESS_BITS_COUNT` bits.
- `LAST`: `ADDRESS_BITS_COUNT` address bits, followed by a read/write bit.

If the read/write bit is `0`, all subsequent bits are interpreted as a read-request for the address specified in the address bits. The endpoint will output the corresponding value from its memory content on the `SO` pin.

If the read/write bit is `1`, all subsequent bits are interpreted as a write-request for the address specified in the address bits. The endpoint will reflect the values read on the `SI` pin to the corresponding value in its memory content.

A single read or write operation may affect multiple registers. The selected register address is automatically incremented by 1 when `REGISTER_WIDTH_BITS` data bits are transferred.

### Transfer example
The table below shows a transfer for the example `GYRO` configuration above. It encodes a 8-bit read from address 0x1A that contains the value 0x03.
In this case, the SPI transfer is 16 bits long. Subtracting 7 address bits and 1 R/W bit leaves 8 bits of data to be read.

x:  Don't care / ignore.

| Bit        |  0      |  1  |  2  |  3  |  4  |  5  |  6  |  7  |  8  |  9  | 10  | 11  | 12  | 13  | 14  | 15  |
|:----------:|:-------:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
| SI meaning | R/W bit | AD6 | AD5 | AD4 | AD3 | AD2 | AD1 | AD0 |  x  |  x  |  x  |  x  |  x  |  x  |  x  |  x  |
| SI value   |   0     |  0  |  0  |  1  |  1  |  0  |  1  |  0  |  x  |  x  |  x  |  x  |  x  |  x  |  x  |  x  |
| SO meaning |   x     |  x  |  x  |  x  |  x  |  x  |  x  |  x  | D7  | D6  | D5  | D4  | D3  | D2  | D1  | D0  |
| SO value   |   x     |  x  |  x  |  x  |  x  |  x  |  x  |  x  |  0  |  0  |  0  |  0  |  0  |  0  |  1  |  1  |


## Setup & Teardown
`reset` is automatically called after each test.

## Test frontend

Setup, controlling and observing functions are shown by their name. [Robot Framework](../TestFrontend/RobotFramework.md) and [pytest](../TestFrontend/pytest.md) explain how to call them on an endpoint instance.

### Setup functions

<a id="reset-keyword"></a>`reset()`
- Sets all register contents to `0`.

### Controlling functions

<a id="set-register-keyword"></a>`set_register(address, value)`
- Updates the register at `address` with the given `value`.
- If `address` is outside the configured `REGISTER_COUNT`, an `EndpointUsageError` is raised.
- If the size of `value` exceeds `REGISTER_WIDTH_BITS`, an `EndpointUsageError` is raised.

### Observing functions

<a id="read-register-keyword"></a>`read_register(address)`
- Returns register value at `address`.

## Model functions

The Memory-Mapped-SPI endpoint has no model functions.

## Callbacks

The Memory-Mapped-SPI endpoint has no callbacks.
