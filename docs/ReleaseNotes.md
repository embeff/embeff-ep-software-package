# Release Notes

# 9.8.0

- Change: :warning: Your tests now reach the ExecutionPlatform through the Python package [embeff-ep](https://github.com/embeff/embeff-ep), installed with `pip install -r requirements.txt` (which also installs its dependencies `fastapi-websocket-rpc` 0.1.29 or newer and `websockets` 14 or newer). It contains the Robot Framework library `EPLibrary`, a pytest plugin that registers itself, and the `ep-config` command line tool. `app/EP.py`, `app/EP_pytest.py`, `app/EP_common.py` and `tools/ep-config.py` are removed. Existing test suites need a few small changes, see [Migrating to 9.8](Migration/9.8.md).
- Change: In pytest, the functions of the DUT, System and FMI Endpoint no longer repeat the endpoint name and are consistently accessed by their endpoint name.
For example change `ep.DUT.dut_invoke(...)` to `ep.DUT.invoke(...)`, `ep.flash_firmware(...)` to `ep.DUT.flash_firmware(...)` etc.
- Change: All Endpoint functions are now named in snake case and documented that way, e.g. `send_frame` instead of `Send_Frame` and `set_pins` instead of `Set_Pins`. Argument names are unchanged.
  - Robot Framework test suites are not affected. Robot Framework ignores case, spaces and underscores in keyword names, so `CAN1 Send Frame`, `CAN1 Send_Frame` and `CAN1 send_frame` all call the same keyword.
  - pytest is case-sensitive. Change e.g. `ep.CAN1.Send_Frame(...)` to `ep.CAN1.send_frame(...)` and `ep.GPIO.Set_Pins(...)` to `ep.GPIO.set_pins(...)`.
  - [System Endpoint](Endpoints/System.md) models are case-sensitive as well. Change `Set_Static` and `Reset` of the [Analog-Out Endpoint](Endpoints/Analog-Out.md) to `set_static` and `reset`.
- Change: The deprecated keywords `GPIO Get Measurement Result In Ns`, `<SPIx> Read Minimal Clock Period In Ns` and `<SPIx> Read Maximal Clock Period In Ns` are removed. Use `GPIO Get Measurement Result`, `<SPIx> Read Minimal Clock Period` and `<SPIx> Read Maximal Clock Period` with the argument `ReturnType=ns` instead.
- Change: :warning: ep-config names with `-` broke `import <name> as config` in models and `import <name>.test` in pytest. The customer portal now rejects `-` and accepts `_`. See [Creating a new configuration](EP_Config.md#creating-a-new-configuration) for the name rules.
- Change: :warning: ep-config files now have the extension `.ep-config.zip` instead of `.ep-config`. Code completion with Pylance only reads packages from `.zip` files. In the past, VS Code setups for pytest and system models therefore needed a copy of each ep-config with an added `.zip` extension. This copy step is not required anymore: The customer portal and the `ep-config` command line tool now produce a single file with the new extension that can be used by Robot Framework, pytest and code completion, without copying or renaming. For Robot Framework, change the file name in the `Library` line of your test suites. pytest setups already import the `.zip` name and keep working. Delete the old `.ep-config` files after the switch. See [Configurations](EP_Config.md) and [Migrating to 9.8](Migration/9.8.md).
- Feature: The software package is published on GitHub at [embeff/embeff-ep-software-package](https://github.com/embeff/embeff-ep-software-package), one release per ExecutionPlatform version, under the Apache License 2.0. See [Updating the software package](SoftwarePackage.md#updating-the-software-package).
- Feature: pytest is now fully supported next to Robot Framework. See [pytest](TestFrontend/pytest.md).
- Feature: The support for AI coding assistants now covers pytest as well. On first use, the assistant asks whether your tests use Robot Framework or pytest and records the answer in `AGENTS.md`.
- Feature: The `embeff-ep` Python package supports Python 3.14. See [Requirements](GettingStarted.md#requirements).
- Bugfix: The `on_byte_received` callback of the [UART Endpoint](Endpoints/UART.md#callbacks) is renamed to `on_bytes_received` and now receives a `bytes` object holding one or more bytes, as documented. Before, it was called once per byte with an `int`.
- Bugfix: You can now update your ExecutionPlatform from the web interface when you reach it through a reverse proxy on a different port. Before, the update page stayed empty.
- Bugfix: The documentation of `Start` of the [I2C-Target Endpoint](Endpoints/I2C-Target.md) stated that a read transfer to an address in `AckedAddresses` is not acknowledged if no response was prepared with `respond_to_read`. The endpoint acknowledges it, returns `0xFF` for every byte read and invalidates strict mode. The same applies once a prepared response is exhausted. The behavior is unchanged; the documentation now describes it correctly.
- Bugfix: A `PWM` input of the [FMI Endpoint](Endpoints/FMI.md) provided the duty cycle of an earlier measurement after `start_measuring` of the [PWM Endpoint](Endpoints/PWM.md), e.g. from the previous test, until the first period of the new measurement completed. It now provides `NaN` until then, as documented.
- Bugfix: VS Code showed a `TypeError` hint when including `EP.py` library in Robot test frontend. Test execution was unaffected. Introduced with 9.7.0. Fixed.
- Bugfix: On some ExecutionPlatform units the [I2C-Target Endpoint](Endpoints/I2C-Target.md) did not reliably pull SDA low, so the DUT saw its transfers as not acknowledged. Fixed.
- Bugfix: The [CAN](Endpoints/CAN.md) and [CAN-FD](Endpoints/CAN-FD.md) Endpoint could not send after `Start` if the DUT had driven its CAN TX pin low while the endpoint was stopped, e.g. during power-on or by sending a frame. `Send Frame` then blocked until the DUT itself sent a frame. The endpoint now sends as soon as the bus is idle.

J-Link / Flasher version in use: V9.88

# 9.7.0

- Change: The Endpoint docs now specify the `Model functions` a [System Endpoint](Endpoints/System.md) model can call safely, for example `set_high` of [GPIO](Endpoints/GPIO.md) or `send_frame` of [CAN](Endpoints/CAN.md). Access to all other keywords is now restricted.
- Change: The software package now ships a `robot.toml` in its root that defines `ep_url`, `ep_app_dir` and `ep_binary_dir` in a more flexible way. These variables were formerly set in `.vscode/settings.json`. `robot.toml` is read by the [RobotCode](https://robotcode.io/latest/) extension and the `robotcode` command line, but not by the plain `robot` executable. See [Software Package](SoftwarePackage.md#use-software-package-to-write-and-run-tests).
- Change: The [EEPROM-SPI](Endpoints/EEPROM-SPI.md) Endpoint now erases its content after each test. It therefore behaves similar to all other Endpoints.
- Change: The documentation is now fully in-sync with the actual implementation. In some cases documented keyword argument names differed from the actual implementation name. Existing test suites are unaffected.
- Change: [GPIO](Endpoints/GPIO.md#controlling-functions) `Generate High/Low Pulse On Pin` now reject a pulse on a pin whose previous pulse is still running.
- Change: [GPIO](Endpoints/GPIO) `Read As Bool` is now accessible only from models and can no longer be called from tests. Use Read, which returns HIGH or LOW.
- Feature: The software package now supports **AI coding assistants** such as Claude Code, Cursor, GitHub Copilot and Codex. It contains this documentation as Markdown in `docs/`, matching the package version, and an `AGENTS.md` in its root that these assistants read automatically. `AGENTS.md` points them to the right documentation and tells them what to check and what to ask you before writing tests (e.g. EP address, DUT pin mapping). As a starting point, `docs/TestCapabilities.md` and `docs/ModelCapabilities.md` list per endpoint type all keywords a test and all functions a Python model can use, with their arguments and the ep-config section they require. See [Software Package](SoftwarePackage.md). This support is intended for Robot Framework and will be extended to pytest in the next release.
- Feature: Added `ep-config`, a **command line tool for validating and building ep-configs** from scripts or CI. See [Command line usage](EP_Config.md?id=command-line-usage-ep-config).
- Feature: Added experimental support for running dynamic models with the [FMI Endpoint](Endpoints/FMI.md). The [Functional Mock-up Interface (FMI)](https://fmi-standard.org/) is a standard for exchanging dynamic models, supported by major tools such as [Simulink&reg;](https://www.mathworks.com/products/simulink.html) and [OpenModelica](https://openmodelica.org/).
- Feature: The [Memory-Mapped-I2C Endpoint](Endpoints/MEMORY-MAPPED-I2C.md#callbacks) now provides the `on_register_set` callback, so a system model can react to a register the DUT writes. Registers written with `Set Register` are not reported.
- Feature: The [UART Endpoint](Endpoints/UART.md) now accepts up to 400 bytes per transmission, up from 200. `Transmit`, `Transmit Periodically` and `Transmit With Different Setup` all take the larger size.
- Bugfix: Adding an [ep-config](EP_Config.md) in the Customer Portal no longer fails silently when the configuration is misaligned.
- Bugfix: `Disable Automatic Strict Mode Check` of the [CAN](Endpoints/CAN.md), [CAN-FD](Endpoints/CAN-FD.md), [UART](Endpoints/UART.md) and [System](Endpoints/System.md) Endpoint remained in effect for all following tests of a suite. The keyword now only applies to the current test, as it already did for the other endpoints.

J-Link / Flasher version in use: V9.66

# 9.6.0
- Feature: Added experimental support for **pytest**.
    - Access it through the new `app/EP_pytest.py` library.
    - Download the software package and open `example/hello_world/test_hello_world.py` for a documented reference start.
    - For a discussion, see [pytest feature request](https://feedback.embeff.com/posts/1/support-pytest).
    - In the future, both Robot and pytest will be supported as test frontends for the ExecutionPlatform.
    - Note that `EP.py` usage for Robot execution did not change with this result, although it has now a dependency to a new `EP_common.py` file.
- Feature: [CAN-Endpoint](Endpoints/CAN.md#technicals) support additional bitrates.
- Feature: The [UART Endpoint](Endpoints/UART.md) now supports unlimited receive buffer size.
- Feature: Added helper to build cmake targets from test sequences. See `app/cmake.resource` (Robot Framework) and `app/cmake_helper.py` (pytest) in the Software Package.
- Feature: Improved documentation and added a chapter on [version compatibility](VersionCompatibility.md) and how to [integrating the software package into customer projects](SoftwarePackage.md).
- Feature: The `System` block in ep-config is now optional and must be ommited for system test binaries (firmware without the EP runtime package). When ommited, `DUT Invoke`, `DUT Get Output` and `DUT Get Coverage` are unavailable. See [EP Config](EP_Config.md) and [System Endpoint](Endpoints/System.md). The old parameter `wait_for_app_loaded` in `DUT Flash Firmware` and `DUT Power On` is not required anymore and was removed. The EP decides this based on the .ep-config in use.
- Bugfix: Exceptions in a [System Endpoint](Endpoints/System.md) callback now fail the test instead of only being logged.
- Bugfix: `Wait For Frame Buffer` of the [CAN](Endpoints/CAN.md) and [CAN-FD](Endpoints/CAN-FD.md) Endpoint no longer rejects an `ExpectedCount` above 100.
- Bugfix: `Wait For Data Written` / `Wait For Data Read` of the [I2C-Target Endpoint](Endpoints/I2C-Target.md) no longer reject an `ExpectedCount` above 200.

J-Link / Flasher version in use: V9.66

# 9.5.2
- Bugfix: Improved handling of flashing failures.

J-Link / Flasher version in use: V9.54

# 9.5.1
- Change: Our examples now produce .hex files and use them for DUT Flash Firmware.
- Bugfix: Resolved a rare duplication of individual values during high-throughput data bursts over the interface.
- Bugfix: Resolved an issue that prevented licenses from being replaced.

J-Link / Flasher version in use: V9.44

# 9.5.0

- Change: User authentication is now done through port `80`. Before both test execution and authentication used port `8270`. See [port usage](Service.md#port-usage). Please note that using older `EP.py` versions with devices that are updated to this release will emit an `Can't connect to '<hostname>': Check your settings for ${ep_url}.` error instead of the version compatibility mismatch.
- Bugfix: Add error handling for usage of [Analog-In Endpoint](Endpoints/Analog-In.md) for DUTs that do not have any Analog-In capable signals.
- Bugfix: Fixed a problem that led in some cases to `Internal Error (2)` when creating ep-configs.
- Bugfix: [GPIO](Endpoints/GPIO.md#setup-functions) `Measure Between Rising Edges` and `Measure Between Falling Edges` were stuck in `Measurement still running` when using the same pins for start and stop condition. Fixed.
- Feature: Improved code completion so that only relevant keyword [categories](Endpoints.md?id=categories) are shown for tests and models.
- Feature: The System Endpoint now supports a [timer callback](Endpoints/System.md#callbacks) for periodic actions in the Python-based environment simulation.

J-Link / Flasher version in use: V9.38a

# 9.4.0
- Bugfix: In rare instances the connection to the GDB Remote Server for [DUT Debugging in VS Code](DUT-Debugging.md) was not successful due to a timing issue. Fixed.
- Bugfix: The scope of `EP.py` Robot Framework Library is now `SUITE` instead of `GLOBAL`. This limits the number of active connections during a test run. It should not have any implications on using the library.
- Feature: Our software package is now compliant with `-Werror=vla`.
- Feature: Our software package now defines all empty signatures as `(void)` and is therefore compliant with `-Werror=strict-prototypes`.
- Feature: The [I2C-TARGET Endpoint](Endpoints/I2C-Target.md) now supports unlimited number of transfers and unlimited total data size for write.
- Feature The [CAN](Endpoints/CAN.md) & [CAN-FD](Endpoints/CAN-FD.md) now supports unlimited number of frames.
- Feature: A new compile flag `EP_CUSTOM_MAIN` allows customer code to define an individual `main` function without modifying our software package. This function needs to call `ep_main`.

J-Link / Flasher version in use: V9.12

# 9.3.0
- Feature: New [Memory-Mapped-SPI Endpoint](Endpoints/MEMORY-MAPPED-SPI.md) to simulate an SPI-Slave device.
- Bugfix: Removed some inconsistencies between Endpoints in the technical documentation
- Bugfix: A config built in the customer portal was forever in state `Scheduling` if the same config was built before. Now it is switching to `Ready` immediately.

J-Link / Flasher version in use: V8.92

# 9.2.1
- Bugfix: In some situations test suite start led to `OSError: [Errno 16] Device or resource busy`. Fixed.
- Bugfix: Loading incompatible .ep-configs led to `ExecutionPlatform is currently in use. Waiting...` until a full restart was performed. Now fully fixed.

J-Link / Flasher version in use: V8.92

# 9.2.0
- Bugfix: Clarified that Python compatibility is only up to `3.13`. `3.14` is currently not supported due to unresolved dependency issues.
- Bugfix: Loading .ep-configs with a version smaller than 9.0.0 did not produce a compatibility check fail. Instead the the ExecutionPlatform blocked for all tests with "ExecutionPlatform is currently in use. Waiting..." until a full restart was performed. Fixed.
- Feature: New [System Endpoint](Endpoints/System.md) for Python-based environment simulation that runs parallel to the test sequence.
- Feature: New [Pin-Follow Endpoint](Endpoints/Pin-Follow.md) to track/follow signals precisely.
- Feature: New [Memory-Mapped-I2C Endpoint](Endpoints/MEMORY-MAPPED-I2C.md) to simulate an I²C target.
- Feature: The [Eth-PHY Endpoint](Endpoints/Eth-PHY.md) now supports 100Mb/s and keywords for load testing.

J-Link / Flasher version in use: V8.82

# 9.1.0
- Feature: From now on, each release will contain an update to a recent software stack from SEGGER and document the version used. This reduces compatibility issues when using [DUT Debugging](DUT-Debugging.md).
- Feature: The [UART Endpoint](Endpoints/UART.md) now supports lower bitrates down to `2400bit/s`.
- Feature: New [EEPROM-SPI Endpoint](Endpoints/EEPROM-SPI.md).
- Bugfix: Inform user with clear instructions when wrong version of `fastapi_websocket_rpc` dependency is used.
- Bugfix: After using [DUT Debugging](DUT-Debugging.md), normal operation sometimes yielded `Timeout: DUT did not reach 'ep_process_loop'`. Introduced in 9.0.0. Fixed.
- Bugfix: In the customer portal, reusing a config name from another DUT was accidentally forbidden. It is allowed now.

J-Link / Flasher version in use: V8.74a

# 9.0.1
- Bugfix: Generation time for ep-configs that used `UART` endpoints were very long. Fixed.
- Bugfix: `ANIN` trigger pins were not resetting at test teardown. Fixed.

# 9.0.0
Please note that this is a major release for the new ExecutionPlatform Rev-E from fall 2025.
It will not install on older ExecutionPlatform hardware.

Rev-E brings the following new features and changes:
- 8 independent analog inputs that can be used to sample analog DUT signals.
- 24 independent analog outputs.
- More resources to support more complex configurations.
- 8 shared logic analyzer outputs that can be configured to show both digital and analog signals.
- Special debugging through the second Ethernet port is discontinued in favor of more flexible connection options through the DUT PCB.
- A button to reset the network and user settings to factory values.
- DUT vcc power configurable (available in an upcoming software version)
- DUT power consumption measurement (available in an upcoming software version)
- USB connection for logic analyzer (available in an upcoming software version)

From a user perspective, there are changes, new features and bugfixes:
- Change: ep-config must now define `Baudrate` in `System.UART`. Use the appropriate value from your DUT BSP.
- Change: ep-config must now define individual `ANOUT` endpoint instances consistent with other endpoints. See [Analog-Out configuration](Endpoints/Analog-Out.md#Configuration) for more details.
- Change: ep-config `Analyzer.Digital` section was renamed to `Analyzer.Output`. This is to reflect that analog signals can now also be included there. See [Analyzer configuration](Endpoints/Analyzer.md#Configuration) for more details.
- Change: Instead of a fallback IP, you can now restore the [factory settings](Service.md#factory-settings-and-reset) with a new button.
- Feature: New [Analog-In Endpoint](Endpoints/Analog-In.md).
- Feature: `DUT Flash Firmware` supports `hex` (in addition to `bin` files).
- Feature: Functions to set the pin level in the [GPIO endpoint](Endpoints/GPIO.md#controlling-functions) now return the precise _test time_ when they went effective.
- Feature: When `ep_abort` is called on the DUT, the message is now shown to the user.
- Bugfix: `ep_register` failed without notification. Abort test when `ep_register` fails.

# 8.4.1
- Bugfix: The `Pull` setting for [GPIO](Endpoints/Eth-PHY.md) endpoints had no effect. Fixed.
- Bugfix: No more `No such file or directory: '/tmp/ep_configs'` after 10 days of inactivity.

# 8.4.0
- Change: Our examples now expect `ep-config` files inside `${ep_binary_dir}`. `${ep_bsp_dir}` is [not used anymore](EP_Config.md).
- Change: Our software package uses a standard CMakePresets.json instead of a VS Code specific CMakeKits.json. Refer to `README.md` inside the software package for more details.
- Change: Our software package automatically defers software capabilities from the BSP through CMake properties. If you use a custom BSP, you must define a property `EP_BSP_APPS_TESTED` for your BSP library. It needs to include a list of apps that should be supported (and compiled) when using that BSP. If your BSP CMake target is named `target_bsp` and should support all apps, you can add this line to your `CMakeLists.txt`:
    ```
    set_target_properties(target_bsp PROPERTIES EP_BSP_APPS_TESTED  "generic;bench;gtest;unity")
    ```
- Feature: The Customer Portal at https://customer.embeff.com is the new central place to access all information and tasks related to your ExecutionPlatform devices. The Customer Portal replaces all former downloads on the device itself.
    - Use the `DUTs` tab to view schematics and download BSPs for all your DUTs.
    - Use the `Configurations` and `Configurations Download` tab to write, manage and [build](GettingStarted.md#downloads-in-customer-portal) configurations for all your DUTs.
    - Use the `Releases` tab to download a software package and corresponding update file for your ExecutionPlatform.

- Feature: The [Eth-PHY](Endpoints/Eth-PHY.md) Endpoint supports the Serial Management Interface (MDC,MDIO).

# 8.3.3
- Bugfix: Clarified a misleading error message for errorenous `${ep_url}` settings.
- Bugfix: Updated Flasher Library to V8.16e to improve flash reliability for some DUTs.

# 8.3.2
- Bugfix: Trying to use GPIO Endpoints with lowercase letters resulted in an `EndpointUsageError`. Fixed.
- Bugfix: The [json schema](EP_Config.md#Schema-for-configuration-file) and the ep-config online generator now enforce valid endpoint names.
- Bugfix: Updated Flasher Library to V8.12e to mitigate flash problems for some DUTs.

# 8.3.1
- Change: Release 8.3.0 introduced a limitation that the user cannot change the name of a generated ep-config file. This was not properly documented.
- Feature: Internal improvements for a reduced update file size and faster ExecutionPlatform bootup.
- Bugfix: Improved error message handling when user renames a configuration file (which is forbidden).
- Bugfix: Removed potential name conflicts when using ep-configs.

# 8.3.0
- Change: `.` is now disallowed in all endpoint names. You may use `_` instead.
- Feature: Code completion for keywords does not require a connected ExecutionPlatform anymore. It is available offline.
- Feature: New [PHY Endpoint](Endpoints/Eth-PHY.md).

# 8.2.2
- Change: `Run Gtest Tests` is now displaying output when test is failing.
- Feature: Added support to benchmark functions without parameters.
- Bugfix: In some rare cases `GPIO Measure` functions did not wait for a trigger and always returned `0`. Fixed.

# 8.2.1
- Bugfix: `DUT Flash Firmware` implicitly assumed that its input file is of type `.bin`. A check for that was added.
- Bugfix: Updated Flasher Library to V8.10d (patched) to mitigate flash problems for some DUTs.

# 8.2.0
- Feature: Support for on-target [coverage](Usecase/Coverage.md).
- Feature: Full text search for our online documentation. Insert your search term in the upper left corner.
- Feature: New [CAN-FD Endpoint](Endpoints/CAN-FD.md).

# 8.1.0
- Change: Options for `ep_debugging` changed. `False` is now called `Off`. `True` is now called `External`. A new mode is introduced below as a feature.
- Change: Our `add_bin_from_elf` cmake macro was renamed to `ep_add_bin_output` and deduces the resulting bin name instead of taking it as a second argument.
- Change: `Run Unity Tests` is now displaying output when test is failing.
- Change: The EP runtime does not use dynamic memory anymore for storing `ep_register` callbacks. You may fine tune the static memory consumption by defining `EP_REGISTER_MAX_CALLS` and `EP_REGISTER_MAX_TOTAL_ID_LEN` when compiling `ep_runtime.c`.
- Change: Minimum cmake version is now 3.23 for our software package.
- Change: Our sample bsps now use the ".elf" extension for executables (they set `CMAKE_SYSTEM_NAME` to `Generic-ELF`).
- Change: The configuration to enable the [Analog-Out](Endpoints/Analog-Out.md) endpoint changed from `ANOUT:` to `ANOUT: []`.
- Change: EP Apps are now declared as INTERFACE libraries instead of STATIC. This requires no user side changes. For details look at ap_app_create in `BuildHelper.cmake`.
- Change: Increased recommended version of [RobotFramework](https://robotframework.org) from 6.1 to 7.0.1.
- Change: [CAN Endpoint](Endpoints/CAN.md) Renamed the BusError enum from `Crc` to `CRC` to match documentation.

- Feature: New `VSCode` mode for `ep_debugging`. For J-Link based EPs, you can now [debug the DUT directly from VS Code](DUT-Debugging.md#vscode).
- Feature: The EP runtime is rewritten in C and provided as a single source file `ep_runtime.c`.
- Feature: When `ep_abort` is called, the given reason is shown in the RobotFramework error message.
- Feature: Added a [json schema](EP_Config.md#Schema-for-configuration-file) for validation and auto-completion when working with ep-config.yaml files in VSCode.
- Feature: Included schema checking in ep-config generator and improved error messages.
- Feature: New optional flag `wait_for_app_loaded` in [DUT Flash Firmwware and DUT Power On](CodeInterface.md). The default is `${True}` and is identical to the existing behavior.
- Feature: Updated JLink Library to V7.98.
- Feature: Show a log message when MCU is programmed.

- Bugfix: No more `No such file or directory: '/tmp/userbins/*'` after 10 days of inactivity.
- Bugfix: [I2C-Target Endpoint](Endpoints/I2C-Target.md) now accepts an empty parameter for `AckedAddresses` instead of just the special RF variable `@{EMPTY}`.

# 8.0.2
- Feature: The [Analyzer Endpoint](Endpoints/Analyzer.md) can now display the runtime communication between ExecutionPlatform and DUT.
- Bugfix: The [Analyzer Endpoint](Endpoints/Analyzer.md) now supports `.` symbols inside the endpoint instance name.

# 8.0.1
- Bugfix: Improved user (error) messages that are shown when connecting to the EP.
- Bugfix: GPIO Measurements work for configurations with more then 128 GPIO Endpoints.
- Change: The recommended Visual Studio Code extension for Robot Framework support is now [robotcode](https://robotcode.io/latest/). We use this extension in our default configuration. It offers consistent functionality and syntax highlighting across different `.ep-config` files. See [robot library documentation](EP_Config) for how to configure it for your environment.
- Feature: Updated JLink Library to V7.94j.

# 8.0.0
- Feature/Change: All access to the ExecutionPlatform is now controlled by a [user management](Service#user-management).
    - Devices shipped with older releases start with a default administrator account (user name _admin_, password _admin_).
    - In our examples the global robot variable `${ep_address}` is replaced by the global robot variable `${ep_url}` that contains the former address but also the user credentials in the form _user:password@address_.
- Feature: Common parts of our software package are now backward-compatible with gcc 4.8 (formerly gcc 4.9).

# 7.0.2
- Bugfix/Change: [Fallback IP](Service#network-settings) 10.0.0.120 is now accessible with Debugger Ethernet to avoid IP conflicts on Network Ethernet.
- Change: Web frontend redesigned in preparation for upcoming improvements.

# 7.0.1
- Bugfix: I2C arbitration loss applies now to all bits of the first byte (not just LSB).
- Change: Each BSP contains only a single "gpio.ep-config". This sample config makes all digital DUT pins available with a GPIO Endpoint. See our [documentation](https://ep-docs.embeff.com/#/EP_Config?id=creating-a-new-configuration) on how to create your own custom configs.

# 7.0.0
Please note that this is a major release. It requires that ExecutionPlatform and Software Package are updated to the new version.

- Change: Increased required version of [RobotFramework](https://robotframework.org) from 4.0 to 6.1.
- Change: New package requirement for user library `fastapi_websocket_rpc`. See [Requirements](GettingStarted#requirements) for details.
- Change: GPIO Endpoint `Set Pins` function was renamed to `GPIO Set Pins`.
- Change: CAN Endpoint removed keyword `Get RX Buffer Fill`. Use `Get Frames` and check length instead.

- Feature: Concurrent usage is now possible. Once a user starts a test suite, other user requests are automatically halted until the test suite finishes. A yellow connection LED indicates that a user session is active.
- Feature: New [Incremental-Encoder Endpoint](Endpoints/Incremental-Encoder.md).
- Feature: New [I2C-Target Endpoint](Endpoints/I2C-Target.md).
- Feature: GPIO Endpoint has a new keyword `Read Pins` to read multiple pins at once.
- Feature: The ExecutionPlatform automatically checks version compatibility for all components (ExecutionPlatform, EP.py user library, Software Package, .ep-config file).

- Bugfix: When a timeout occurs during DUT communication, further data sent by the DUT is no longer crashing the test but ignored.
- Bugfix: Long standard output without line breaks is now properly processed.
- Bugfix: Visual Studio Code Intellisense is enabled for the sample package

# 6.0.0
Please note that this is a major release. It requires that ExecutionPlatform and Software Package are updated to the new version.

- Feature: New [PWM Endpoint](Endpoints/PWM.md).
- Feature: Increased limit for maximal [GPIO Endpoint](Endpoints/GPIO.md) count from 32 to 144.
- Change: BSP must now implement `char const* ep_bsp_reference_pin_get_external_name()`. The file `bsp.resource` was removed for simplification. See [Use Case - Benchmarks](Usecase/Benchmark.md) for details.
- Change: Unified error handling in endpoints. All endpoints use the same [set of errors](Endpoints.md#error-handling).


# 5.0.1
Please note that this is a major release. It requires that ExecutionPlatform and Software Package are updated to the new version.

- Change: Support for ExecutionPlatform Rev-D.
- Change: Configuration files are supplied by the user. The usage of `EP.py` changed accordingly. All relevant information around this change is documented [here](EP_Config.md).
- Change: Example folder contains only examples that run on the specific DUT. For full examples with all endpoints, refer to our [online demo](https://embeff.com/ep-demo-1/).

- Feature: Support for up to 24 analog-out channels. `Connect` is now required to select how an [Analog-Out](Endpoints/Analog-Out.md) endpoint should be connected to an analog-out pin of your DUT.
- Feature: New [Analyzer Endpoint](Endpoints/Analyzer.md). You can now connect an external logic analyzer and view any of your DUT signals.
- Feature: New [UART Endpoint](Endpoints/UART.md).
- Feature: Allow user to [control DUT Power](CodeInterface.md). New keywords `DUT Power On` and `DUT Power Off`. By default, the DUT power is off when a Test Suite is started. The DUT Power is automatically enabled after `DUT Flash Firmware` is called. No automatic actions are taking place for Test Setup and Test Teardown. So by default all tests within a suite are executed without cold-starting the DUT in between. You may add `DUT Power Off` and `DUT Power On` in Test Setup to ensure a cold start for each test.
- Feature: `DUT Flash Firmware` and `DUT Power On` now block until DUT reaches `ep_process_loop` after power up. An error is raised if the DUT is not booting up properly.
- Feature: You can view a live log at `<ep-ip>/log`. Log file now also includes suite start/end and test start/end events.
- Feature: Improve error messages for `DUT Invoke`.
- Feature: Significant speed improvements in test execution.
- Feature: Better error handling if EP is not available.
- Feature: Updated JLink Library to V7.88c.

# 4.3.1
- Feature: Updated JLink Library to V7.84e
- Bugfix: Fixed situations where JLinkFlashException occured.

# 4.3.0
- Feature: Added support for [CAN](Endpoints/CAN.md).
- Feature: Update JLink Library to V7.82c (For J-Link based EPs)
- Change: All [controlling functions](Endpoints.md#controlling-functions) will now wait for completion. For example creating a GPIO pulse of length 1s will block the test for this one second.The old behavior can be restored by providing the `WaitForCompletion=False` argument.
- Change: Analog Set Sin/Square/Sawtooth default count changed to 1.
- Bugfix: Calling `DUT Invoke` while the DUT is still starting was resulting in an unclear `DUT not responding` error. It now results in a `DUT not ready` error.
- Bugfix: Fix sporadic `DUT Invoke called but DUT not ready` errors after `DUT Flash Firmware`.

# 4.2.0
- Feature: Support for physical units added.
    - Keywords parameters that are a physical units can now use readable postfixes to specify a number. For example `52.7us` for 52.7 microseconds.
    - More details are available in our documentation for [Physical units](Endpoints.md#physical-units)
    - Keywords that return units support an optional `ReturnType` parameter. Existent keywords that contained the return type in their keyword name (such as `Get Measurement Result in ns`) are deprecated.
- Feature: Improved documentation [Byte Data](Endpoints.md#byte-data) handling in test sequences.
- Change: `GPIO Generate [High|Low] Pulse On Pin` now expect a parameter `PulseLength` that contains a unit. The old parameter `PulseLength_ns` is no longer supported.
    - For example `GPIO Generate High Pulse On Pin    PB5    10000` must be changed to
      `GPIO Generate High Pulse On Pin    PB5    10000ns`  or for better readability `GPIO Generate High Pulse On Pin    PB5    10us`
- Change: Default interrupts such as systicks are now enabled in all our sample BSPs. They can be disabled with the new bsp function `ep_bsp_disable_default_irqs()`. This function must be implemented for custom BSPs.
- Bugfix: Sporadic `Internal error during dut_invoke: 'NoneType' object is not subscriptable`
- Bugfix: `DUT Get Output` now also fetches standard output that was not explicitly synchronized by line break or flush.

# 4.1.1
- Feature: When debugging is active, wait for user to confirm that debug connection can be closed.
- Bugfix: Small corrections in user documentation.

# 4.0.1
Please note that this is a major release. It requires that all components (Device, BSP) are updated to the new version.
This update changes the way you work with the EP. All workflows are now controlled and executed by [Robot Framework](https://robotframework.org/).
This provides a huge boost in usability and productivity. Refer to the [Getting Started](GettingStarted) part for more instructions.

- Feature: Unified test execution from Robot Framework replaces CLI.
- Feature: Robot helper libraries for Unit Testing and Benchmarking.
- Feature: Benchmarking can be configured to use a different DUT pin.
- Discontinue: Chaining multiple devices is no longer supported.

# 3.2.0
- Feature: BSP restructuring to ease integration effort. For details refer to `README.md` within your BSP.

# 3.1.0
- Feature: Dynamic app `ep_invoke` now supports longer and non-ascii payloads.
- Feature: New endpoint SPI-Slave.
- Bugfix: Pulses generation is now properly reset in `Endpoint Teardown`.

# 3.0.2
- Bugfix: Pulse generation length correct for all pins.
- Bugfix: Removed occasional glitches in communication between testfile and test runner.

# 3.0.0
Please note that this is a major release. It requires that all components (Device, CLI, BSP) are updated to the new version.

- Feature: New app `target_runner_dynamic`.
- Feature: Support for Periphery Testing.
- Bugfix: CLI now handles external config files (-config flag) properly.
- Bugfix: CLI now uses a timeout set in a config file.

# 2.1.2
- Bugfix: IP addresses acquired in DHCP were sometimes not discarded when using static network configuration. This could cause connectivity issues.

# 2.1.1
- Feature: Support for remote debugging with external debuggers.
- Change: Unify output transmission across all targets with bsp function `ep_internal_flush`.

# 2.0.3
Please note that this is a major release. It requires that all components (Device, CLI, BSP) are updated to the new version.

- Change: Support for ExecutionPlatform Rev C
    - More flexible microbenchmarks (timings are measured instead of clock cycles)
    - Support for offline updates
    - Contains a single microcontroller
    - Power supply changed to 24V

- Feature: Configuration in webbrowser replaces terminal.
- Feature: Offline updates. The firmware can now be replaced with update files on devices that do not have an internet connection.
- Feature: Chaining allows to connect multiple devices with a single ExecutionPlatform acting as as a controller. Requests for all connected targets are sent only to the controller and dispatched automatically to the device supporting this target.
- Feature: The CLI "type" parameter is not required anymore as it discovered automatically.
- Change: The bench application uses an enhanced header structure and renamed keywords to `ep_bench_register`, `ep_bench_invoke`, `ep_bench_print` and `ep_bench_serialize`.
- Change: The shipped unity example is now written in C.
- Change: For ARM remote debugging the JLink DLL was updated to 7.54.1.

# 1.5.0
- Feature: The BSP in CMake is now auto detected if no `-DBSP_DIR` is given and a single bsp directory exists.
- Change: Update target_runner_gtest to use the official Google Test version 1-11.0. This requires an update to arm gcc 9.3.
- Feature: Calling the CLI will now return the application return code in the request. This makes it possible to integrate unittest requests into a CI setup without parsing the actual outputs.

# 1.4.0
- Feature: Support static ip configurations.

# 1.3.0
- Bugfix: Check for filesystem corruption at boot time.
- Bugfix: Improve CLI error handling in corner cases.
- Feature: Allow user to shutdown and restart the device.

# 1.1.2
Release notes starting.
