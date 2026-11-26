<!-- The keyword sections of this page are generated from the Python docstrings; see tools/docs/generate_docs.py -->
## System endpoint
The System endpoint makes it possible to simulate external behavior during test runs.
External behavior is defined in a python file that can be loaded during the test.

The simulation is processed continuously and independent of the sequential test. Reactions to events are implemented by registering to callbacks that are provided by other endpoints.

:information_source: For this endpoint the System license package is required on your ExecutionPlatform.

## Technicals
- Maximum number of configured endpoints: 1.
- Parallel python-based simulation of external behavior.
- Real time guarantee for callbacks: 10ms.

## Configuration
There is no configuration necessary. The endpoint is automatically available when the System license is present on your device.

## General behavior
This endpoint can be used to run system tests for your firmware with the ExecutionPlatform. This firmware is potentially the same that runs later on your actual targets. It is not required to include the embeff software package or perform any interaction with the ExecutionPlatform.

:information_source: For system testing, remove the `System` block from the ep-config. Otherwise, `flash_firmware` and `power_on` of the [Code Interface](../CodeInterface.md) will block until the EP runtime starts up.

The test sequence uses `set_input` and `get_output` to interact with the model.

The model response time is validated for each callback automatically.
The response time is defined as follows: It starts when an Endpoint detects a new event and ends after the registered Python callback is fully executed.

If this response time exceeds the real time guarantee (see technical section), a violation is reported during Strict Mode Check.
Automatic Strict mode check is enabled by default and will fail in teardown. It can be disabled with `disable_automatic_strict_mode_check`.

### Python model format

The system behavior/reactions are defined in a python file that is stored next to the test sequence.
A file must comply with the following format when used in `start_model`. The functions a model can call on endpoints are listed in the "Model functions" and "Callbacks" sections of each endpoint page.

- Import your configuration (name without ending) that is also used in the corresponding test suite and alias it as `config`.
- Define a class `SystemModel` that derives from `config.Configuration`.
- Define at least the `start` method.
- Optionally define `__init__` to set initial values for input/output variables.
- Optionally define the `input_changed(variable,value)` method if you want to use `set_input`.
- Optionally define a number of callbacks and register them in `start`.

A model reaches an endpoint by its instance name in the ep-config (e.g. `self.GPIO1`) and this endpoint as `self.SYSTEM`.

The following python class shows the structure of a valid python system model.
```python
import <config_name> as config

class SystemModel(config.Configuration):
    def __init__(self):
        self.outputs = {}
        # Define all required outputs and set them to default values
        self.outputs['OutputA'] = 0
        self.InputA_current_value = None

    def input_changed(self, variable, value):
        # This function is called when the test sequence uses the `System Set Input` keyword.
        if variable == "InputA":
            self.InputA_current_value = value # Can be used across the whole model

    def gpio0_on_rising(self):
        # Reaction: Define what should happen on rising edge of GPIO0.
        # Endpoints of <config_name>.ep-config.zip that a model can use are available by name.
        self.GPIO1.set_high()
        self.outputs['OutputA'] += 1

    def start(self):
        # Register callbacks to react to pin changes
        # Check endpoint documentation for available callbacks.
        # Endpoints of <config_name>.ep-config.zip that a model can use are available by name.

        # Example: register method that should be executed on each rising edge of GPIO0
        self.GPIO0.on_rising(self.gpio0_on_rising)
```

