# ExecutionPlatform: instructions for AI assistants

This folder is the Software Package of the embeff ExecutionPlatform (EP), a test system for microcontrollers. The EP holds a DUT board with the microcontroller the customer chose. Its usable pins are wired to an FPGA on the EP, and an ep-config assigns *endpoints* (GPIO, CAN, UART, ...) to them. Tests written in Robot Framework or pytest run on a PC. They flash the DUT, call functions inside the DUT firmware (integration test mode only), and drive or observe its pins through endpoints. Overview: [docs/README.md](docs/README.md), terms: [docs/Glossary.md](docs/Glossary.md).

Read this file first. Then read the page of the project's test frontend ([docs/TestFrontend/RobotFramework.md](docs/TestFrontend/RobotFramework.md) or [docs/TestFrontend/pytest.md](docs/TestFrontend/pytest.md)), [docs/TestCapabilities.md](docs/TestCapabilities.md) (test functions per endpoint type) and [docs/ModelCapabilities.md](docs/ModelCapabilities.md) (functions for Python models), and finally the endpoint pages they link to for the endpoint types you use.

## Test frontend

Tests are written in Robot Framework or in pytest. Both are fully supported and reach the same functions. Firmware, ep-configs and models are the same for both; only the test files differ.

Test frontend of this project: not chosen yet

- While the line above says "not chosen yet", ask the user which test frontend the project uses before you write, change or run a test. If tests outside `example/` already use one, say so in the question: `*.robot` files that import `EPLibrary`, or `test_*.py` files that import `<name>.test`. The shipped `example/` has both and does not count.
- Then change that line to `Test frontend of this project: Robot Framework` or `Test frontend of this project: pytest`, so the question is asked only once.
- Write tests only in the chosen frontend. Where the two differ, this file says "Robot Framework:" and "pytest:"; follow the chosen one.
- This file names test functions as Robot Framework keywords, e.g. `DUT Invoke` or `<CANx> Send Frame`. [docs/TestCapabilities.md](docs/TestCapabilities.md) lists the pytest call next to each keyword, e.g. `ep.DUT.invoke(...)` or `ep.<CANx>.send_frame(...)`.

## Sources of truth

- **Generated from the code, always accurate:**
  - the function sections of `docs/Endpoints/*.md`,
  - `docs/TestCapabilities.md` and `docs/ModelCapabilities.md`.
  - If a prose snippet in the docs disagrees with these, trust these. Never use a function, keyword or argument that does not appear in them, nor an ep-config key that is not in the schema below.
- **[.vscode/ep-config.schema.json](.vscode/ep-config.schema.json)** is the schema the customer portal validates ep-config YAML against. It does not check pin numbers.
- **The examples in [example/](example/)** build and run where the BSP supports their app (see the test mode table). Each comes as a Robot Framework suite (`*.robot`) and as a pytest module (`test_*.py`) that test the same.

| Task | Read |
|---|---|
| Package layout, CMake or other build systems, own BSP, running tests from a terminal or CI | [docs/SoftwarePackage.md](docs/SoftwarePackage.md) |
| First steps, host requirements, firmware skeleton | [docs/GettingStarted.md](docs/GettingStarted.md) |
| Calling test functions, suite and test lifecycle, byte data, comparing floats | [docs/TestFrontend/RobotFramework.md](docs/TestFrontend/RobotFramework.md), [docs/TestFrontend/pytest.md](docs/TestFrontend/pytest.md) |
| Flash, power and call the DUT, DUT standard output | [docs/CodeInterface.md](docs/CodeInterface.md) |
| ep-config YAML, integration vs system test mode, building a config | [docs/EP_Config.md](docs/EP_Config.md) |
| Units, byte data, test time, errors (all endpoints) | [docs/Endpoints.md](docs/Endpoints.md) |
| One endpoint type: config keys, behaviour, test functions | `docs/Endpoints/`, the page named in the heading of `docs/TestCapabilities.md` |
| Python environment model, FMU co-simulation | [docs/Endpoints/System.md](docs/Endpoints/System.md), [docs/Endpoints/FMI.md](docs/Endpoints/FMI.md) |
| Logic analyzer header | [docs/Endpoints/Analyzer.md](docs/Endpoints/Analyzer.md) |
| On-target unit tests, benchmarks, coverage | [docs/Usecase/](docs/Usecase/) |
| Version errors after an update | [docs/VersionCompatibility.md](docs/VersionCompatibility.md) |
| Debugging firmware while a test runs | [docs/DUT-Debugging.md](docs/DUT-Debugging.md) |
| Device web page, users, licences, network | [docs/Service.md](docs/Service.md) |

