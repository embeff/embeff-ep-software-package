<!-- The keyword sections of this page are generated from the Python docstrings; see tools/docs/generate_docs.py -->
# EEPROM-SPI
The EEPROM-SPI endpoint simulates a standard SPI-based serial EEPROM, such as
- the CAT25-series from onsemi,
- the 25LC-series from Microchip,
- the BR25A-series from ROHM.

The user can load memory contents that the DUT will receive when reading from the EEPROM.
The memory content is updated for each write-access from the DUT.
The user can read the current memory content at any time.

The endpoint is intended to be used for system tests where the target hardware contains such an EEPROM but the DUT does not.

:information_source: For this endpoint the System license package is required on your ExecutionPlatform.

## Technicals
- Maximum number of configured endpoints: 1
- Up to 20 MHz SPI speed.
- Support for standard OPCodes, including status registers and write protection.
- Support for Hardware Write Protection pin.
- Support for SPI mode (0,0).
- Not supported: Hold Transmission Input (nHOLD pin), Additional Identification Page.

## Configuration
This section lists the properties that can be configured for each `EEPROM-SPI` endpoint.

| Setting     | Description | Values |
| ----------- | ----------- | ------- |
| SI        | DUT pin that drives the SI pin.  | DUT-dependent |
| SO        | DUT pin that reads the SO pin.  | DUT-dependent |
| SCK         | DUT pin that drives the SCK pin. | DUT-dependent |
| nCS         | DUT pin that drives the active-low CS pin. Signal has Pull-Up in Endpoint. | DUT-dependent |
| nWP         | Optional DUT pin that drives the nWP (Hardware Write Protection) pin. If omitted, hardware write protection is disabled. Signal has Pull-Up in Endpoint.| DUT-dependent |
| SIZE_Kb        | The EEPROM size in Kilobit (Kb). One Kilobit (Kb) consists of 1,000 bits. | 128, 256, 512 |

### Example
```yaml
EEPROM_SPI:
  EEPROM1:
    SI: 20
    SO: 21
    SCK: 24
    nCS: 25
    SIZE_Kb: 512
```

## General behavior
The endpoint must not be explicitly enabled. If selected via the `nCS` pin, it will behave like a physical EEPROM and respond to the OPCodes (instruction set) mentioned below.
For a typical test scenario, the user might want to
- set known data (via the `set_memory` keyword) prior to starting the DUT.
- read data back (via the `read_memory` or `read_memory_address` keyword) and compare it against expected values to assert that the DUT wrote data.

The OpCodes and status register are mentioned here so that the user can check if the firmware driver is using only elements that are supported by this endpoint.

#### Supported OpCodes
| Instruction | OpCode | Operation |
| ---- | --------- | ------------------------ |
| READ | 0000 0011 | Read Data from Memory    |
| WRITE | 0000 0010 | Write Data to Memory    |
| WREN | 0000 0110 | Enable Write Operations  |
| WRDI | 0000 0100 | Disable Write Operations |
| RDSR | 0000 0101 | Read Status Register     |
| WRSR | 0000 0001 | Write Status Register    |

#### Internal 8-bit status register
| Bit | Name | Writeable (W) / Readable (R) |
| --- | ---- | ---------------------------- |
| 7   | WPEN | W/R                          |
| 6 | -    | -                         |
| 5 | -    | -                         |
| 4 | -    | -                         |
| 3   | BP1 | W/R                          |
| 2   | BP0 | W/R                          |
| 1   | WEL | R                          |
| 0   | WIP | R                          |

### Binary representation (hexdump format)
To exchange data with this endpoint, the keywords `set_memory` and `read_memory` use hexdump strings.
These strings are human-readable and can be easily compared against expected results.

Each row starts with a hexadecimal offset address followed by 16 hexadecimal data bytes.
Memory storing the following contents:
```
X=10
Y=20
Z=30
CalibrationInfo=1065
```

would be encoded in a hexdump as follows:
```
0000000 58 3d 31 30 0a 59 3d 32 30 0a 5a 3d 33 30 0a 43
0000010 61 6c 69 62 72 61 74 69 6f 6e 49 6e 66 6f 3d 31
0000020 30 36 35
0000023
```

Note: The file format is as produced by the standard linux [od](https://en.wikipedia.org/wiki/Od_(Unix)) tool with the command line options `-t x1 -v -Ax` (group into single bytes, not squeezing repeating contents into `*` characters, show offset in hexadecimal).

:warning: The standard od command emits data with an octal offset address. Make sure to add the `-Ax` parameter to change this to hex.

To use these hexdumps inside your tests, store them as a multiline string:

<!-- tabs:start -->

#### **Robot Framework**

Add the multiline string to the variables section:
```robot
*** Variables ***
${hexdump_example} =  SEPARATOR=\n
...  0000000 58 3d 31 30 0a 59 3d 32 30 0a 5a 3d 33 30 0a 43
...  0000010 61 6c 69 62 72 61 74 69 6f 6e 49 6e 66 6f 3d 31
...  0000020 30 36 35
...  0000023
```

#### **pytest**

```python
HEXDUMP_EXAMPLE = """\
0000000 58 3d 31 30 0a 59 3d 32 30 0a 5a 3d 33 30 0a 43
0000010 61 6c 69 62 72 61 74 69 6f 6e 49 6e 66 6f 3d 31
0000020 30 36 35
0000023"""

ep.EEPROM.set_memory(HEXDUMP_EXAMPLE)
```

<!-- tabs:end -->

## Setup & Teardown
`erase` is automatically called after each test.

## Test frontend

Setup, controlling and observing functions are shown by their name. [Robot Framework](../TestFrontend/RobotFramework.md) and [pytest](../TestFrontend/pytest.md) explain how to call them on an endpoint instance.

### Setup functions

<a id="erase-keyword"></a>`erase()`
- Sets all bits of the EEPROM to `1`.

<a id="set-memory-keyword"></a>`set_memory(data)`
- `data` is a hexdump string, see [Binary representation (hexdump format)](EEPROM-SPI.md#binary-representation-hexdump-format).
- All memory regions contained in `data` are updated. All other regions are unaffected.
- If `data` contains memory regions that exceed the EEPROMs configured `SIZE`, an `EndpointUsageError` is raised.
- This keyword should be called before the DUT attempts any communication. Otherwise, the results are undefined.

### Controlling functions

The EEPROM-SPI endpoint has no controlling functions.

### Observing functions

<a id="read-memory-address-keyword"></a>`read_memory_address(address, count)`
- Returns memory contents starting at `address` for `count` bytes in bytes.

<a id="read-memory-keyword"></a>`read_memory()`
- Returns all current memory contents as a hexdump string.

## Model functions

The EEPROM-SPI endpoint has no model functions.

## Callbacks

The EEPROM-SPI endpoint has no callbacks.