### Code completion (VS Code, pyright)
For code completion in VS Code (Pylance) or any editor using pyright, add each ep-config used by your models to `extraPaths` in `pyproject.toml`, see [Importing ep-configs in Python](../SoftwarePackage.md#importing-ep-configs-in-python).
Code completion then shows all available endpoints and their functions (see screenshot below).

We highly recommend that you enable code completion for all your models.

:information_source: Pylance/Pyright code completion does not work reliably in VS Code multi workspace projects. This is a known limitation.

![Code completion configured properly](System-CodeCompletion.png)


### Example
:information_source: The example below uses `example.ep-config.zip` that defines 2 GPIO endpoints. For system tests, the `System` block must be omitted from the configuration.

Configuration `example.ep-config.yaml`
```yaml
GPIO:
  GPIO0:
    Pin: 2
    Pull: None
  GPIO1:
    Pin: 3
    Pull: None
```

Python model `example_model.py`
```python
import example as config


class SystemModel(config.Configuration):
    def __init__(self):
        self.outputs = {}
        self.outputs['RisingEdgeCount'] = 0

    def gpio0_on_rising(self):
        self.GPIO1.set_high()
        self.outputs['RisingEdgeCount'] += 1

    def gpio0_on_falling(self):
        self.GPIO1.set_low()

    def start(self):
        if self.GPIO0.read_as_bool():
            self.GPIO1.set_high()
        else:
            self.GPIO1.set_low()

        self.GPIO0.on_rising(self.gpio0_on_rising)
        self.GPIO0.on_falling(self.gpio0_on_falling)
```

The following test sequence checks that the model is running. It does not require any DUT code. In a typical usage, the GPIO0 would be driven by the DUT and GPIO1 would be read by the DUT.

<!-- tabs:start -->

#### **Robot Framework**

```robot
*** Test Cases ***
Model follows GPIO0 changes
    GPIO0 Set Low
    System Start Model    ${CURDIR}/example_model.py
    ${level_gpio1} =    GPIO1 Read
    Should Be Equal    ${level_gpio1}    LOW

    GPIO0 Set High        # Model reacts and sets GPIO1 to HIGH
    ${level_gpio1} =    GPIO1 Read
    Should Be Equal    ${level_gpio1}    HIGH
```

#### **pytest**

```python
import os

def test_model_follows_gpio0_changes(ep):
    ep.GPIO0.set_low()
    ep.SYSTEM.start_model(f"{os.path.dirname(__file__)}/example_model.py")
    assert ep.GPIO1.read() == "LOW"

    ep.GPIO0.set_high()     # Model reacts and sets GPIO1 to HIGH
    assert ep.GPIO1.read() == "HIGH"
```

<!-- tabs:end -->

## Setup & Teardown
`stop_model` is automatically called after each test.

## Test frontend

Setup, controlling and observing functions are shown by their name. [Robot Framework](../TestFrontend/RobotFramework.md) and [pytest](../TestFrontend/pytest.md) explain how to call them on an endpoint instance.

All functions on this page are called on the static endpoint `SYSTEM`, for example `SYSTEM Start Model` in Robot Framework or `ep.SYSTEM.start_model(file)` in pytest. `SYSTEM` needs no entry in the ep-config. See [Static endpoints](../Endpoints.md#static-endpoints).

### Setup functions

<a id="start-model-keyword"></a>`start_model(file)`
- Loads the python file to the ExecutionPlatform.
- If the python file is not conformant to the model structure mentioned above, an `EndpointUsageError` is raised.
- The models `start` function is executed.

<a id="disable-automatic-strict-mode-check-keyword"></a>`disable_automatic_strict_mode_check()`
- Disable the strict mode check in Teardown for the current test.

<a id="stop-model-keyword"></a>`stop_model()`
- Stops processing of any system model callbacks.

<a id="set-input-keyword"></a>`set_input(variable, value)`
- Transfer information from the test sequence to the model.
- Invokes the python model `input_changed` function with `variable` and `value`.
- If no model is running, an `EndpointUsageError` is raised.

### Controlling functions

The System endpoint has no controlling functions.

### Observing functions

<a id="check-strict-mode-keyword"></a>`check_strict_mode()`
- Fails test if strict mode is invalid.

<a id="get-output-keyword"></a>`get_output(variable)`
- Transfer information from the model to the test sequence.
- Returns the value of the key `variable` in the models `outputs` dictionary member variable.
- Still available after `stop_model` - e.g. to read a final
  failure count a long-running model accumulated before it was stopped.
- If `variable` is not found in the system model `outputs` dictionary, an `EndpointUsageError` is raised.
- If no model has been loaded, an `EndpointUsageError` is raised.

## Model functions

The System endpoint has no model functions.

## Callbacks

This endpoint provides callbacks that can be used inside the [Python-based environment simulation](System.md).

```python
def on_tick(self, interval, callback: Callable[[], None])
```
- Registers a tick callback that is called repeatedly each time the specified time interval elapses.

A simple example that increments an internal variable each 100ms.

```python
class SystemModel(config.Configuration):
    def __init__(self):
        self.outputs = {}
        self.outputs['tick'] = 0

    def timed_event(self):
        self.outputs['tick'] += 1

    def start(self):
        self.SYSTEM.on_tick("100ms", self.timed_event)
```
