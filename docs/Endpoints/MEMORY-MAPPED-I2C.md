<!-- The keyword sections of this page are generated from the Python docstrings; see tools/docs/generate_docs.py -->
# MEMORY-MAPPED-I2C
This endpoint simulates an I²C target.

- The user can load register contents that the DUT will receive for read transfers.
- The register contents are updated after each write transfer from the DUT.
- The user can subscribe to a callback to react to registers the DUT writes.
- The user can read the current register content at any time.

The endpoint is intended to be used for system tests where the target hardware contains I²C-based target peripherals  (such as gyroscope, RTC, ...) but the DUT does not. In this case, the DUT firmware acts as a I²C controller and can read/write registers on this peripheral.

To perform actions based on a write-transfer, the user subscribes to the `on_register_set` callback. Inside the callback you may change other parts of the registers as a reaction.

:information_source: For this endpoint the System license package is required on your ExecutionPlatform.

## Technicals
- Maximum number of configured endpoints: 3.
- Acts as an I²C-Target.
- Supported speeds
    - Standard-mode (Sm), with a bit rate up to 100 kBit/s
    - Fast-mode (Fm), with a bit rate up to 400 kBit/s
    - Fast-mode Plus (Fm+), with a bit rate up to 1 MBit/s
- Number of register address bits: 8

## Configuration
This section lists the properties that can be configured for each `MEMORY-MAPPED-I2C` endpoint.


| Setting      | Description | Values |
| ----------- | ----------- | ------- |
| SCL      | Package number for the SCL pin on the DUT.  | DUT-dependent |
| SDA      | Package number for the SDA pin on the DUT.  | DUT-dependent |
| DEVICE_ADDRESS  | 7-bit I²C address that the endpoint should respond to. | DUT-dependent |
| REGISTER_WIDTH_BITS | The size of each register word. If they are of different size, use the longest. | 8,16,32 |
| REGISTER_COUNT | Number of registers. | 1-256 |
| REGISTER_AUTO_INCREMENT | `Yes`: The selected register number is automatically incremented by 1 when REGISTER_WIDTH_BITS data bits are transferred.  | `No`, `Yes` |

### Example
```yaml
MEMORY_MAPPED_I2C:
  SENSOR_TCN75A:
    SCL: 50
    SDA: 51
    DEVICE_ADDRESS: 0x48
    REGISTER_WIDTH_BITS: 16
    REGISTER_COUNT: 4
    REGISTER_AUTO_INCREMENT: No
```

## General behavior
The endpoint is register based. The DUT can read or write registers.

Register selection happens through the first 8 bits of a write transfer. Subsequent data bits are then written to the selected register.
To read data, the register selection must happen first through a write transfer. Subsequent read transfers then return data from the selected registers.

Registers are always addressed by a 8-bit value, independent of the length of the register content that is set via `REGISTER_WIDTH_BITS`.

If `REGISTER_AUTO_INCREMENT` is set to `No`:
- a single read or write operation affects only a single register.

If `REGISTER_AUTO_INCREMENT` is set to `Yes`:
- a single read or write operation may affect multiple registers.
- the selected register number is automatically incremented by 1 when `REGISTER_WIDTH_BITS` data bits are transferred.


### Transfer examples
A write-transfer:
1. DUT: sends out the START condition.
2. DUT: sends out the device address with the R/W bit set to 0 (write-transfer).
    - Endpoint: ACK (if the address matches the configured `DEVICE_ADDRESS`)
    - No reaction otherwise.
3. DUT: sends out the 8-bit register address.
4. Endpoint: ACK
5. DUT: sends out the 8-bit data to write to the register selected in step 3.
6. Steps 4+5 can be repeated if necessary. If `REGISTER_AUTO_INCREMENT` is `No` this is restricted to transfer `REGISTER_WIDTH_BITS` in total.
7. DUT: sends out the STOP condition.

A read-transfer with address selection:
1. DUT: sends out the START condition.
2. DUT: sends out the device address with the R/W bit set to 0 (write-transfer).
    - Endpoint: ACK (if the address matches the configured `DEVICE_ADDRESS`)
    - No reaction otherwise.
3. DUT: sends out the 8-bit register address.
4. Endpoint: ACK
5. DUT: sends out the REPEATED START condition.
6. DUT: sends out the device address with the R/W bit set to 1 (read-transfer).
7. Endpoint: ACK
8. Endpoint: sends out the data from the selected register.
9. DUT: ACK
10. Steps 8+9 can be repeated if necessary. If `REGISTER_AUTO_INCREMENT` is `No` this is restricted to transfer `REGISTER_WIDTH_BITS` in total.
11. DUT: NACK
12. DUT: sends out the STOP condition.

:information_source: If a read-transfer happens without the write-transfer to set the register address, the last selected register address is used.

## Setup & Teardown
`reset` is automatically called after each test.

## Test frontend

Setup, controlling and observing functions are shown by their name. [Robot Framework](../TestFrontend/RobotFramework.md) and [pytest](../TestFrontend/pytest.md) explain how to call them on an endpoint instance.

### Setup functions

<a id="reset-keyword"></a>`reset()`
- Sets all registers to `0`.
- The selected register address is set to `0`.

### Controlling functions

<a id="set-register-keyword"></a>`set_register(register, value)`
- Sets `register` to `value`.
- If `register` is outside the configured `REGISTER_COUNT`, an `EndpointUsageError` is raised.
- If the size of `value` exceeds `REGISTER_WIDTH_BITS`, an `EndpointUsageError` is raised.

### Observing functions

<a id="read-register-keyword"></a>`read_register(address)`
- Returns data at the register `address`.

<a id="check-strict-mode-keyword"></a>`check_strict_mode()`
- Fails test if strict mode is invalid.

## Model functions

The Memory-Mapped-I2C endpoint has no model functions.

## Callbacks

This endpoint provides callbacks that can be used inside the [Python-based environment simulation](System.md).

```python
def on_register_set(self, callback: Callable[[int, int], None])
```
- Registers a callback that is called when the DUT wrote a register.
- The callback receives the register number and the new value of that register.
- Every write is reported, including one that writes the value the register already holds.
- Writes by the endpoints own `set_register` keyword are not reported.
- For registers wider than 8 bits, the callback is called once the DUT wrote the whole register. A partially written register is not reported.
- Note that the current content of every register is still available in the test sequence through `read_register`.
