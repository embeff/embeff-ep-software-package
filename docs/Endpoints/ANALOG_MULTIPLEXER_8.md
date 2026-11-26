# ANALOG_MULTIPLEXER_8 Endpoint
The ANALOG_MULTIPLEXER_8 endpoint simulates an 8:1 analog multiplexer with 3 select pins. The select pins are driven by the DUT. The 8 inputs are [Analog-Out](Analog-Out.md) endpoints.

The ANALOG_MULTIPLEXER_8 endpoint has no keywords.

## Technicals
- Maximum number of configured endpoints: 24.
- Total DUT pins that can be connected to analog output signals of any endpoint type: 24.
- Inputs: 8, `In0` to `In7`.
- Select pins: 3, `Sel0` to `Sel2`. `Sel0` is the least significant bit.
- Switch time: max. 20us.
- Output: the same voltage range, resolution and accuracy as an [Analog-Out](Analog-Out.md#technicals) endpoint.

## Configuration
This section lists the properties that can be configured for each `ANALOG_MULTIPLEXER_8` endpoint.

| Setting | Description | Values |
| ------- | ----------- | ------ |
| Sel0 | Select pin, least significant bit. | `<GPIO>.Pin` |
| Sel1 | Select pin. | `<GPIO>.Pin` |
| Sel2 | Select pin, most significant bit. | `<GPIO>.Pin` |
| Out | Package number of the output pin. The pin must be connected to an Analog_Out label in the DUT schematic. | DUT-dependent |

Inputs are connected to Analog-Out endpoints through their `Pin` setting. See [Analog-Out](Analog-Out.md#configuration)

### Example
```yaml
GPIO:
  PA0:
    Pin: 26
    Pull: None
  PA1:
    Pin: 27
    Pull: None
  PA2:
    Pin: 28
    Pull: None

ANALOG_MULTIPLEXER_8:
  Multiplexer1:
    Sel0: PA0.Pin
    Sel1: PA1.Pin
    Sel2: PA2.Pin
    Out: 34

ANOUT:
  SomeVoltageA:
    Pin: Multiplexer1.In0
  SomeVoltageB:
    Pin: Multiplexer1.In1
  SomeVoltageH:
    Pin: Multiplexer1.In7
```

## General behavior


The input that drives the output is select through pins `{Sel2, Sel1, Sel0}`. For example `Sel2 = LOW`, `Sel1 = HIGH`, `Sel0 = HIGH` selects `In3`.

Analog-Out endpoints connected to an ANALOG_MULTIPLEXER_8 endpoint behave exactly as if they were used standalone.

The output is 0V while an unpopulated input is selected.

The select pins are GPIO endpoints. The test can read them like any other GPIO endpoint. The same select pin can be used by more than one ANALOG_MULTIPLEXER_8 endpoint.
