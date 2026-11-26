# pytest
pytest tests use the plugin of the Python package [embeff-ep](https://github.com/embeff/embeff-ep), which registers itself when the package is installed. A fixture connects to one EP, loads the ep-config onto the EP and hands the EP handle to the tests. The EP handle has one attribute for each endpoint, for example `ep.DUT` for the [Code Interface](../CodeInterface.md), and the tests call for example `ep.PA1.read()`. `EP.fixture()` of an imported configuration creates this fixture:

```python
from gpio.test import EP    # imports gpio.ep-config.zip

ep = EP.fixture()    # address and credentials from $EP_URL


def test_pa1_is_low(ep: EP):
    assert ep.PA1.read() == "LOW"
```

`EP` is the class of the gpio configuration. Import it once and use it for both the fixture and the argument of each test: the annotation gives your editor the endpoints of this configuration for code completion. Pylance infers it from the fixture, pyright and other checkers need the annotation. If an argument is annotated with the class of another configuration than its fixture opens, the test fails with a message instead of running with endpoints that code completion does not show. A module that loads two configurations gives each class its own name, for example `from gpio.test import EP as Gpio` and `from button.test import EP as Button`.

Flash the DUT, or prepare the EP otherwise, in a suite lifecycle fixture that takes the EP fixture:

```python
ep = EP.fixture()


@pytest.fixture(scope="module", autouse=True)
def suite_lifecycle(ep: EP):
    ep.DUT.flash_firmware("build/firmware.hex")
    yield
```

See [Getting Started](../GettingStarted.md) for how to set up your EP and your environment, and [Software Package](../SoftwarePackage.md#use-software-package-to-write-and-run-tests) for how to run the tests in your own project.

## Before you start
If you have not run pytest tests on the EP yet, set up your environment as described in [Getting Started](../GettingStarted.md):
- [Requirements](../GettingStarted.md#requirements): run `pip install -r requirements.txt` in the root folder of the [Software Package](../SoftwarePackage.md). Its `requirements.txt` installs embeff-ep in the version of your EP.
- [VS Code configuration](../GettingStarted.md#vs-code-configuration): list each `.ep-config.zip` in `pyproject.toml` (see [Importing ep-configs in Python](../SoftwarePackage.md#importing-ep-configs-in-python)), then set `EP_URL` and `EP_BINARY_DIR` in `.env`.
- [Run example in VS Code](../GettingStarted.md#run-example-in-vs-code): run the hello world example to check the setup.

## Suites and their Lifecycle
A suite is a logical group of tests that share one ep-config. pytest calls these suites 'modules', the two terms are equivalent. Each test file is collected by pytest as one suite.

Set up a suite lifecycle by defining a fixture with `scope="module", autouse=True` and `yield`:

```python
@pytest.fixture(scope="module", autouse=True)
def suite_lifecycle(ep: EP):
    print("This runs before the whole suite.")
    yield
    print("This runs after the whole suite.")
```

Something similar can be done on a per-test basis by defining another fixture with `scope="function", autouse=True` and `yield`:

```python
@pytest.fixture(scope="function", autouse=True)
def test_lifecycle(ep: EP):
    print("This runs before each and every test in the suite.")
    yield
    print("This runs after each and every test in the suite.")
```

Note that the EP handle can be requested by listing the `ep` fixture in the arguments.

Find more information about fixtures in the [pytest docs](https://docs.pytest.org/en/stable/how-to/fixtures.html#scope-sharing-fixtures-across-classes-modules-packages-or-session).



## Calling endpoint functions
The endpoint pages show each function by its name, for example `set_high()` on the [GPIO](../Endpoints/GPIO.md) page. In pytest, you call it on the attribute of the handle that is named like the endpoint instance:

| Endpoint page | ep-config | Call |
| - | - | - |
| `set_high()` on the GPIO page | GPIO endpoint `PA0` | `ep.PA0.set_high()` |
| `send_frame(Id, Data: ByteData, ...)` on the CAN page | CAN endpoint `CAN1` | `ep.CAN1.send_frame(...)` |
| `set_pins(high=[], ...)` on the GPIO page | none, `GPIO` is a [static endpoint](../Endpoints.md#static-endpoints) | `ep.GPIO.set_pins(...)` |

Function names are case-sensitive. Use them exactly as the endpoint pages show them, or let your editor's code completion list them.

The functions of the other static endpoints are called the same way: `ep.DUT.flash_firmware(file)`, `ep.SYSTEM.start_model(file)`, `ep.FMI.load_model(fmu_file)`.

Arguments are Python values. [Physical values](../Endpoints.md#physical-units) are strings with their unit, as the endpoint pages show them: `Pause="0ms"`.

## Example: GPIO
The ep-config defines two GPIO endpoints: `BUTTON` on a pin the firmware reads as an input, and `LED` on a pin the firmware drives. The test only writes `BUTTON` and only reads `LED`, as the DUT would see a real button and a real LED. It checks that the firmware switches on the LED within 10 ms after the button is pressed:

```python
from button.test import EP    # imports button.ep-config.zip

ep = EP.fixture()


def test_led_turns_on_within_10ms_after_button_press(ep: EP):
    ep.BUTTON.set_high()
    measurement = ep.GPIO.measure_between_falling_and_rising_edge("BUTTON", "LED")
    ep.BUTTON.set_low()    # the button pulls its pin low when pressed

    reaction_ms = ep.GPIO.get_measurement_result(measurement, ReturnType="ms")
    assert reaction_ms < 10
    assert ep.LED.read() == "HIGH"
```

`ep.BUTTON` and `ep.LED` are the instances from the ep-config. The measurement runs on the static `GPIO` endpoint, which takes the pin names as arguments.

## Unit tests and benchmarks
The `unity`, `gtest` and `bench` apps come with functions that run unit tests on the DUT and measure the runtime of functions. Import them from the app folder and pass the EP handle as first argument:

```python
from unity.unity import run_unity_tests
from gtest.gtest import run_gtest_tests
from bench.bench import do_benchmark

output = run_unity_tests(ep)
runtime_ns, retval = do_benchmark(ep, "invoke_bench_cos", "30")
```

See [Unit Tests](../Usecase/UnitTest.md) and [Benchmarks](../Usecase/Benchmark.md).

## Byte data
Functions with a [_ByteData_](../Endpoints.md#byte-data) argument accept `str`, `bytes` and lists of byte values. All of them reach the DUT as the same bytes:

```python
ep.DUT.invoke("print_payload", "Hey")
ep.DUT.invoke("get_length", b"\xFF\x48\x65\x85\x79")
ep.DUT.invoke("get_length", [0xFF, 0x48, 0x65, 0x85, 0x79])
```

Functions that return byte data return it as `bytes`.

## Compare floating-point results
Endpoint functions return [physical values](../Endpoints.md#return-values) as floats. Compare them with a tolerance, for example with `pytest.approx`:

```python
reaction_s = ep.GPIO.get_measurement_result(measurement)
assert reaction_s == pytest.approx(970e-9, abs=20e-9)
```
