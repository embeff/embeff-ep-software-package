<!-- The keyword sections of this page are generated from the Python docstrings; see tools/docs/generate_docs.py -->
# Analog-Out Endpoint
The Analog-Out endpoint emits analog signals that make it possible to test ADC implementations.

## Technicals
- Maximum number of configured endpoints: 24. Using [ANALOG_MULTIPLEXER_8](ANALOG_MULTIPLEXER_8.md) endpoints: 192.
- Total DUT pins that can be connected to analog signals of any endpoint type: 24.
- Output voltage range: DUT-dependent (defined by `analog_ref_out` voltage), 5V max.
- Resolution: 14 bits
- Sample Rate: 200kSPS
- Differential Nonlinearity: max. 1 LSB
- Integral Nonlinearity: typ. &plusmn;3 LSB, max. &plusmn;16 LSB
- Offset Error: typ. &plusmn;1 LSB, max. &plusmn;9 LSB
- Gain Error: typ. &plusmn;0.2% FSR, max. &plusmn;0.7% FSR

## Configuration
Each `Analog-Out` endpoint instance has to be configured with the DUT-dependent package number connected to the instance or with the input of an [ANALOG_MULTIPLEXER_8](ANALOG_MULTIPLEXER_8.md) endpoint.

| Setting      | Description | Values |
| ----------- | ----------- | ------- |
| Pin      | Package number connected to the instance. The pin must be connected to an Analog_Out label in the DUT schematic. Alternatively one input of an [ANALOG_MULTIPLEXER_8](ANALOG_MULTIPLEXER_8.md) endpoint. | DUT-dependent, or `<ANALOG_MULTIPLEXER_8>.In0` to `<ANALOG_MULTIPLEXER_8>.In7` |

### Example
```yaml
ANOUT:
  PA0_ADC:
    Pin: 34
  PA1_ADC:
    Pin: 35
```

Note that the instance names (`PA0_ADC`, `PA1_ADC`) are arbitrary. You can exchange them to your liking.


## General behavior
For each DUT a maximum of 24 pins are available for operation with the Analog-Out endpoint.
Refer to your DUT schematic to see which pins have an Analog_Outx label. These pins are called _analog-out pins_ here.

Each Analog-Out endpoint is connected to one _analog-out pin_. Each _analog-out pin_ can be only connected to one Analog-Out endpoint at a time.

An Analog-Out endpoint can also connect to an input of an [ANALOG_MULTIPLEXER_8](ANALOG_MULTIPLEXER_8.md) endpoint. The multiplexer then connects up to 8 Analog-Out endpoints to one _analog-out pin_.

If a static or waveform signal would exceed the voltage range supported by the specific DUT, an error `MaxVoltage exceeded` is raised. The maximum voltage equals the analog out reference you find in your DUT schematic.

If a controlling function is called but the analog out endpoint is not connected to any pins, a `EndpointUsageError` is raised.

## Setup & Teardown
`reset` is automatically called after each test.


## Test frontend

Setup, controlling and observing functions are shown by their name. [Robot Framework](../TestFrontend/RobotFramework.md) and [pytest](../TestFrontend/pytest.md) explain how to call them on an endpoint instance.

### Setup functions

The Analog-Out endpoint has no setup functions.

### Controlling functions

<a id="reset-keyword"></a>`reset()`
- Resets the endpoint to initial conditions.
- No waveform generated.
- The output is set to 0V.

<a id="set-static-keyword"></a>`set_static(voltage)`
- Immediately sets the given `voltage`.

