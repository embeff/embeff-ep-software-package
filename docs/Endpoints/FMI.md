<!-- The keyword sections of this page are generated from the Python docstrings; see tools/docs/generate_docs.py -->
# FMI Endpoint

The FMI endpoint runs dynamic simulation models that follow the [Functional Mock-up Interface (FMI)](https://fmi-standard.org/) standard.

Such models are packaged as so-called _Functional Mock-up Units_ (FMU) and run directly on the ExecutionPlatform. The unit is executed in real time and its inputs and outputs are wired to other endpoints, so the DUT interacts with the simulated environment through real electrical signals.

Typical use case: the DUT controls an actuator and reads back a sensor value. The plant behavior between them is described by a physical model created in a third-party tool such as [Modelica&reg;](https://modelica.org/) or [Simulink&reg;](https://www.mathworks.com/products/simulink.html).

:information_source: For this endpoint the System license package is required on your ExecutionPlatform.

## Technicals

- Maximum number of configured endpoints: 1.
- Supported FMI standard: FMI 2.0, Co-Simulation interface.
- The FMU must contain C source code; it is compiled on the ExecutionPlatform.
- Supported variable types: `Real` and `Boolean`.
- Endpoints usable as model inputs (read from the DUT): `PWM`, `GPIO`.
- Endpoints usable as model outputs (driven towards the DUT): `ANOUT`, `GPIO`.
- Model step size: taken from the FMU's `DefaultExperiment.stepSize`, fallback 1ms (1kHz).
- The smallest achievable step size depends on the computational requirements of the model. Typically between 60us and 200us.

## Configuration

The endpoint itself has no settings. Only one FMI instance is used and its name is a free label: the name is required for upward compatibility, but is currently not evaluated by the ExecutionPlatform.

| Setting      | Description | Values |
| ------------ | ----------- | ------ |
| *(none)*     | The endpoint has no properties. The instance entry is left empty. | - |

The endpoints that the model connects to are configured as usual in their own blocks.

### Example

```yaml
FMI:
  HEATING_FMU:
```

See the example at the end of this page for a full ep-config that also defines the endpoints used by the model.

## FMU requirements
The **link between model and hardware is established by name**: an FMU variable is connected to the endpoint instance of the same name.

- Model inputs must have an endpoint of the same name, otherwise `load_model` fails with `FMU input '<name>' has no matching endpoint`.
- Variables with causality `output` are connected if an endpoint of the same name exists. Outputs without a matching endpoint are skipped (they are then also not available via `get_output`).

Only the following endpoints can be connected to a model variable. The variable must use the causality and the FMI type listed here:

| Endpoint | Causality (from the model's point of view) | FMI type | Value in the model |
| -------- | --------- | ------------- | ------------------ |
| `GPIO`   | `input`   | `Boolean`     | Level of the pin driven by the DUT |
| `GPIO`   | `output`  | `Boolean`     | Level of the pin driven towards the DUT |
| `PWM`    | `input`   | `Real`        | Current duty cycle measured on the DUT signal, expressed as a value between 0.0 and 1.0 |
| `ANOUT`  | `output`  | `Real`        | Voltage in volts, driven towards the DUT |

- To have the `PWM` input provide duty cycle measurements to the model, you need to call its `start_measuring` function from the test sequence. If not started or the DUT signal is idle, `NaN` is provided to the model.

## Building the FMU

Any tool that exports an FMI 2.0 Co-Simulation FMU as source code can be used. Three details matter:

- Export with source code, not as a platform binary (a FMU containing only x86 binaries cannot run on the ExecutionPlatform).
- Name the model's input and output variables exactly like the endpoint instances they belong to.
- Set the simulation interval of the model to the step size you want (it ends up in `DefaultExperiment.stepSize`).

### Built-in support for OpenModelica

For models written in [OpenModelica](https://openmodelica.org/), our Software Package contains a CMake helper `OpenModelicaHelper.cmake` that generates the FMU automatically from the `.mo` files. For this the OpenModelica compiler (`omc`) must be in your `PATH`. It is used during the build:

```cmake
ep_add_openmodelica_fmu(
    my_model_fmu                                        # cmake target name
    MyModel                                             # Modelica model name
    "${CMAKE_CURRENT_SOURCE_DIR}/MyModel.mo"            # model file
    "${CMAKE_BINARY_DIR}/fmu"                           # output directory
)
```

The target produces `<output directory>/MyModel.fmu`, which can then be used with `load_model`.

If `omc` is not installed or not found in `PATH`, the target is skipped with a warning.

## General behavior

The model is uploaded from the test sequence with `load_model`, then started with `start_model`. While it is running, the ExecutionPlatform continuously runs one simulation step after the other. For each step:

1. All model inputs are read from the endpoints.
2. The values are handed to the FMU and one `fmi2DoStep` is performed.
3. All model outputs are read back from the FMU and applied to the endpoints.
4. The loop waits until the next step period is due.

The step period of the loop is the step size taken from the FMU (see [Technicals](#technicals)). The step size handed to `fmi2DoStep` is the elapsed wall-clock time since the previous step, so the model always advances by the time that really passed — including any overrun of a previous step.

:information_source: The ExecutionPlatform is a Processor-in-the-Loop test system. This means that the model inputs and outputs must be scaled to levels appropriate for the pin. For example, if your original plant model outputs 0V to 20V and the schematic uses a 1:10 voltage divider to bring this to the 0V to 3V range appropriate for an ADC, you need to add this voltage divider to the model.

## Setup & Teardown

`stop_model` is automatically called after each test.

## Test frontend

Setup, controlling and observing functions are shown by their name. [Robot Framework](../TestFrontend/RobotFramework.md) and [pytest](../TestFrontend/pytest.md) explain how to call them on an endpoint instance.

All functions on this page are called on the static endpoint `FMI`, for example `FMI Load Model` in Robot Framework or `ep.FMI.load_model(fmu_file)` in pytest. See [Static endpoints](../Endpoints.md#static-endpoints).

### Setup functions

<a id="load-model-keyword"></a>`load_model(fmu_file)`
- Uploads the FMU at path `fmu_file` to the ExecutionPlatform, compiles it if it is not cached yet, and maps its variables to the configured endpoints.
- The model is not yet running afterwards.
- Depending on the model, the compilation might take several minutes. It is cached by file content, so loading the same FMU again is fast.

<a id="start-model-keyword"></a>`start_model()`
- Starts the simulation loop. Requires that a model was loaded with `load_model` before.
- Calling it while the model is already running has no effect.

<a id="stop-model-keyword"></a>`stop_model()`
- Stops the simulation loop.

### Controlling functions

The FMI endpoint has no controlling functions.

### Observing functions

<a id="get-output-keyword"></a>`get_output(variable)`
- Returns the value that the model last wrote to the output variable `variable`, in the unit of the model (e.g. volts for an `ANOUT` output).

## Model functions

The FMI endpoint has no model functions.

## Callbacks

The FMI endpoint has no callbacks.

## Example: a thermostat with a simulated heater

This example shows a system in which a RP2040 microcontroller drives a heater using a two-point control algorithm to reach a certain target temperature.
The plant model is simulated. A simple system test checks that the DUT firmware manages to reach the target temperature.

The DUT measures a temperature with its ADC and switches a heater output accordingly. The model turns that heater output back into a temperature and provides it as the sensor voltage:

```
DUT (RP2040)                                   ExecutionPlatform
GPIO16  heater      (output) ───────────────►  GPIO16 endpoint  ───►  model input  GPIO16 (Boolean)
ADC0    sensor      (input)  ◄───────────────  ADC0 endpoint    ◄───  model output ADC0   (Real, volts)
```

Four parts play a role in this test:
1. The plant model defines the physics and the mapping to the DUT's inputs and outputs.
2. The ep-config defines endpoints whose names match the interface variables of the plant model.
3. The firmware measures the sensor and controls the heater.
4. The test brings everything together.

### 1. The plant model
The plant model is written with OpenModelica.

While `GPIO16` is high, `HeatingSwitch` connects the 10V source to the resistor, which heats the heat capacitor. `ThermalConductor` cools it towards the ambient temperature. The measured temperature is scaled to a sensor voltage that emulates a TMP36-style sensor with 0.5V offset and 50mV/°C:

![Heating model in OpenModelica: the model input `GPIO16` and the model output `ADC0` are named after the endpoint instances they are connected to](FMI-HeatingOM.png)

Only two things in the model are specific to its use with the FMI endpoint:
- the names and types of the interface variables
- the step size

```modelica
Modelica.Blocks.Interfaces.BooleanInput GPIO16;    // model input:  heater switch driven by the DUT
Modelica.Blocks.Interfaces.RealOutput ADC0;        // model output: sensor voltage driven to the DUT

// part of the model's annotation:
annotation(experiment(StopTime = 10, Interval = 0.001));    // Interval -> 1ms step size (1kHz)
```

### 2. The ep-config

```yaml
GPIO:
  GPIO16:
    Pin: 27
    Pull: None

ANOUT:
  ADC0:
    Pin: 38

FMI:
  HEATING_FMU:

Analyzer:
  Output:
    "0": SYSTEM.TEST_RUNNING
    "1": ADC0.Pin
    "2": GPIO16.Pin
```

- `GPIO16` is the model input. The endpoint only reads the pin, the DUT drives it.
- `ADC0` is the model output. An `ANOUT` endpoint is required because the model provides a voltage.
- The `Analyzer` block is optional. It routes the involved signals to the analyzer header, which is useful while bringing up a new model.

### 3. The firmware

The firmware is a plain application: it does not know about the ExecutionPlatform and contains no test hooks.

```cpp
static const float desiredTemperature = 20.0f;
static const float controlHysteresis = 1.0f;

static const unsigned int ADC_GPIO_OFFSET = 26;
static const unsigned int ADC_TEMP_SENSOR = 0;
static const unsigned int HEATER_GPIO = 16;

int main() {
    // ... clock, GPIO and ADC initialisation ...

    bool heaterOn = false;

    while (1) {
        // 12-bit resolution, ADC_AVDD = 3.0 V
        const float conversion_factor = 3000.0f / (1 << 12);
        uint16_t result = averageADC();

        float voltage_mV = static_cast<float>(result) * conversion_factor;

        // Sensor scaling of the model: 0.5V offset, 50mV/degree celsius
        float currentTemperature = (voltage_mV - 500) / 50;

        float error = currentTemperature - desiredTemperature;

        if (error < -controlHysteresis)
            heaterOn = true; // Too cold

        if (error > controlHysteresis)
            heaterOn = false; // Too hot

        gpio_put(HEATER_GPIO, heaterOn);
    }
}
```

### 4. The test sequence

<!-- tabs:start -->

#### **Robot Framework**

```robot
*** Settings ***
Library           EPLibrary   ${ep_url}    ${ep_binary_dir}/fmi.ep-config.zip
Suite Setup       Suite Setup
Resource          ${ep_app_dir}/cmake.resource

*** Keywords ***
Suite Setup
    CMake Build    ${ep_binary_dir}    fmi_thermostat   # DUT binary
    CMake Build    ${ep_binary_dir}    fmi_thermostat_fmu  # FMU build produced by ep_add_openmodelica_fmu
    GPIO16 Set Tristate    # the model reads this pin, so the EP must not drive it
    FMI Load Model    ${ep_binary_dir}/fmu/ControlledTemperature_EP.fmu
    DUT Flash Firmware    ${ep_binary_dir}/fmi_thermostat.hex    # powers the DUT before the model drives its pins
    FMI Start Model

*** Test Cases ***
Temperature is between 10 and 30 Degrees Celsius after initial 4s
    Sleep    4.0s

    ${voltage} =    FMI Get Output    ADC0
    # Convert voltage to temperature using the formula from the firmware/model.
    ${temperature} =    Evaluate    (${voltage} - 0.5) / 0.05

    Should Be True    10 < ${temperature} < 30
```

#### **pytest**

```python
import os
import time

import pytest
from cmake_helper import cmake_build
from fmi.test import EP    # imports fmi.ep-config.zip

BINARY_DIR = os.environ["EP_BINARY_DIR"]

ep = EP.fixture()


@pytest.fixture(scope="module", autouse=True)
def suite_lifecycle(ep: EP):
    cmake_build(BINARY_DIR, "fmi_thermostat")       # DUT binary
    cmake_build(BINARY_DIR, "fmi_thermostat_fmu")   # FMU build produced by ep_add_openmodelica_fmu
    ep.GPIO16.set_tristate()    # the model reads this pin, so the EP must not drive it
    ep.FMI.load_model(f"{BINARY_DIR}/fmu/ControlledTemperature_EP.fmu")
    ep.DUT.flash_firmware(f"{BINARY_DIR}/fmi_thermostat.hex")    # powers the DUT before the model drives its pins
    ep.FMI.start_model()
    yield


def test_temperature_is_between_10_and_30_degrees_after_initial_4s(ep: EP):
    time.sleep(4.0)

    voltage = ep.FMI.get_output("ADC0")
    # Convert voltage to temperature using the formula from the firmware/model.
    temperature = (voltage - 0.5) / 0.05

    assert 10 < temperature < 30
```

<!-- tabs:end -->