## Before you start: check, ask, hand off

Check yourself:
1. The Python package embeff-ep is installed in the environment the tests run in: `pip show embeff-ep` prints the version of this package. If not, run `pip install -r requirements.txt` (in the project's virtual environment, if it has one). It provides the Robot Framework library `EPLibrary`, the pytest plugin and the `ep-config` command. Suites written for 9.7 or older load `EP.py`; migrate them as [docs/Migration/9.8.md](docs/Migration/9.8.md) describes.
2. `bsp/default-c/` exists, because the CMake preset expects it. If it is missing, ask the user to download the BSP for their DUT from the customer portal ([docs/GettingStarted.md](docs/GettingStarted.md)).
3. `build/` holds the `.ep-config.zip` files the tests use. The examples use `build/gpio.ep-config.zip`, the sample configuration embeff provides for each DUT.
   - `ep-config info build/gpio.ep-config.zip` prints its DUT name (e.g. `DUT_RP2040_E0`), its EP version and the YAML it was built from.
4. Take firmware facts (MCU, compiler flags, the UART the EP runtime uses) from `bsp/default-c/`, not from guesses.

Ask the user for:
- **The EP address and credentials**, as `user:password@address`.
  - Robot Framework: set it as `ep_url` in [robot.toml](robot.toml) for local runs only. The shipped value `user:password@10.0.0.120` is a placeholder.
  - pytest: set it as `EP_URL` in `.env`, a copy of [.env.example](.env.example). The shipped value `user:password@my-ep` is a placeholder.
  - Never commit credentials, neither in `robot.toml` nor in `.env`.
- **An API secret for building ep-configs**, unless the environment variable `EP_CONFIG_SECRET` is already set.
  - The user generates it under `API access` in the settings menu of the customer portal and sets it as `EP_CONFIG_SECRET` ([docs/EP_Config.md](docs/EP_Config.md#command-line-usage-ep-config)).
  - Prefer the variable over `--secret`. Never write the secret into a file of the package.
- **The DUT pin mapping file `<DUT>-mapping.yaml`**, unless you already have it. You need it to find pin numbers; see [Pin numbers](#pin-numbers).
- **Which MCU pins and signals the code under test uses**, if the code does not show it.

Hand off to the user everything that needs the customer portal website, the device web page, licences or physical access.

Build and run, from the package root:
- **Configure once** with `cmake --preset default_c`. Then build with `cmake --build build --target <target>`. From a test, Robot Framework: the `CMake build` keyword from [app/cmake.resource](app/cmake.resource); pytest: `cmake_build(build_dir, build_target)` from [app/cmake_helper.py](app/cmake_helper.py). Both build but never configure.
- **Build ep-configs** with `ep-config`; see [Build an ep-config](#build-an-ep-config).
- **Run, Robot Framework:** `robotcode robot <suite or folder>`. Plain `robot` does not read robot.toml, so it needs `-v ep_url:... -v ep_app_dir:... -v ep_binary_dir:...` ([docs/SoftwarePackage.md](docs/SoftwarePackage.md)).
- **Run, pytest** ([docs/SoftwarePackage.md](docs/SoftwarePackage.md#use-software-package-to-write-and-run-tests)):
  1. List each `build/<name>.ep-config.zip` in [pyproject.toml](pyproject.toml), next to `app`: under `pythonpath` in `[tool.pytest]`, and under `extraPaths` in `[tool.pyright]` for code completion.
  2. Set `EP_URL` and `EP_BINARY_DIR=build` (only the examples read it). `.env` holds them, but only the VS Code Python extension reads it. From a shell, set them yourself as [docs/SoftwarePackage.md](docs/SoftwarePackage.md) shows.
  3. Run `pytest <file or folder>` from the package root. The plugin registers itself when embeff-ep is installed. [pyproject.toml](pyproject.toml) passes `-s`, so `print()` output shows. `--junitxml=<file>` writes a report for CI.

## Choose the test mode

The ep-config decides the mode ([docs/EP_Config.md](docs/EP_Config.md#configurations)):
- **Integration test mode:** the ep-config has a `System:` section. The DUT runs a test binary that contains the EP runtime from `src/`, and the test can call DUT functions with `DUT Invoke`.
- **System test mode:** there is no `System:` section. The DUT runs firmware without the EP runtime, typically unchanged production firmware. The test flashes and powers the DUT and works through endpoints; `DUT Invoke`, `DUT Get Output` and `DUT Get Coverage` are not available.

| Goal | Firmware | ep-config | Test | Read |
|---|---|---|---|---|
| On-target unit tests | Link `ep_app_gtest` and write only `TEST()`s, or link `ep_app_unity` and write `int run_tests(void)` returning `UNITY_END()`. Define neither `main` nor `ep_app_main`. | `System:` section, no endpoints needed (the sample gpio config works) | Robot Framework: `Resource  ${ep_app_dir}/gtest/gtest.resource` and `Run Gtest Tests  expectFailures=False`; for Unity `unity.resource` and `Run Unity Tests  expectedFailures=0`. pytest: `from gtest.gtest import run_gtest_tests` and `run_gtest_tests(ep, expect_failures=False)`; for Unity `from unity.unity import run_unity_tests` and `run_unity_tests(ep, expected_failures=0)` | [UnitTest.md](docs/Usecase/UnitTest.md), [example/misc/](example/misc/) |
| Runtime benchmark (C++) | Link `ep_app_bench`. Declare the function under test with `ep_bench_register(<prototype>)`. Write `int ep_app_main()` that registers a glue function with `ep_register` and ends with `return ep_process_loop();`; the glue parses the payload and calls `ep_bench_invoke(fn, args...)` | `System:` section plus a GPIO endpoint on the BSP reference pin (the sample gpio config has it) | Robot Framework: `Resource  ${ep_app_dir}/bench/bench.resource` and `Do Benchmark`. pytest: `from bench.bench import do_benchmark` and `runtime_ns, retval = do_benchmark(ep, "<function>", "<args>")` | [Benchmark.md](docs/Usecase/Benchmark.md) |
| HW/SW integration test of HAL or driver code | Link `ep_app_generic`. `int ep_app_main()` registers glue functions with `ep_register("<method_name>", &fn)` and ends with `return ep_process_loop();` | `System:` section plus endpoints on the pins under test | `DUT Invoke` plus endpoint functions | [CodeInterface.md](docs/CodeInterface.md), [example/hello_world/](example/hello_world/) |
| System test of unchanged firmware | Production firmware, no EP runtime | No `System:` section. Endpoints on every pin the test drives or observes | Endpoint functions. Optionally a Python environment model (`System Start Model`, needs the System licence) or an FMU (`FMI Load Model`, needs an `FMI:` section and the System licence) | [System.md](docs/Endpoints/System.md), [FMI.md](docs/Endpoints/FMI.md) |

The BSP decides which apps exist: `EP_BSP_APPS_TESTED` in `bsp/default-c/CMakeLists.txt`, printed as `Supported apps:` when CMake configures. Some BSPs have no `gtest` or `bench`.

How to choose:
- **Pick the lowest level that can show the behaviour.**
  - Pure logic: host unit tests outside the EP, run most often.
  - Compiler- or MCU-specific code and internal peripherals (CRC, DMA, crypto): on-target unit tests.
- **HAL and drivers: HW/SW integration tests.** Treat the endpoint as a hardware test double:
  1. Arrange it (e.g. `SPI0 Set Response Bytes`).
  2. Call the function with `DUT Invoke`.
  3. Assert both its return value and what happened on the pins or bus.
- **Unchanged firmware, closed-loop behaviour, or legacy code without tests: system tests.** A model simulates the environment and exposes its state through `outputs`. The test sequence alone decides pass or fail.

"System" means three different things:
- the ep-config `System:` section, which is the UART link to the EP runtime in the DUT;
- the System endpoint, which runs Python models, depends only on the licence and works in both modes;
- system test mode, where that section is absent.

## How names connect

- **Instance names:** each endpoint section (all but `System:` and `Analyzer:`) holds instances with free names matching `^[_a-zA-Z][_a-zA-Z0-9]*$`, unique in the whole ep-config. Do not use `DUT`, `SYSTEM`, `GPIO`, `FMI`, `ENERGY` or `Analyzer`. Neither rule is checked; a clash is not reported.
  - `FMI:` instances name an FMU, which is linked to the endpoints of the same names as its variables. `ANALOG_MULTIPLEXER_8:` instances have no functions. Neither becomes a keyword prefix or handle attribute.
  - The name is used verbatim as keyword prefix, as attribute of the pytest handle and as model attribute: `CAN: {CAN1: {...}}` gives `CAN1 Send Frame` in Robot Framework, `ep.CAN1.send_frame(...)` in pytest and `self.CAN1.send_frame(...)` in a model. The function names differ between tests and models: take them from the matching capabilities file.
  - The docs write these names as placeholders like `<CANx>` or `<GPIOPin>`.
  - An instance name does not choose a pin; only the pin values do (`GPIO4: {Pin: 6, Pull: None}` is valid if package pin 6 is usable on that DUT).
- **Test functions come from the `.ep-config.zip`** the suite loads. Different suites may use different configs.
  - Robot Framework: `Library  EPLibrary  ${ep_url}  ${ep_binary_dir}/<name>.ep-config.zip`. Import `EPLibrary` once per suite.
  - pytest: `from <name>.test import EP` imports `<name>.ep-config.zip` and the class of the configuration, `ep = EP.fixture()` creates a fixture with `scope="module"`, and the tests take that fixture as argument annotated with the same class (`def test_x(ep: EP)`), which code completion needs. With two configurations in one module, name each class (`from gpio.test import EP as Gpio`). The handle has one attribute per instance (`ep.CAN1`) and per static endpoint (`ep.DUT`, `ep.GPIO`, `ep.SYSTEM`, `ep.FMI`). A pytest module is a suite. See [example/hello_world/test_hello_world.py](example/hello_world/test_hello_world.py).
- **The `.ep-config.zip` file name is the Python module name** (`import <name> as config` in models, `from <name>.test import EP` in pytest). Never rename an `.ep-config.zip`; moving it is fine.

## Pin numbers

- **Pin values are package pin numbers.** Every pin value in an ep-config (`Pin: 69`, CAN `TX: 20`, UART `DUT_TX: 36`) is the DUT's package pin number. BGA balls are strings like `"B17"`. A value is never a port name like `PA1`.
  - Exceptions, which reference other instances: the select pins of `ANALOG_MULTIPLEXER_8` (`Sel0: PA0.Pin`), an Analog-Out `Pin` on a multiplexer input (`<MUX>.In3`), and the `Analyzer:` outputs, which name signals. Their endpoint pages give the syntax.
- **Finding a digital or analog pin number:** look it up in `<DUT>-mapping.yaml`. It lists every package pin that is connected to the EP and therefore available for testing. Pins are the keys under `GPIO` (all digital endpoints) and, if present, `AnalogOut` and `AnalogIn`. It names package pins only, not port names like `PA1`.
  - The customer portal offers it next to the BSP (DUTs → click the DUT → "Mapping definition").
  - If you do not have it, ask the user for it.
- **For digital pins, the sample gpio config also helps:** it has one GPIO instance per digital pin, named after the MCU pin: `ep-config info build/gpio.ep-config.zip`. The EP UART pins are in its `System:` section; on some DUTs debug-port pins are left out.
- **With the sample BSP, copy the `System:` section from the sample gpio config.** Its UART pins and baudrate match that BSP and differ per DUT. With your own BSP, use the UART settings it configures. The example in [docs/EP_Config.md](docs/EP_Config.md) is illustrative only.
- **Each pin serves one signal**, including the `System:` UART pins. Invalid pins are only reported when the build fails.

## Build an ep-config

`ep-config` (from embeff-ep) builds on the embeff build farm through the customer portal ([docs/EP_Config.md](docs/EP_Config.md#command-line-usage-ep-config)). It needs `EP_CONFIG_SECRET`.

1. **Write the YAML as `<name>.ep-config.yaml`**, so VS Code validates it against the schema. `<name>` becomes the name of the `.ep-config.zip` and of its Python module: letters, digits and `_`, starting with a letter, not a Python keyword, at most 40 characters.
2. **Build it into `build/`:** `ep-config build --dut <DUT> --version <version> --output-folder build <name>.ep-config.yaml`
   - `<DUT>` is the DUT name that `info build/gpio.ep-config.zip` prints. `<version>` is the full version of this package: the tag after `@v` in the embeff-ep line of [requirements.txt](requirements.txt) (e.g. `9.8.0`).
   - The YAML is checked against the schema first, so schema errors fail within seconds. `validate` with the same `--dut` and `--version` runs only that check; it checks neither the DUT name nor the file name.
   - The result is `build/<name>.ep-config.zip`; a file of that name is overwritten. `build/` must exist, otherwise the command fails at once with `output folder does not exist`.
3. **The command blocks until the farm is done**, typically 10 to 30 minutes plus queue time, and gives up after 35 minutes (`--timeout`, in seconds). Start it in the background if your shell times out sooner.
   - At most two command-line builds per customer may be queued or running; the portal refuses a third. Builds started on the portal website do not count. A build whose command timed out or was killed keeps running and still counts, and it cannot be resumed.
   - Invalid pins are reported only now, as `build failed: <reason>`.
- Builds from the command line are not saved in the `Configurations` tab of the portal. Keep the YAML with the tests.

## Traps

Behaviour:
1. **Measure timing on the EP:** as the difference of test times returned by test functions, or with measurement functions such as `GPIO Measure Between ...` ([test time](docs/Endpoints.md#test-time)).
   - PC-side timing (`Sleep` in Robot Framework, `time.sleep()` in pytest, call durations) has no guarantees. Sleep only to let time pass.
   - Test time restarts with every test and wraps after about 42.9 s.
2. **Compare measured values with a tolerance.** Measurement functions return the base unit (`s`, `Hz`, `V`) by default; their `ReturnType` argument accepts any unit of that quantity, e.g. `ns`, `kHz` or `mV`.
   - Robot Framework: `Should Be Equal As Numbers` rounds to 6 decimals by default, so nanosecond differences expressed in seconds vanish.
   - pytest: physical values are floats; compare with `pytest.approx(expected, abs=...)`.
3. **Strict mode** is checked automatically at the end of every test on the endpoints that have it (CAN, CAN-FD, UART, I2C-Target, SPI-Slave, Eth-PHY, Pin-Follow, System). A violation fails the test in teardown; pytest reports it as an error at teardown.
   - Never disable the check to make a test pass.
   - Use `<X> Disable Automatic Strict Mode Check` only when the test deliberately provokes the condition, e.g. bus errors.
4. **Never drive endpoints while the DUT is unpowered** ([docs/CodeInterface.md](docs/CodeInterface.md)).
   - The EP powers the DUT off at suite start and suite end. `DUT Flash Firmware` in Suite Setup leaves it powered. In pytest the suite starts when the module-scoped fixture opens the EP.
   - Flash before you start a model or FMU that drives pins.
5. **Endpoints are reset after every test:** GPIOs go to tristate, buses stop, buffers are cleared, models stop. Configure endpoints (`<CANx> Start`, pin levels, responses) and start models in each test or in Test Setup, not in Suite Setup. In pytest that is the test itself or a fixture with function scope, not the module-scoped one.
6. **The DUT is not reset between tests**; its RAM and peripherals carry over.
   - For a cold start per test, use `DUT Power Off` and `DUT Power On` in Test Setup (pytest: in an autouse fixture with function scope).
   - After `DutAborted` or an invoke timeout, every later `DUT Invoke` fails with `DutNotReady` until the DUT is power-cycled.
7. **`DUT Invoke` blocks until the DUT function returns.** Arm any stimulus that must happen during the call beforehand: `... Periodically`, pulse and waveform keywords with `WaitForCompletion=False` (they block by default), `Respond To Read`, `Set Response Bytes`.
8. **Flash once, in Suite Setup**, because flashing is slow. In pytest flash in the module-scoped fixture or in an autouse fixture with `scope="module"`. Flashing again needs `DUT Power Off` first. An unchanged binary is not reprogrammed.

Mechanics:
9. **Firmware glue connects `DUT Invoke` to the code under test** ([DUT Invoke](docs/CodeInterface.md#functions), [example/hello_world/hello_world.cpp](example/hello_world/hello_world.cpp)):
    - `DUT Invoke  <method_name>  <data>` calls the glue function registered with `ep_register("<method_name>", &fn)`. The method names must be identical. Register every glue function in `ep_app_main()`, before `return ep_process_loop();`.
    - `#include <ep/core.h>`; the signature is `int32_t fn(uint8_t const* payload, int len)`. The glue turns the payload into the arguments of the function under test and returns its result.
    - `payload` holds the bytes of `data`: `DUT Invoke  square  3` (pytest: `ep.DUT.invoke("square", "3")`) sends the character `'3'` (0x33), which the glue parses; the examples use `atoi`. For raw bytes pass `\x03` or a list in Robot Framework, `b"\x03"` or `[3]` in pytest ([byte data](docs/Endpoints.md#byte-data)).
    - The payload is not guaranteed to be NUL-terminated and is only valid during the call, so copy it into your own buffer.
    - The `int32_t` return value is the call's result. Robot Framework: compare with `${9}`, not `9`. pytest: it is an `int`, so `== 9`.
    - Limits: by default at most 10 registered method names and 200 bytes of method names in total (both configurable), and method name plus payload ≤ 254 bytes ([docs/CodeInterface.md](docs/CodeInterface.md#limitations-of-ep_register)).
10. **Physical values are strings with a unit:** `52ms`, `500 kbit/s`, `1.5V`, in pytest (`Pause="52ms"`) and in models too. Units are case-sensitive and bare numbers are rejected ([physical units](docs/Endpoints.md#physical-units)).
11. **Named arguments are case-sensitive** and differ per endpoint (`timeout` on GPIO, `Timeout` elsewhere). So are function names in pytest (`ep.PA0.set_high()`, `ep.CAN1.send_frame(...)`); Robot Framework ignores case in keyword names only. Take both from [docs/TestCapabilities.md](docs/TestCapabilities.md).
12. **Wait before reading.** `Get Received` and `Get Frames` return only what has arrived.
    - Wait first with `Wait For Received Buffer`, `Wait For Frame Buffer` or `Wait For Data Written`. Their default is `Timeout=100ms`; on UART and CAN, `Timeout=0s` waits forever.
    - The CAN frame buffer also holds the frames the EP sent itself, unless they were sent with `StoreInBuffer=False`.
13. **Time limits in firmware:**
    - An invoked function must return within 1.5 s. Standard output that reaches the EP restarts this limit, so call `ep_bsp_flush()` after progress.
    - After power-on the BSP must report ready within 2 s, and `ep_process_loop()` must follow within another 2 s. Move slow initialisation into an invoked function.
14. **Print only inside invoked functions**, and read the output with `DUT Get Output`. Output at other times can abort the DUT session. Print ASCII only: output containing any other character is dropped as a whole.
15. **Versions must match:** the EP, the `.ep-config.zip` files and this package must share their major.minor version. That includes the installed embeff-ep and the `src/` runtime compiled into every DUT binary. After a major or minor EP update, take `src/` and `app/` from the matching package and rebuild the firmware ([docs/VersionCompatibility.md](docs/VersionCompatibility.md)).
16. **An EP serves one client per test suite**; other runs wait until the suite ends, and fail if that takes longer than their connection timeout.
17. pytest: open the EP with `ep = EP.fixture()`. Its scope is the module, so the plugin ties suite start and end to the module. Prepare the DUT and the EP, e.g. flash, in an autouse fixture with `scope="module"` that takes this fixture. The plugin resets the endpoints and checks strict mode only around tests that use this fixture, directly or through another fixture.
18. pytest: a module may load two configurations through two fixtures, but each needs its own EP. Two fixtures that reach the same EP fail. Annotate each argument with the class of the configuration its fixture opens; a mismatch fails the test.

## Python models

Rules for a model loaded with `System Start Model` ([docs/Endpoints/System.md](docs/Endpoints/System.md)):
- **File layout:** start with `import <config> as config` and define `class SystemModel(config.Configuration)`. Do not name the file `<config>.py`; use e.g. `<config>_model.py`.
- **`__init__` holds plain state only.** Create `self.outputs` with every key the test reads. Endpoints are not available yet: use them and register callbacks in `start()`.
- **Reaching endpoints:** use `self.<Instance>`, and `self.SYSTEM` for the System endpoint. Only the types in [docs/ModelCapabilities.md](docs/ModelCapabilities.md) are reachable.
- **Callbacks** are plain synchronous functions: no `async def` (the body of an async callback never runs, without an error), no `sleep`. GPIO, CAN, CAN-FD, I2C-Target, Memory-mapped I2C and tick callbacks must finish within 10 ms of their event (strict mode); UART callbacks are not timed. An exception in a callback fails the test in teardown.
- **A model runs for one test only.** Start it in each test or in Test Setup.
  - Models cannot start endpoints. The test must start and configure CAN, CAN-FD, UART and I2C-Target first.
- **Talking to the test:** `System Get Output  <key>` reads `self.outputs[<key>]`. `System Set Input` calls `input_changed(self, variable, value)`.
- **`print()` output of a model does not appear in the test log or report.** Expose state through `outputs` instead.
- **UART `on_bytes_received`** calls back with a `bytes` object of one or more bytes.

## Error messages

| Message contains | Cause | Fix |
|---|---|---|
| `DUT did not reach 'ep_process_loop'` | The ep-config has a `System:` section but the firmware has no EP runtime or never calls `ep_process_loop()`; or the `System:` UART settings do not match the BSP; or initialisation is too slow | Remove `System:` for production firmware; copy `System:` from the sample gpio config; move slow initialisation |
| `DUT not ready` with `Missing System block` | `DUT Invoke` in system test mode | Add the `System:` section and a test binary, or drop the call |
| `Current DUT state is States.TIMEOUT` or `States.ABORTED` | An earlier invoke timed out or the DUT crashed | `DUT Power Off`, then `DUT Power On` |
| `The invoked method did not respond.` | The invoked function sent neither output nor its result for 1.5 s: it is slow, hangs or crashed | Return sooner, or print and flush progress output |
| `DutAborted: <reason>` | `ep_abort` (also from `ep_register` errors, a returning `ep_app_main`, and hard faults on most sample BSPs), or `<...>` reasons from the EP such as output outside an invoked function | Read the reason |
| `is not a known function` | The `DUT Invoke` method name differs from the one passed to `ep_register` | Use the identical string |
| `Unknown Keyword` | A `SYSTEM ...` function without the System licence, or a `GPIO ...`/`FMI ...` function without that ep-config section, in both frontends | Check the licence and the ep-config |
| `The EP handle has no '<name>'` | pytest: the ep-config has no instance or static endpoint of that name | Check the instance names with `ep-config info` |
| `Did you rename the file?` | An `.ep-config.zip` was renamed | Restore its original name |
| `BSP version in the DUT binary is incompatible` | The firmware's runtime major.minor version differs from the EP | Recompile with this package |
| `Test teardown failed.` (pytest: `ERROR at teardown`) with `Strict mode` | A strict-mode violation during the test | Read the endpoint's strict-mode conditions |
| `is not available to a model` | The model used a `self.<name>` that is neither one of its endpoints nor defined by the model: an endpoint type it cannot reach, a misspelled instance, or a missing method such as `input_changed` when the test calls `System Set Input` | Use a name from the list in the message, or define the attribute; see [Python models](#python-models) |
| `Cannot load model:` | The model file failed to load (a model rule or a Python error) | Read the text after the colon |
| `Execution stopped by user.` right after an error (pytest: the fixture fails with `<fixture>: <reason>`) | EPLibrary or the pytest plugin could not start: connection, credentials, or a config built for another DUT or version | Read the error above it, or the reason |
| `Your EP client version <x> is incompatible with EP version <y>` (Robot Framework: then `Test execution stopped due to a fatal error.`) | The installed embeff-ep and the EP differ in major.minor | Install the embeff-ep that matches the EP ([docs/VersionCompatibility.md](docs/VersionCompatibility.md)) |
| `No module named '<name>'` | pytest: `<name>.ep-config.zip` is missing or not under `pythonpath` in pyproject.toml | Build or download it into build/ and add it to `pythonpath` |
| `No module named 'EPLibrary'`, `No module named 'embeff_ep'`, `ep-config: command not found` | embeff-ep is not installed in the environment that runs the tests, or `robotcode` comes from another installation (in Robot Framework, the `PYTHONPATH` listed under the import error is outside the project's `.venv`) | `pip install -r requirements.txt` in that environment, which installs robotcode as well, and run from it |
| `No EP url specified.` | pytest: neither the `url` of `EP.fixture()` nor `EP_URL` gives the EP | Set `EP_URL` |
| `KeyError: 'EP_BINARY_DIR'` | pytest: the examples read `EP_BINARY_DIR` | Set it to `build` |
| `is annotated with EP_<a>, but its fixture provides EP_<b>` | pytest: an argument is annotated with the class of another configuration than its fixture opens | Annotate it with the class the fixture uses |
| `both use the same EP` | pytest: two fixtures of one module reach the same EP | Use one fixture per EP, or split the module |
| `Invalid name. Allowed: 1-40 characters` | `<name>` in the YAML file name has a dot or another character that is not a letter, digit or `_`, does not start with a letter, is longer than 40 characters, or is a Python keyword | Rename the YAML file so that `<name>` follows step 1 of [Build an ep-config](#build-an-ep-config) |
| `Unknown ep_version` | `--version` is not a released version, e.g. only major.minor | Pass the embeff-ep version from [requirements.txt](requirements.txt) |
| `builds queued or running` | Two builds are already on the farm, including ones whose command timed out | Wait until one finishes |
