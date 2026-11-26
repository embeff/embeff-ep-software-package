# The Software Package and your projects
In this section you learn how to use the Software Package to integrate the ExecutionPlatform in your projects and testing workflows.

[Getting Started](GettingStarted.md) explained the ExecutionPlatform usage based on our example environment.
You should **read that first** and only then continue to read here.

In practice, your environment will differ in several important aspects from this example environment. The goal of this section is to give you an understanding how to integrate the ExecutionPlatform usage in your project and workflows.

To work with the ExecutionPlatform, you need to
- compile DUT binaries for usage within the tests (not required in [system test mode](EP_Config.md#configurations)).
- write and run tests

Both are fully independent steps. Both depend on parts of the Software Package.
Our example environment combines both in a VS Code project. You might choose different tools / IDEs for each.

The **software package contains everything you need to compile DUT binaries and write/run tests**.

The software package version must match the ExecutionPlatform version in major and minor version. See [Version Compatibility](VersionCompatibility.md).
Login to the [Customer Portal](https://customer.embeff.com), open the `Releases` tab, download the `Software Package` and extract its content.
The same software package is published on GitHub at [embeff/embeff-ep-software-package](https://github.com/embeff/embeff-ep-software-package), see [Updating the software package](#updating-the-software-package).

Your tests reach the ExecutionPlatform through the Python package [embeff-ep](https://github.com/embeff/embeff-ep): the Robot Framework library `EPLibrary`, the pytest plugin and the `ep-config` command line tool. It is not part of the software package; `pip install -r requirements.txt` installs the version that matches it. Coming from a version before 9.8? See [Migrating to 9.8](Migration/9.8.md).

We now take a high-level look at its structure and then digest how to use (parts) of it for DUT compilation and test execution.

## Structure
After extracting the package, you find the following structure:

| File / folder | Description | Used for DUT binary compilation | Used for tests |
| --- | --- | --- | --- |
| `app/` | Various _apps_ support different test use cases, with their CMake targets and test helpers: `cmake.resource` and `cmake_helper.py` build firmware from a test, `bench/`, `gtest/` and `unity/` evaluate the DUT's output. | Mandatory | Mandatory |
| `requirements.txt` | The Python package [embeff-ep](https://github.com/embeff/embeff-ep) in the version of this software package, and robotcode: `pip install -r requirements.txt`. | | Mandatory |
| `src/` | A C-based runtime that is linked to the code under test and establishes a connection between DUT and ExecutionPlatform. Depends on a DUT-specific BSP that implements the interface in `src/include/ep/bsp.h`.  | Mandatory | |
| `example/` | Ready-to-run generic examples that you can use as a starting point. | | |
| `third_party/` | Specific versions of third-party code, for example open source unit test frameworks. | Only for `gtest` or `unity` apps. Adapt to specific version needed. | |
| `docs/` | This documentation for the current hardware revision, as Markdown. | | |
| `AGENTS.md`, `CLAUDE.md` | Instructions for AI coding assistants such as Claude Code, Cursor, GitHub Copilot and Codex. They point the assistant to this documentation. On first use, the assistant asks whether your tests use Robot Framework or pytest and records the answer in `AGENTS.md`. | | |
| `.vscode/` | VS-Code specific settings for example environment. `ep-config.schema.json` gives code completion for writing `.ep-config.yaml` files. | | Adapt |
| `robot.toml` | [RobotCode](https://robotcode.io/latest/) configuration for the example environment. Defines the `ep_url`, `ep_app_dir` and `ep_binary_dir` variables and the profiles for [DUT debugging](DUT-Debugging.md). | | Adapt (Robot Framework only) |
| `pyproject.toml` | pytest and pyright configuration for the example environment. Lists the `.ep-config.zip` files Python imports, see [Importing ep-configs in Python](#importing-ep-configs-in-python). | | Adapt (pytest only) |
| `.env.example` | Template for a `.env` file that defines the [environment variables](SoftwarePackage#use-software-package-to-write-and-run-tests) of the pytest examples. VS Code reads `.env`. | | Adapt (pytest only) |
| `CMakeLists.txt`, `cmake/`, `CMakePresets.json`, `ep_build_info.yaml.in` | CMake infrastructure for example environment. Serves as a reference point for adaption. `BuildHelper.cmake` wires runtime, BSP and apps into your build. | Adapt | |
| `LICENSE`, `NOTICE` | The software package is licensed under the Apache License 2.0. `third_party/` keeps the licences of its projects. | | |

_Mandatory_ items should not be modified by the user. _Adapt_ items are intended to be changed for project specific needs.

## Updating the software package
Every ExecutionPlatform release has a release `v<version>` in the GitHub repository [embeff/embeff-ep-software-package](https://github.com/embeff/embeff-ep-software-package), for example `v9.8.0`. Each release is a single commit with the complete software package, so `git diff v9.7.0 v9.8.0` shows everything that changed between two releases.

If you keep a copy of the software package in your own repository, you can take over a new release with git instead of extracting a zip over the old copy. Alternatively, download the zip of the release from GitHub or the Customer Portal; both contain the same files.

After an update, run `pip install -r requirements.txt` again: it installs the embeff-ep version of the new software package.

### Installing embeff-ep without the software package
Projects that only run tests, for example a CI job, need embeff-ep but not the software package. Install the version of your ExecutionPlatform directly:
```
pip install "embeff-ep[robot,pytest] @ git+https://github.com/embeff/embeff-ep@v<version>"
```
Each release of [embeff/embeff-ep](https://github.com/embeff/embeff-ep/releases) also has the wheel attached. The extras are `robot`, `pytest` and `all`. Pin the version in your own `requirements.txt` so that every machine runs the same one.

## Apps
Apps are designed for a specific use case and define, how the test sequence interacts with DUT code.
Apps are independent of Endpoints defined in the [configuration](EP_Config.md).

They consist of DUT code and a `.resource` file to access the functionality from Robot Framework.

| App | Use case | CMake target | Robot Framework resource |
| --- | --- | --- | --- |
| Generic | Default app for most tests. Call any function you registered with `ep_register` from your test and read back standard output. All other apps inherit this functionality.  | `ep_app_generic` | None.  All keywords integrated in `EPLibrary` |
| Benchmark | Runtime measurement of a function on the DUT. `ep_bench_invoke` toggles the BSP reference pin around the call, the ExecutionPlatform measures the pulse and reports arguments and return value. C++ only. | `ep_app_bench` | `app/bench/bench.resource` |
| Unity | On-target unit testing with the Unity framework. Your `run_tests()` is executed on the DUT; test output and the number of failed tests are returned to the test sequence. Builds Unity from `third_party/Unity`. | `ep_app_unity` | `app/unity/unity.resource` |
| Gtest | On-target unit testing with GoogleTest and GoogleMock. `RUN_ALL_TESTS()` is executed on the DUT; test output and pass/fail state are returned to the test sequence. C++ only. Builds GoogleTest from `third_party/googletest`. | `ep_app_gtest` | `app/gtest/gtest.resource` |

Not every app is built for every DUT (flash limitations) or toolchain (C++ language support). The example CMake infrastructure limits the build to the apps listed in the BSPs `EP_BSP_APPS_TESTED` property.

## Use software package to compile test binaries
Aside from rare greenfield projects, you will already have a project structure that produces at least a single binary for your code.
This subsection explains what you need to do to extend this structure to produce binaries suitable for testing.

:information_source: A binary suitable for testing means that the [Code Interface](CodeInterface.md) can be used for communication between test and code. This is required for all apps listed above. In [system test mode](EP_Config.md#configurations) you use the normal firmware and no special build as described here is required.

The following instruction explain how to use our CMake targets and how to mix that with your environment. We assume that you followed [Getting Started](GettingStarted.md) and thus have a running example environment.


1. You should see the structure listed above, plus a `bsp` folder.
1. Add a new top-level folder `user` and add there two files.
    `test.c`
    ```
    #include <ep/core.h>

    int ep_app_main() {
        return ep_process_loop();
    }
    ```

    `CMakeLists.txt`
    ```
    add_executable(mytest test.c)
    target_link_libraries(mytest    PUBLIC  ep_app_generic)
    ```
1. In the top-level `CMakeLists.txt` add a `add_subdirectory(user)` to include the new file in the build process.
1. Start CMake configuration and build all targets.
1. The build should succeed. Depending on your compiler and settings, it might not produce a `.bin` or `.hex` automatically. The ExecutionPlatform `DUT Flash Firmware` needs one of these extensions.
    - You may use (or adapt) our helper in `cmake/BuildHelper` to convert an `.elf` output to `.bin` and `.hex`. Append `CMakeLists.txt` with the following lines
        ```
        include(../cmake/BuildHelper.cmake)
        ep_add_bin_output(mytest)
        ```

You now have a `.bin` and a `.hex` file that can be used in a robot test sequence `DUT Flash Firmware`.
The typical next steps are

- Adapt `user/CMakeLists.txt` so that the executable links your own sources and libraries.
- Add glue code in `user/test.c` that calls your functions under test and use `ep_register` to make this glue code callable from the test.

The instructions above used the `Generic` app. The steps are similar for all other apps

- link against your desired app (instead of `ep_app_generic`)
- change `test.c` to match your app use case. The `gtest` and `unity` apps define `ep_app_main` themselves, so remove it from `test.c`: `unity` needs your `int run_tests(void)`, `gtest` only your `TEST()`s in a C++ file such as `test.cpp` (see [Unit tests](Usecase/UnitTest.md)). See our examples to get a working starting point.

### I do not use CMake. What to do?
The ExecutionPlatform is in no way dependent on CMake. In fact it is entirely independent from how you create your DUT binaries.
Our examples use CMake because it is the de-facto industry standard.

If you use custom IDEs, Makefiles or other setups we recommend to that you use our CMake setup as a blue print to implement a working build for your setup. Start with the `Generic` app.

- Open `app/generic/CMakeLists.txt`
- Review `ep_app_create` in `cmake/BuildHelper.cmake` to understand how `ep_app_generic` is created and migrate this to your build system.

### Add your own BSP
The software package relies on BSP functions to establish the connection between DUT and ExecutionPlatform via UART.

For DUTs ordered at embeff, we ship a sample BSP that you can use until you are familiar with the ExecutionPlatform basic usage.
The instructions above assumed that you used this sample BSP.

The next step is to integrate your custom BSP. It is typically the same set of low level code and functions that are used for the production build of your firmware.

A conforming BSP:
- Must initialize the DUT in a typical way (clock tree, interrupts, ...). Typically by a combination of assembly and C-Code.
- Must call `main` at the end of the startup routine.
- Must implement all functions listed and documented in `src/include/ep/bsp.h`.
- Must redirect standard output (e.g. printf) to the EP. For example the ARM GCC stdout can be redirected by linking this function:
    ```cpp
    #include <stdio.h>
    #include <ep/app_data_splitter.h>

    size_t _write(int file, void const *ptr, size_t len) {
        (void) file;
        ep_internal_resp_splitted(EP_RESP_STDOUT, ptr, len);
        return len;
    }
    ```
- The CMake setup in our software package automatically defers software capabilities from the BSP through CMake properties. If you use a custom BSP and want to retain that functionality, you must define a property `EP_BSP_APPS_TESTED` for your BSP library. It needs to include a list of apps that should be supported (and compiled) when using that BSP. If your BSP CMake target is named `target_bsp` and should support all apps, you can add this line to your `CMakeLists.txt`:

    ```
    set_target_properties(target_bsp PROPERTIES EP_BSP_APPS_TESTED  "generic;bench;gtest;unity")
    ```

## Use Software Package to write and run Tests

<!-- tabs:start -->

#### **Robot Framework**

The Robot Framework library `EPLibrary` of the embeff-ep package gives your tests access to the ExecutionPlatform functionality. Robot Framework finds it in the Python environment you installed embeff-ep into.

If you have not set up your EP for Robot Framework yet, start with [Getting Started](GettingStarted.md): it connects the EP to your network, downloads the software package and the ep-configs, and prepares Robot Framework in VS Code. The [Robot Framework](TestFrontend/RobotFramework.md) chapter explains how tests call the endpoint functions.

The `EPLibrary` library expects 2 arguments:
1. An url consisting of user name, password and ip/hostname pointing to the ExecutionPlatform to use.
2. The path of your chosen ep-config. The library then provides keywords depending on the endpoints in this configuration.

This section describes what you need to configure to run Robot tests that use this library.

Our examples use the following standard to include the library:

```
*** Settings ***
Library           EPLibrary   ${ep_url}    ${ep_binary_dir}/gpio.ep-config.zip
```

`${ep_app_dir}`, `${ep_url}` and `${ep_binary_dir}` are RobotFramework variables that are used for convenience in our example environment. You can redefine or rename them as you like.

- `ep_app_dir`: The full path of the `app` folder, for the `Resource` files of the apps (eg. `${ep_app_dir}/cmake.resource`). Relatives paths are not supported.
- `ep_url`: URL in the form `user:password@address`. Address can be the IP or hostname of your ExecutionPlatform. Admins manage access in the [User Management](Service#user-management).
- `ep_binary_dir`: The path where both build results and configurations are stored. Path can be absolute or relative. When using this approach, you need to copy the required `.ep-config.zip` files to your binary/build directory.

You can rename or extend these variables as you like. They are only used inside test sequences, no other parts of the software package depend on them.

You may also hard-code your required settings. `${CURDIR}` is a built-in variable from Robot Framework that contains the absolute path to the directory of the current file.

```
*** Settings ***
Library           EPLibrary   user:password@10.0.0.1    ${CURDIR}/../../build/gpio.ep-config.zip
```

When you use variables, you can define them

- in `robot.toml`, the [RobotCode](https://robotcode.io/latest/) configuration file shipped in the root of the software package,
- in a shared `.resource` file,
- or supply them when actually running the test.

Our example environment defines them in `robot.toml`:

```toml
[variables]
ep_url = "user:password@10.0.0.120"
ep_app_dir.expr = "Path.cwd() / 'app'"
ep_binary_dir.expr = "Path.cwd() / 'build'"
```

Change `ep_url` to the address of your ExecutionPlatform. `ep_app_dir` and `ep_binary_dir` are `.expr` entries that RobotCode evaluates as Python. They resolve against the **current working directory**, so run your tests from the root of the software package. Replace them with absolute paths if your layout differs.

`robot.toml` also defines the profiles that enable [DUT debugging](DUT-Debugging.md).

:warning: `robot.toml` belongs to RobotCode, not to Robot Framework itself. It is read by the VS Code extension and by the `robotcode` command line, but **not** by the plain `robot` executable.

```
robotcode robot example/hello_world/hello_world.robot
```

Without RobotCode, supply the variables on the command line instead:

```
robot -v ep_url:"user:password@10.0.0.1" -v ep_binary_dir:"/Users/Michael/ep_example/build" -v ep_app_dir:"/Users/Michael/ep_example/app"    example/hello_world/hello_world.robot
```

Both executables can be given a single `robot` file, a list of files or a list of directories to execute.

:information_source: A typical **Continuous Integration** step will invoke one of the two commands above and thus run all required tests.

If you use Visual Studio Code and the [RobotCode extension](https://robotcode.io/latest/), you can start tests by click the run-icon next to each test case.

#### **pytest**

Each `.ep-config.zip` is also a Python package. The pytest plugin of the embeff-ep package connects these packages to the ExecutionPlatform. It registers itself when embeff-ep is installed; you do not need to load it.

If you have not set up your EP for pytest yet, start with [Getting Started](GettingStarted.md): it connects the EP to your network, downloads the software package and the ep-configs, and prepares pytest in VS Code. The [pytest](TestFrontend/pytest.md) chapter explains how tests call the endpoint functions.

A test module opens the EP in a fixture. `EP.fixture()` of the imported configuration creates it, and the handle it provides has the endpoints of this configuration. Annotate the tests with the same class for code completion; see [pytest](TestFrontend/pytest.md):

```python
from gpio.test import EP    # imports gpio.ep-config.zip

ep = EP.fixture()


def test_invoke_square_with_3_gets_9(ep: EP):
    assert ep.DUT.invoke("square", "3") == 9
```

The EP stays connected while the module runs and closes when it ends.

The handle gives access to the [Code Interface](CodeInterface.md) through `ep.DUT` and to each endpoint through the name of its instance in the ep-config, for example `ep.PA0.set_high()`. The [endpoint pages](Endpoints.md) show every function by its name, the [pytest](TestFrontend/pytest.md) chapter explains how to call it, and your editor's code completion lists them.

`EP.fixture()` takes these optional arguments. Each falls back to an environment variable when omitted:

| Argument | Environment variable | Description |
| --- | --- | --- |
| `url` | `EP_URL` | URL in the form `user:password@address`. Address can be the IP or hostname of your ExecutionPlatform. Admins manage access in the [User Management](Service#user-management). Required. |
| `debugging` | `EP_DEBUGGING` | `Off`, `External` or `VSCode`. See [DUT debugging](DUT-Debugging.md). Defaults to `Off`. |
| `timeout_s` | `EP_TIMEOUT_S` | How long to wait for a free EP, in seconds. Waits without a limit by default. |
| `tags` | | Narrows which EP a central management node may assign. |

You may also hard-code the url:

```python
from gpio.test import EP
ep = EP.fixture("user:password@10.0.0.1")
```

Our examples additionally read `EP_BINARY_DIR`, the path where build results are stored. It is only a convention of the examples.

For a test module to run, pytest must find the configurations it imports: list each in `pyproject.toml`, see [Importing ep-configs in Python](#importing-ep-configs-in-python).

In VS Code, copy `.env.example` to `.env` and adapt it. VS Code reads `.env` and lists the tests in its Testing view.

On the command line, `.env` is not read. Set the environment variables in your shell and run from the root of the software package:

Windows (PowerShell):
```
$env:EP_URL        = "user-id:password@ep-0000-0000"
$env:EP_BINARY_DIR = "build"
pytest example/hello_world/test_hello_world.py
```

Linux and macOS:
```
export EP_URL=user-id:password@ep-0000-0000
export EP_BINARY_DIR=build
pytest example/hello_world/test_hello_world.py
```

These commands set the variables for the current shell only.

pytest can be given a single file, a list of files or a list of directories to execute.

:information_source: A typical **Continuous Integration** step sets the environment variables and invokes pytest. `--junitxml=<file>` writes a report that most CI servers can display.

<!-- tabs:end -->

## Importing ep-configs in Python

Each `.ep-config.zip` configuration is also a Python package. The package is used by pytest tests to import that configuration (`from gpio.test import EP`). The package is also used by [System](Endpoints/System.md) models to get code completion for the endpoints of that configuration. `pyproject.toml` in the root of the software package tells pytest and the code completion where these packages are. It lists every `.ep-config.zip` twice, with paths relative to the folder of `pyproject.toml`:

```toml
[tool.pytest]
pythonpath = ["app", "build/gpio.ep-config.zip"]

[tool.pyright]
extraPaths = ["app", "build/gpio.ep-config.zip"]
```

- `pythonpath` in `[tool.pytest]` lets pytest import the configurations. `app` is listed for `cmake_helper`. A configuration missing here fails the test module with `No module named '<name>'`. Robot Framework and System models do not need this list.
- `extraPaths` in `[tool.pyright]` gives code completion in VS Code (Pylance) and in any editor using pyright. While this section exists, Pylance ignores the VS Code setting `python.analysis.extraPaths`.

When you use another configuration, add it to both lists.
