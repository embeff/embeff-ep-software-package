# Getting started
In this section you learn how to use the ExecutionPlatform in our example environment.

You will
1. Install all required software.
1. Connect your EP and download the software package.
1. Compile and run the first example.

We recommend that you go through these steps first and then read about our [Software Package](SoftwarePackage.md) to learn how to integrate the ExecutionPlatform in your specific project.

# Requirements
The EP can be used on Windows, Linux and macOS.

The following software is required:
- [Visual Studio Code](https://code.visualstudio.com/)
- [python 3.10 - 3.14](https://www.python.org/downloads/)
- The Python packages of your test framework, see below.
- [cmake 3.23 (or newer)](https://cmake.org/download/)
- [ninja](https://ninja-build.org/) or another cmake-compatible build system such as make.
- A cross compiler for your DUT. The reference software uses [arm-none-eabi-gcc 13.2.Rel1](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads).

Make sure all of them are installed and in your [PATH](https://en.wikipedia.org/wiki/PATH_(variable)).

Both test frameworks use the Python package [embeff-ep](https://github.com/embeff/embeff-ep). It contains the Robot Framework library `EPLibrary`, the pytest plugin and the `ep-config` command line tool, and brings Robot Framework, pytest and the other packages it needs. Its version must match your ExecutionPlatform. The `requirements.txt` of the software package names the matching version, so from the folder of the software package (see [Downloads](#downloads-in-customer-portal)) install it with
```
pip install -r requirements.txt
```
We recommend a [virtual environment](https://docs.python.org/3/library/venv.html) per project (`python -m venv .venv`, then activate it), so that every project keeps the embeff-ep version of its software package. Upgrading from a version before 9.8? See [Migrating to 9.8](Migration/9.8.md).

<!-- tabs:start -->

#### **Robot Framework**

Nothing else: `requirements.txt` also installs [robotcode](https://robotcode.io/latest/), which runs tests from the command line. It reads the `robot.toml` of the software package, which defines `ep_url` and the other [variables](SoftwarePackage#use-software-package-to-write-and-run-tests) our examples use. Run it from the environment you installed into: a robotcode installed elsewhere runs a Python without embeff-ep.

#### **pytest**

Nothing else: the pytest plugin registers itself when embeff-ep is installed.

<!-- tabs:end -->

# Network connection
Connect the ExecutionPlatform to your local network. The network is pre-configured as requested by the customer. To look up the factory settings
1. Open https://customer.embeff.com/ and log-in.
2. Open the `Devices` tab and click on the ExecutionPlatform you are looking for.
3. The factory settings box displays information about the network settings.

See [network settings](Service#network-settings) on instructions how to change these settings.

The hostname of your device is set to its serial number, which you will find on its type plate.
Power up the device using the provided power supply. After 90s the power, DUT and connection LED should turn green and indicate a device ready to use.

# Downloads in Customer Portal
To get started, you need to download:
- The generic software package.
- A specific BSP for your DUT.
- Specific [.ep-config.zip](EP_Config.md) files required by your test cases. embeff demos at least require `gpio.ep-config.zip`.

You find all these downloads within the [Customer Portal](https://customer.embeff.com). To access it, you need your customer number and a password.
- Open the `Releases` tab and download the `Software Package` that matches your ExecutionPlatform device version. To identify your device version, open `http://<ep-serial-number or ip>`. Update the device version if required.
- Open the `DUTs` tab and click on a DUT row. You find the download for the `default-c` example `BSP` at the bottom of the popup.
- Open the `Configuration Download` tab and filter for the version that matches your `Software Package`.  Select all configurations and click download.

The software package is also published on GitHub at [embeff/embeff-ep-software-package](https://github.com/embeff/embeff-ep-software-package), with one release per ExecutionPlatform version. Instead of downloading it from the Customer Portal you can clone the release that matches your device, for example `git clone --branch v9.8.0 https://github.com/embeff/embeff-ep-software-package c:/ep`. The BSP and the ep-config files are only available in the Customer Portal.

To get started with these files:
1. Extract the contents of `Software Release` to a folder of your choice, say `c:/ep`. Skip this step if you cloned the software package.
2. Extract the contents of `BSP` to `c:/ep/bsp/default-c`.
3. Create a new folder `c:/ep/build`.
4. Extract the ep-config download and move all `ep-config` files to `c:/ep/build`.

# VS Code configuration
1. Open the folder from the previous step (here `c:/ep`) in Visual Studio Code.
1. Confirm that you want to use the `EP default-C BSP` CMake preset.
1. :warning: VS Code will ask you to install a list of recommended extensions. The manual assumes that you install all these plugins. You can find the list in `.vscode/extensions.json`.
1. Tell the tests where your EP is:

<!-- tabs:start -->

#### **Robot Framework**

Open `robot.toml` and change `ep_url` to match the username, password and address for your EP, eg. to `michael:secretpass@ep-a201-ff1a`.

The example tests expect the `.ep-config.zip` files within the `build` directory.

#### **pytest**

1. `pyproject.toml` already lists `build/gpio.ep-config.zip` for the example. Configurations that are imported by your own tests go there too, see [Importing ep-configs in Python](SoftwarePackage.md#importing-ep-configs-in-python).
1. Copy `.env.example` to `.env` and adapt it. VS Code reads `.env` and lists the tests in its Testing view. pytest reads these environment variables:
    - `EP_URL`: the username, password and address of your EP, eg. `user-id:password@ep-0000-0000`.
    - `EP_BINARY_DIR`: the folder with the DUT binaries, here `build`.

<!-- tabs:end -->

Press <kbd>F7</kbd> to build everything.

# Run example in VS Code

<!-- tabs:start -->

#### **Robot Framework**

<div style="padding:56.25% 0 0 0;position:relative;"><iframe src="https://player.vimeo.com/video/744135758?h=af6763a5fe&amp;badge=0&amp;autopause=0&amp;player_id=0&amp;app_id=58479" frameborder="0" allow="autoplay; fullscreen; picture-in-picture" allowfullscreen style="position:absolute;top:0;left:0;width:100%;height:100%;" title="Getting Started - Run Example in VS Code"></iframe></div><script src="https://player.vimeo.com/api/player.js"></script>


1. Open `example/hello_world/hello_world.robot`.

    This test suite contains 3 tests. It flashes the binary produced by `hello_world.cpp`.

1. Press <kbd>F7</kbd> to build all binaries for the DUT. Now everything is ready to run the tests.
1. Press <kbd>F1</kbd> and select `Robot Framework: Run Tests/Tasks Suite`.

    The test results will be shown. For details open the `log.html` test report.

    ```
    ==============================================================================
    Hello World
    ==============================================================================
    Invoke Square With 3 Gets 9                                          | PASS |
    ------------------------------------------------------------------------------
    Invoke Square With -5 Gets 25                                        | PASS |
    ------------------------------------------------------------------------------
    Call Function That Prints MCU-related Output                          ...
    Analyzing variable 'value':
    - Content: 0x1234abcd
    - Address: 0x200bff84
    - Address[0]: 0xcd
    Call Function That Prints MCU-related Output                          | PASS |
    ------------------------------------------------------------------------------
    Hello World                                                           | PASS |
    3 tests, 3 passed, 0 failed
    ==============================================================================
    ```

1. Corrupt the `square` function in `hello_world.cpp` by adding a 1 to the result.
1. Press <kbd>F7</kbd> to rebuild the binary.
1. Open `hello_world.robot` and run the robot tests again.

    The 2 tests for the `square` function will fail.

     ```
    ==============================================================================
    Hello World
    ==============================================================================
    Invoke Square With 3 Gets 9                                          | FAIL |
    9 != 10
    ------------------------------------------------------------------------------
    Invoke Square With -5 Gets 25                                        | FAIL |
    25 != 26
    ------------------------------------------------------------------------------
    Call Function That Prints MCU-related Output                          ...
    Analyzing variable 'value':
    - Content: 0x1234abcd
    - Address: 0x200bff84
    - Address[0]: 0xcd
    Call Function That Prints MCU-related Output                          | PASS |
    ------------------------------------------------------------------------------
    Hello World                                                           | FAIL |
    3 tests, 1 passed, 2 failed
    ==============================================================================
    ```

#### **pytest**

1. Open `example/hello_world/test_hello_world.py`.

    This test module contains 4 tests. It builds and flashes the binary produced by `hello_world.cpp`. The last test reads the GPIO endpoint `GPIO1`. If the `gpio.ep-config.zip` of your DUT names its pins differently, replace `GPIO1` with one of them.

1. Open the Testing view of VS Code and run the tests.

    To run them on the command line instead, set the environment variables in your shell as described in [Software Package](SoftwarePackage.md#use-software-package-to-write-and-run-tests) and run `pytest example/hello_world/test_hello_world.py` from the root of the software package.

    The test results will be shown:

    ```
    ============================= test session starts =============================
    collected 4 items

    example/hello_world/test_hello_world.py::test_invoke_square_with_3_gets_9
    Compiling target example_hello_world

    Programming MCU...
    PASSED
    example/hello_world/test_hello_world.py::test_invoke_square_with_minus_5_gets_25 PASSED
    example/hello_world/test_hello_world.py::test_call_function_that_prints_mcu_related_output
    Analyzing variable 'value':
    - Content: 0x1234abcd
    - Address: 0x20041fb4
    PASSED
    example/hello_world/test_hello_world.py::test_read_and_print_gpio1 GPIO1: LOW
    PASSED

    ============================== 4 passed in 1.72s ==============================
    ```

1. Corrupt the `square` function in `hello_world.cpp` by adding a 1 to the result.
1. Run the tests again. The test module rebuilds the binary.

    The 2 tests for the `square` function will fail.

    ```
    ...
    example/hello_world/test_hello_world.py::test_invoke_square_with_3_gets_9 FAILED
    example/hello_world/test_hello_world.py::test_invoke_square_with_minus_5_gets_25 FAILED
    ...
    ================================== FAILURES ===================================
    ______________________ test_invoke_square_with_3_gets_9 _______________________

        def test_invoke_square_with_3_gets_9(ep):
    >       assert ep.DUT.invoke("square", "3") == 9
    E       AssertionError: assert 10 == 9
    ...
    ___________________ test_invoke_square_with_minus_5_gets_25 ___________________

        def test_invoke_square_with_minus_5_gets_25(ep):
    >       assert ep.DUT.invoke("square", "-5") == 25
    E       AssertionError: assert 26 == 25
    ...
    =========================== short test summary info ===========================
    FAILED example/hello_world/test_hello_world.py::test_invoke_square_with_3_gets_9
    FAILED example/hello_world/test_hello_world.py::test_invoke_square_with_minus_5_gets_25
    ========================= 2 failed, 2 passed in 4.98s =========================
    ```

<!-- tabs:end -->

# Add your own test
This last section explains
- how to create a binary that runs on your DUT and can communicate with the EP.
- how to write a `.robot` file that is using the EP and is calling functions of your binary.
- how to configure the EP so that the [Endpoints](Endpoints.md) of your choice are available for testing.

## Create a binary
Our [software package](GettingStarted#downloads-in-customer-portal) contains a library `ep_app_generic`. This library establishes the communication between the test runner and your user code. The library uses an Board Support Package (BSP) that contains basic microcontroller software (such as startup and linker definition files).
A sample BSP for your microcontroller is also shipped by us. Typically you will at some point replace this BSP with your own implementation.

:information_source: This section assumes a basic knowledge of CMake.

Perform these steps to create a binary that can be processed on the EP:

1. Add a blank c or cpp file and create a new CMake executable.
1. Link the executable against ```ep_app_generic```.
1. Include the core ep file: ```#include <ep/core.h>```
    - If you want to use standard output like ```printf```, include the appropriate headers: ```#include <cstdlib>``` and ```#include <cstdio>```.
1. Provide the ```int ep_app_main()```function.
    - This function is automatically executed. Do not provide `main`.
    - The last command must be ```return ep_process_loop();```.
1. Use ```ep_register(const char* method_name, Callback function)``` within `ep_app_main` to make one or more functions callable from the test.
    - The test calls the registered function with `invoke` of the [Code Interface](CodeInterface.md), passing the same `method_name`.
    - The registered function must match the type `Callback`, `int32_t(uint8_t const* payload, int len)`.
    - When called from your test case, the payload will always be transferred as list of bytes. If you want to transfer a number, you first need to decode this payload (as shown in the example below).
    - :information_source: The typical use case is to register functions that are either driver functions or otherwise interact with peripherals.

### Example: hello_world.cpp

```cpp
#include <ep/core.h>
#include <cstdlib>

// This is the function we want to test.
// Typically it will reside in some other translation unit.
int32_t square(int num) {
    int res = num*num;
    return res;
}

// This is glue code that the ExecutionPlatform calls with `len` bytes.
// The glue code converts this into the arguments needed for the square function.
// All functions invoked by DUT Invoke need to have the same signature as square_glue.
//
int32_t square_glue(uint8_t const* data, int len) {
    // Convert byte argument to integer
    if (len > 10) return -1;
    // First create null-terminated string
    char cstring[10] = {0};
    memcpy(cstring, data, len);
    int num = atoi(cstring);

    int res = square(num);
    return res;
}

int ep_app_main() {
    ep_register("square", &square_glue);
    return ep_process_loop();
}
```

### Example: CMakeLists.txt
```cmake
ep_app_example( APP ep_app_generic
                NAME example_hello_world
                SOURCES hello_world.cpp)
```

`ep_app_example` is a helper from `cmake/BuildHelper.cmake`. It creates the executable `example_hello_world`, links it against `ep_app_generic` and writes `example_hello_world.bin` and `example_hello_world.hex` to `build/example`. A plain `add_executable` linked against `ep_app_generic` works as well, see [Software Package](SoftwarePackage#use-software-package-to-compile-test-binaries).

## Write the test
You write the test either as a Robot Framework suite or with pytest. Both reach the same [Endpoints](Endpoints.md) and keywords, so pick the one your team already uses.

In this example the test
- flashes the binary to the DUT,
- calls the `square` function on the DUT.

<!-- tabs:start -->

#### **Robot Framework**

Test sequences for Robot Framework are written down in files with the extension `.robot`.
Within these files, you access the ExecutionPlatform functionality through our `EPLibrary` library. Check out the [EPLibrary documentation](SoftwarePackage#use-software-package-to-write-and-run-tests) to understand the variables and parameters.

`example/hello_world/hello_world.robot`:

```robot
*** Settings ***
Library           EPLibrary   ${ep_url}    ${ep_binary_dir}/gpio.ep-config.zip
Suite Setup       DUT Flash Firmware    ${ep_binary_dir}/example/example_hello_world.hex

*** Test Cases ***
Invoke Square With 3 Gets 9
    ${res} =     Dut Invoke  square  3
    Should Be Equal    ${9}    ${res}

Invoke Square With -5 Gets 25
    ${res} =     Dut Invoke  square  -5
    Should Be Equal    ${25}    ${res}
```

Run it from the root of the software package:

```
robotcode robot example/hello_world/hello_world.robot
```

#### **pytest**


Each `.ep-config.zip` is importable as a Python package: `import gpio.test` loads `gpio.ep-config.zip`, and the fixture from `gpio.test.EP.fixture()` connects to the EP with that configuration. The handle gives access to the Code Interface and to the endpoints of the configuration: `ep.DUT.invoke()` calls a function registered on the DUT. The [endpoint pages](Endpoints.md) show every endpoint function by its name, and the [pytest](TestFrontend/pytest.md) chapter explains how to call it.

`example/hello_world/test_hello_world.py`:

```python
import os

from cmake_helper import cmake_build
import pytest
from gpio.test import EP    # imports gpio.ep-config.zip

BINARY_DIR = os.environ["EP_BINARY_DIR"]

ep = EP.fixture()


@pytest.fixture(scope="module", autouse=True)
def suite_lifecycle(ep: EP):
    cmake_build(BINARY_DIR, "example_hello_world")
    ep.DUT.flash_firmware(f"{BINARY_DIR}/example/example_hello_world.hex")
    yield


def test_invoke_square_with_3_gets_9(ep: EP):
    assert ep.DUT.invoke("square", "3") == 9


def test_invoke_square_with_minus_5_gets_25(ep: EP):
    assert ep.DUT.invoke("square", "-5") == 25
```

Run it as described in [Run example in VS Code](#run-example-in-vs-code). Every `.ep-config.zip` your tests import must be listed in `pyproject.toml`, see [Importing ep-configs in Python](SoftwarePackage.md#importing-ep-configs-in-python).

<!-- tabs:end -->

# What next?
This section explained the ExecutionPlatform usage based on our ready-to-run example environment.

- [Software Package](SoftwarePackage.md) explains how to use the ExecutionPlatform in your projects and testing workflows.
- The Test frontend chapters for [Robot Framework](TestFrontend/RobotFramework.md) and [pytest](TestFrontend/pytest.md) explain how your tests call the functions of the endpoints.