<a id="set-sine-keyword"></a>`set_sine(frequency, amplitude, base, count=1, TriggerPin=None, WaitForCompletion=True)`
- Generate a sine waveform with the given `frequency`, `amplitude` and `base`.
- The start value is `base + amplitude/2`.
- The `frequency` must be between 1 Hz and 10 kHz.
- If another waveform is active, an error `Waveform on this analog output already in progress.` is raised. Call `set_static` to abort any other waveform.
- The optional `count` controls the number of waveform repetitions.
- After the last repetition, the waveform stops and the output holds the last sample of the waveform.
- If `count` is set to `0xFFFF`, there will be unlimited repetitions until `set_static` is called. `WaitForCompletion` must be set to `False`.
- If the optional `TriggerPin` is specified, the waveform begins only when a rising edge occurs on this GPIO endpoint.
    - If specified, the start value of the waveform is applied immediately but the waveform begins only when the rising edge occurs.
    - If specified, the pin must be `LOW`. Otherwise an error `Trigger pin already high. Only low pins can be used as trigger.` is raised. See [Interaction with other endpoints](Analog-Out.md#interaction-with-other-endpoints) for more information.
    - The delay between trigger detection and waveform beginning is between 10us and 15us.

![Sine waveform parameters](Analog-Out-Sine.drawio.png)

<a id="set-square-keyword"></a>`set_square(frequency, amplitude, base, count=1, TriggerPin=None, WaitForCompletion=True)`
- Generate a symmetric square waveform with the given `frequency`, `amplitude` and `base`.
- The start value is `base`.
- The `frequency` must be between 1 Hz and 10 kHz.
- If another waveform is active, an error `Waveform on this analog output already in progress.` is raised. Call `set_static` to abort any other waveform.
- The optional `count` controls the number of waveform repetitions.
- After the last repetition, the waveform stops and the output holds the last sample of the waveform.
- If `count` is set to `0xFFFF`, there will be unlimited repetitions until `set_static` is called. `WaitForCompletion` must be set to `False`.
- If the optional `TriggerPin` is specified, the waveform begins only when a rising edge occurs on this GPIO endpoint.
    - If specified, the start value of the waveform is applied immediately but the waveform begins only when the rising edge occurs.
    - If specified, the pin must be `LOW`. Otherwise an error `Trigger pin already high. Only low pins can be used as trigger.` is raised. See [Interaction with other endpoints](Analog-Out.md#interaction-with-other-endpoints) for more information.
    - The delay between trigger detection and waveform beginning is between 10us and 15us.

![Square waveform parameters](Analog-Out-Square.drawio.png)

<a id="set-sawtooth-keyword"></a>`set_sawtooth(frequency, amplitude, base, count=1, TriggerPin=None, WaitForCompletion=True)`
- Generate a sawtooth waveform with the given `frequency`, `amplitude` and `base`.
- The start value is `base`.
- The `frequency` must be between 1 Hz and 10 kHz.
- If another waveform is active, an error `Waveform on this analog output already in progress.` is raised. Call `set_static` to abort any other waveform.
- The optional `count` controls the number of waveform repetitions.
- After the last repetition, the waveform stops and the output holds the last sample of the waveform.
- If `count` is set to `0xFFFF`, there will be unlimited repetitions until `set_static` is called. `WaitForCompletion` must be set to `False`.
- If the optional `TriggerPin` is specified, the waveform begins only when a rising edge occurs on this GPIO endpoint.
    - If specified, the start value of the waveform is applied immediately but the waveform begins only when the rising edge occurs.
    - If specified, the pin must be `LOW`. Otherwise an error `Trigger pin already high. Only low pins can be used as trigger.` is raised. See [Interaction with other endpoints](Analog-Out.md#interaction-with-other-endpoints) for more information.
    - The delay between trigger detection and waveform beginning is between 10us and 15us.

![Sawtooth waveform parameters](Analog-Out-Sawtooth.drawio.png)

### Observing functions

The Analog-Out endpoint has no observing functions.

## Model functions

These functions can be called from the [Python-based environment simulation](System.md).

```python
def set_static(self, voltage)
```
- Immediately sets the given `voltage`.

```python
def reset(self)
```
- Resets the endpoint to initial conditions.
- No waveform generated.
- The output is set to 0V.

## Callbacks

The Analog-Out endpoint has no callbacks.

# Interaction with other endpoints
## Synchronization
The waveform generation can be synchronized by using the `TriggerPin`. Multiple Analog-Out endpoints may use the same trigger, so that waveforms are started at the same time. Once a waveform has been triggered, it can not be retriggered.

`TriggerPin` can be set to any GPIO endpoint instance.
The rising edge can be driven both from the endpoint (set pin high, pulse creation) or by setting it from the DUT.
