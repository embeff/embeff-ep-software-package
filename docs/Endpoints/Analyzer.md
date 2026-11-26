
# Analyzer Endpoint
The Analyzer Endpoint lets you view digital and analog signals that can help in understanding and debugging tests.

On the front plate of your ExecutionPlatform you can access 8 signals of your choice that can be compromised of:
- digital signals from any configured endpoint
- system signals
- analog signals from the [ANOUT](Analog-Out.md) or [ANIN](Analog-In.md) endpoint

Your configuration determines which signals are shown.

:information_source: Each analyzer channel can show any digital and system signal. The available analog signals for each analyzer channel are shown below in the configuration.

:information_source: Each analyzer channel can show digital signals with a frequency up to 25 MHz. Signals above that limit might be shown distorted.

| Analyzer |  Available analog signals |
|---|---|
| 0 | `Analog_In0`, `Analog_Out0`, `Analog_Out8`, `Analog_Out16`   |
| 1 | `Analog_In1`, `Analog_Out1`, `Analog_Out9`, `Analog_Out17`   |
| 2 | `Analog_In2`, `Analog_Out2`, `Analog_Out10`, `Analog_Out18`   |
| 3 | `Analog_In3`, `Analog_Out3`, `Analog_Out11`, `Analog_Out19`   |
| 4 | `Analog_In4`, `Analog_Out4`, `Analog_Out12`, `Analog_Out20`   |
| 5 | `Analog_In5`, `Analog_Out5`, `Analog_Out13`, `Analog_Out21`   |
| 6 | `Analog_In6`, `Analog_Out6`, `Analog_Out14`, `Analog_Out22`   |
| 7 | `Analog_In7`, `Analog_Out7`, `Analog_Out15`, `Analog_Out23`   |

:warning: Digital outputs are valid only when an test is running (that is when system signal SYSTEM.TEST_RUNNING is HIGH). Outside an active test, the signal is always LOW.

The digital outputs are 3.3V. The analog range depend of the analog reference voltage of your DUT.
The connector is designed to be fully compatible with the logic analyzers from [Saleae](https://www.saleae.com/) and its 4x2 pin headers.
We recommend Saleae, but you may use any other logic analyzer as well.

## Configuration

```
Analyzer:
  Output:
    '0': <Signal>
    '1': <Signal>
    '2': <Signal>
    '3': <Signal>
    '4': <Signal>
    '5': <Signal>
    '6': <Signal>
    '7': <Signal>
```

Possible values for `<Signal>` are
- `<Endpoint>.<Signal>` for digital signals referencing other endpoints (for example `PA0.Pin` or `CAN1.RX`)
- `<AnalogEndpoint>.Pin` for analog signals referencing an `ANOUT` or `ANIN` endpoint (for example `ADC0.Pin` or `DAC1.Pin`), as long as they are not connected to an `ANALOG_MULTIPLEXER_8` input.
- `<ANALOG_MULTIPLEXER_8>.Out` for the analog output of an `ANALOG_MULTIPLEXER_8` endpoint.
- `<ANALOG_MULTIPLEXER_8>.Sel0` to `<ANALOG_MULTIPLEXER_8>.Sel2` for the select pins of an `ANALOG_MULTIPLEXER_8` endpoint.

  Note that any analog assignments need to comply with the available analog signals listed above.
  For example for analyzer output 2, you may either use an
    1. `ANOUT` instance that itself uses a pin that is connected to `Analog_Out2/10/18`, or an
    2. `ANIN` instance that itself uses a pin that is connected to `Analog_In2`.
  Misconfigurations are detected and explained during ep-config generation.
- `<SystemSignal>` from the table below

### System signals
System signals are not related to a specific endpoint.

| Signal      | Description |
| ----------- | ----------- |
| SYSTEM.TEST_RUNNING      | Is automatically set `HIGH` when a test starts and set `LOW` when it ends. The [test time](GPIO.md) starts counting on the rising edge. |
| SYSTEM.UART.DUT_RX       | Data that the DUT receives from the ExecutionPlatform in order to establish runtime communication via UART. _Useful for debugging during BSP implementation or customization._ |
| SYSTEM.UART.DUT_TX       | Data that the DUT sends to the ExecutionPlatform in order to establish runtime communication via UART. _Useful for debugging during BSP implementation or customization._ |

### Example
```
Analyzer:
  Output:
    '0': SYSTEM.TEST_RUNNING
    '1': PA0.Pin
    '2': CAN1.RX
    '3': CAN1.TX
    '4': MY_DAC.Pin
    '5': MY_ADC.Pin
    '6': PB1.Pin
    '7': PB2.Pin

ANIN:
  MY_DAC:
    Pin: 20 # Pin 20 connected to `Analog_In4` on DUT schematic.

ANOUT:
  MY_ADC:
    Pin: 24 # Pin 24 connected to `Analog_Out13` on DUT schematic.
```

## Typical usage
1. Connect a Saleae Logic Analyzer to your ExecutionPlatform. Take care that pin 0 on the Saleae matches pin 0 on the ExecutionPlatform.
2. You configure Logic 2 (or any other trace software) with the same signals that are used in your configuration.
Assume you want to view the signals of an SPI communication. With the example configuration above, the 3 signals that are of interest are D4,D5,D6.
3. You start capturing.
4. You start the robot test.
5. You stop capturing.

![Wiring](AnalyzerWiring.jpg)

![Wiring](Analyzer_SPI_CRC_Saleae.PNG)
