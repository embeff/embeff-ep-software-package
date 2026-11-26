:information_source:This documentation applies to the latest ExecutionPlatform hardware revision (Rev-E). Click <a href="rev-d/#">here</a> for Rev-D (software versions prior to 9.0.0).

# What is the ExecutionPlatform?

The ExecutionPlatform (EP) is a Processor-in-the-Loop test system for microcontrollers. It runs your code on the real microcontroller and gives your tests access to all of its pins. You use it for unit, integration and system tests.

Each ExecutionPlatform consists of our generic test hardware and a DUT board carrying the microcontroller you chose. No further measurement or instrumentation hardware is required. The EP flashes and powers the microcontroller, generates and measures its digital and analog signals, speaks its bus protocols and can simulate the environment the microcontroller is built into.

The EP is a network device. Tests are written in [Robot Framework](https://robotframework.org/) or [pytest](https://docs.pytest.org/en/stable/), run on your PC or CI server and connect to the EP over the network. Every step, from flashing to evaluating the result, is automated, so the EP fits naturally into a continuous integration pipeline.

![Your tests connect to the ExecutionPlatform over the network. The DUT board with your microcontroller plugs into the ExecutionPlatform.](img/ep-setup.drawio.png ':size=690')

## Processor-in-the-Loop
Test approaches differ in where the code under test runs and how the test environment acts on it:

| Approach | Code runs on | Test environment acts through |
| --- | --- | --- |
| Software-in-the-Loop (SiL) | a PC | function calls |
| Processor-in-the-Loop (PiL) | the microcontroller | the pins of the microcontroller |
| Hardware-in-the-Loop (HiL) | the microcontroller on the final hardware | the inputs and outputs of the device |

The ExecutionPlatform is a PiL system.

# Key concepts
A test on the EP combines a few building blocks. Each of them has its own chapter in this documentation. This section explains what they are and how they fit together.

![The test sequence connects to the ExecutionPlatform. The Code Interface reaches the firmware on the DUT, endpoints such as CAN1, PA1 and TEMP connect to DUT pins, and an environment model acts on the DUT through the endpoints.](img/ep-architecture.drawio.png ':size=700')

## DUT

Your microcontroller is connected to the EP with a plug-in board. We call this board Device-under-Test (DUT).
This board makes pins and programming interface accessible to the test system. You design it with our template or order a ready-to-use one at embeff.

## Endpoints
[Endpoints](Endpoints.md) are the interface between a test and the pins of the DUT. Each endpoint type implements one kind of periphery and acts as the counterpart of the DUT: the GPIO endpoint drives and measures digital signals, the CAN endpoint is a node on the CAN bus of the DUT, the SPI-Slave endpoint behaves like an external SPI chip. You assign each endpoint to the DUT pins of your choice.

Endpoints run in hardware on the EP, independent of the PC that executes the test. Signals are therefore generated and measured precisely.

The endpoint types fall into three groups:

- Basic: [GPIO](Endpoints/GPIO.md) for digital signals, and the [Analyzer](Endpoints/Analyzer.md), which routes up to 8 signals to the front plate for an external logic analyzer.
- Peripheral: counterparts for the peripherals of a microcontroller, such as [UART](Endpoints/UART.md), [SPI](Endpoints/SPI-Slave.md), [I²C](Endpoints/I2C-Target.md), [CAN](Endpoints/CAN.md), [CAN-FD](Endpoints/CAN-FD.md), [Ethernet](Endpoints/Eth-PHY.md), [PWM](Endpoints/PWM.md), [incremental encoders](Endpoints/Incremental-Encoder.md) and [analog inputs](Endpoints/Analog-In.md) and [outputs](Endpoints/Analog-Out.md).
- Simulation: endpoints that replace parts of the target hardware in system tests. They emulate external chips such as an [SPI EEPROM](Endpoints/EEPROM-SPI.md), register-based [SPI](Endpoints/MEMORY-MAPPED-SPI.md) and [I²C](Endpoints/MEMORY-MAPPED-I2C.md) peripherals or an [analog multiplexer](Endpoints/ANALOG_MULTIPLEXER_8.md), [follow a signal](Endpoints/Pin-Follow.md) with a precise delay, or run environment models ([System](Endpoints/System.md), [FMI](Endpoints/FMI.md)).

Endpoints offer their functionality as keywords. Two examples:

- The GPIO endpoint is configured for a single pin. It sets the pin high, low or tristate, reads its level, generates pulses and measures the time between edges.
- The SPI-Slave endpoint is configured for the SCK, MISO and MOSI pins and optionally a chip select. It records the bytes the DUT writes, which the test can evaluate, and answers with the response bytes the test has set.

## ep-config
The [ep-config](EP_Config.md) determines which endpoints are available and which DUT pins they use. You write it in YAML and build it in the Customer Portal or with its [command line tool](EP_Config.md#command-line-usage-ep-config), which produces an `.ep-config.zip` file. A build typically takes 10 to 30 minutes, because it creates the hardware configuration of the EP for exactly these endpoints.

Each endpoint instance has a name of your choice, and this name becomes part of its keywords. A GPIO endpoint named `PA1` provides keywords such as `PA1 Read` and `PA1 Set High`, a CAN endpoint named `CAN1` provides `CAN1 Start` and `CAN1 Send Frame`. In pytest, the name is an attribute of the EP handle: `ep.PA1.read()`, `ep.CAN1.send_frame()`.

![A YAML file defines the GPIO endpoint PA1 on pin 35 and the CAN endpoint CAN1 on pins 20 and 19. The Customer Portal builds it into demo.ep-config, which EPLibrary loads in the test suite. The test then has keywords such as PA1 Set High, PA1 Read, CAN1 Start and CAN1 Send Frame.](img/ep-config-flow.drawio.png ':size=760')

Pin values are package pin numbers of the DUT. Each test suite loads the ep-config it needs, so different suites can use different configurations. The optional `System` block decides the [test mode](#integration-test-mode-and-system-test-mode).

## Test sequences
Test sequences are written in Robot Framework or pytest. The [Software Package](SoftwarePackage.md#use-software-package-to-write-and-run-tests) connects both to the EP: it takes the address and credentials of your EP and an ep-config, and provides the keywords of the Code Interface and of all endpoints in this configuration.

[Robot Framework](TestFrontend/RobotFramework.md) and [pytest](TestFrontend/pytest.md) show how a test suite loads an ep-config and calls the functions of its endpoints.

Some rules apply to every test, regardless of the endpoint:

- Endpoints are reset after each test. GPIOs return to tristate, buses stop, buffers are cleared and models stop. Each test sets up the endpoints it needs.
- The DUT is powered off when a test suite starts and when it ends. Between the tests of a suite it stays powered and is not reset.
- Keywords that return a time use the [test time](Endpoints.md#test-time). It restarts with every test and is shared by all endpoints, so the difference between two test times is precise.
- Endpoints with a strict mode, such as CAN, UART or System, watch for errors during the whole test, for example error frames or missed real-time deadlines. A violation fails the test in its teardown.
- [Physical values](Endpoints.md#physical-units) are given with their unit, for example `52ms`, `500kbit/s` or `1.5V`.

## Code Interface
The [Code Interface](CodeInterface.md) controls the DUT itself. It flashes firmware, switches the DUT power, calls functions in the firmware and collects their standard output and coverage data.

Calling functions requires a test binary that links the EP runtime from the software package. The firmware registers functions with `ep_register`, and the test calls them with `DUT Invoke` (`ep.DUT.invoke()` in pytest). The runtime talks to the EP over a UART, which your BSP provides and the `System` block of the ep-config describes.

[Apps](SoftwarePackage.md#apps) build on the runtime for specific use cases: `generic` to call any registered function, `unity` and `gtest` for [on-target unit tests](Usecase/UnitTest.md), and `bench` for [runtime measurements](Usecase/Benchmark.md).

While a test runs, you can [debug the firmware](DUT-Debugging.md) on the DUT.

## Integration test mode and system test mode

![In integration test mode, the DUT runs a test binary with your code under test, glue functions, the EP runtime and the BSP, and the Code Interface talks to it over UART. In system test mode, the DUT runs production firmware without the EP runtime, and the Code Interface only flashes and powers it. Endpoints work the same in both modes.](img/test-modes.drawio.png ':size=720')

| | Integration test mode | System test mode |
| --- | --- | --- |
| ep-config | with `System` block | without `System` block |
| DUT runs | a test binary that links the EP runtime | your firmware without changes, typically the production build |
| Test uses | Code Interface (`DUT Invoke`, `DUT Get Output`, ...) and endpoints | endpoints; flashing and power control remain available |
| Typical tests | on-target unit tests, benchmarks, HW/SW integration tests of drivers and HAL | system tests of the complete firmware |

## Environment models
In a system test, the firmware often needs an environment that reacts: a temperature sensor that follows the heater the firmware switches, a CAN node that answers requests. Environment models provide this. They run on the EP in real time, in parallel to the test sequence, and act on the DUT through endpoints.

- Python models run in the [System endpoint](Endpoints/System.md). A model registers callbacks for endpoint events, such as a rising edge, a received CAN frame or a timer tick, and reacts with model functions of other endpoints. Each callback is checked against a real-time limit of 10 ms.
- Dynamic models run in the [FMI endpoint](Endpoints/FMI.md) (experimental). It executes a Functional Mock-up Unit (FMU) exported from tools such as OpenModelica or Simulink&reg;, and connects its inputs and outputs to the endpoints of the same name.

## Software Package, Customer Portal and device
- The [Software Package](SoftwarePackage.md) contains everything you need to build test binaries and run tests: the EP runtime, the CMake infrastructure, examples, and this documentation as Markdown together with an `AGENTS.md` for AI coding assistants. Its `requirements.txt` installs the Python package [embeff-ep](https://github.com/embeff/embeff-ep) with the Robot Framework library `EPLibrary`, the pytest plugin and the `ep-config` command line tool.
- The [Customer Portal](https://customer.embeff.com) is where you download software packages and device updates, find the schematic, pin mapping and BSP of your DUT, and write and build your ep-configs.
- The [web interface](Service.md) of your EP at `http://<ep-serial-number or ip>` manages users, network settings, the license and system updates.
- EP, software package and ep-configs share one version number and must match in major and minor version. After a major or minor update, you rebuild your test binaries and ep-configs. See [Version Compatibility](VersionCompatibility.md).
- The [license](Service.md#license) of your EP determines which endpoints you can use. The Code Interface and the Analyzer are always available.
- An EP runs one test suite at a time. Test suites of other users wait until it has finished.

# Choosing the test level
Choose the lowest test level that can show the behaviour you want to verify. Lower levels give faster feedback and can typically run more often.

![Test pyramid: host unit tests on your PC at the base, then on-target unit tests, HW/SW integration tests and system tests on the ExecutionPlatform.](img/test-levels.drawio.png ':size=680')

| What you test | Test level | How |
| --- | --- | --- |
| Pure logic without hardware dependencies | Host unit tests | On the PC, outside the EP. |
| Compiler- or MCU-specific code, internal peripherals such as CRC, DMA or crypto | On-target unit tests | `unity` or `gtest` app in integration test mode. See [Unit Tests](Usecase/UnitTest.md). |
| HAL and drivers | HW/SW integration tests | `generic` app in integration test mode. `DUT Invoke` and endpoints. |
| Complete firmware | System tests | System test mode. Endpoints and optionally an environment model. |

# Example: integration test of a GPIO driver
Assume we want to test a GPIO driver. Specifically, we want to test that a call to the driver function `HAL_GPIO_WritePin` sets the physical pin PA1 to high.

First we make the function callable through the Code Interface by adding the glue function `gpio_set_high`. The test binary registers it with `ep_register`:

```cpp
#include <ep/core.h>

int32_t gpio_set_high(uint8_t const* payload, int len) {
    if (len == 2 && payload[0] == 'A' && payload[1] == '1') {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
        return 0;
    }
    return -1;
}

int ep_app_main() {
    ep_register("gpio_set_high", &gpio_set_high);
    return ep_process_loop();
}
```

The ep-config contains the `System` block for the Code Interface and a GPIO endpoint named `PA1` on the package pin of PA1:

```yaml
System:
  UART:
    DUT_TX: 101
    DUT_RX: 102
    Baudrate: 115200

GPIO:
  PA1:
    Pin: 35
    Pull: None
```

Conceptually, our test performs these steps:
1. Flash the test binary to the DUT.
1. Use the GPIO endpoint `PA1` to check that the pin is low.
1. Use the Code Interface to invoke `gpio_set_high` with the parameter `A1`.
1. Use the GPIO endpoint `PA1` to check that the pin is high.

A runnable test case looks like this:

<!-- tabs:start -->

#### **Robot Framework**

```robot
*** Settings ***
Library        EPLibrary    ${ep_url}    ${ep_binary_dir}/gpio_driver.ep-config.zip
Suite Setup    DUT Flash Firmware    ${ep_binary_dir}/test_gpio_driver.hex

*** Test Cases ***
Calling gpio_set_high sets output high
    ${levelBeforeInvoke} =    PA1 Read
    Should Be Equal    ${levelBeforeInvoke}    LOW    Default output state is not LOW.

    DUT Invoke    gpio_set_high    A1

    ${levelAfterInvoke} =    PA1 Read
    Should Be Equal    ${levelAfterInvoke}    HIGH    Output is not HIGH when called.
```

#### **pytest**

```python
import os

import pytest
from gpio_driver.test import EP    # imports gpio_driver.ep-config.zip

BINARY_DIR = os.environ["EP_BINARY_DIR"]

ep = EP.fixture()


@pytest.fixture(scope="module", autouse=True)
def suite_lifecycle(ep: EP):
    ep.DUT.flash_firmware(f"{BINARY_DIR}/test_gpio_driver.hex")
    yield


def test_calling_gpio_set_high_sets_output_high(ep: EP):
    assert ep.PA1.read() == "LOW", "Default output state is not LOW."

    ep.DUT.invoke("gpio_set_high", "A1")

    assert ep.PA1.read() == "HIGH", "Output is not HIGH when called."
```

<!-- tabs:end -->

# Example: system test of unchanged firmware
Now assume the production firmware must switch on an LED within 10 ms after a button is pressed. The button pulls its pin low. The firmware is flashed as it is, without the EP runtime.

The ep-config has no `System` block and defines one GPIO endpoint for the button and one for the LED:

```yaml
GPIO:
  BUTTON:
    Pin: 34
    Pull: None
  LED:
    Pin: 47
    Pull: None
```

The test drives the button, and the GPIO endpoints measure the reaction time on the EP:

<!-- tabs:start -->

#### **Robot Framework**

```robot
*** Settings ***
Library        EPLibrary    ${ep_url}    ${ep_binary_dir}/button.ep-config.zip
Suite Setup    DUT Flash Firmware    ${ep_binary_dir}/firmware.hex

*** Test Cases ***
LED turns on within 10ms after button press
    BUTTON Set High
    ${id} =    GPIO Measure Between Falling And Rising Edge    BUTTON    LED
    BUTTON Set Low
    ${reaction_ms} =    GPIO Get Measurement Result    ${id}    ReturnType=ms
    Should Be True    ${reaction_ms} < 10
```

#### **pytest**

```python
import os

import pytest
from button.test import EP    # imports button.ep-config.zip

BINARY_DIR = os.environ["EP_BINARY_DIR"]

ep = EP.fixture()


@pytest.fixture(scope="module", autouse=True)
def suite_lifecycle(ep: EP):
    ep.DUT.flash_firmware(f"{BINARY_DIR}/firmware.hex")
    yield


def test_led_turns_on_within_10ms_after_button_press(ep: EP):
    ep.BUTTON.set_high()
    measurement = ep.GPIO.measure_between_falling_and_rising_edge("BUTTON", "LED")
    ep.BUTTON.set_low()
    reaction_ms = ep.GPIO.get_measurement_result(measurement, ReturnType="ms")
    assert reaction_ms < 10
```

<!-- tabs:end -->

When the firmware needs a reacting environment, for example a sensor value that depends on its outputs, an [environment model](#environment-models) provides it. The [FMI endpoint](Endpoints/FMI.md#example-a-thermostat-with-a-simulated-heater) shows a complete system test of a thermostat with a simulated heater.

# Where to go next
- [Getting Started](GettingStarted.md) installs the required software, connects your EP and runs the first example.
- [Software Package](SoftwarePackage.md) explains how to integrate the EP into your project, your build system and your CI.
- [Endpoints](Endpoints.md) describes the principles that apply to all endpoints. The endpoint pages list the configuration and all keywords of each endpoint type.
