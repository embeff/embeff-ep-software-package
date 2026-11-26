# DUT Debugging
Sometimes you may want to debug your DUT binary to understand in detail what is happening during a particular test situation. The ExecutionPlatform supports debugging through various mechanisms.

The usage and possibilities depend on whether your DUT is [supported by SEGGER J-Link](https://www.segger.com/supported-devices/jlink/).

Debugging is controlled by one setting that defaults to `Off`:

<!-- tabs:start -->

#### **Robot Framework**

Set the robot variable `ep_debugging` in the `robot.toml` of the software package, or activate one of the profiles the `robot.toml` from the [Software Package](SoftwarePackage.md#use-software-package-to-write-and-run-tests) defines for it: `with_dut_debugging` (sets `VSCode`) and `with_external_debugging` (sets `External`). On the command line, a profile is selected with `robotcode --profile with_dut_debugging robot ...`.

#### **pytest**

Set the environment variable `EP_DEBUGGING` in your `.env`, or pass `debugging` to `EP.fixture()`. See [Software Package](SoftwarePackage.md#use-software-package-to-write-and-run-tests).

```python
from gpio.test import EP
ep = EP.fixture(debugging="VSCode")
```

<!-- tabs:end -->

| Value | J-Link Support | No J-Link Support |
| ----           | ----        | ----       |
|  `Off` (Default)           | Debugging is turned off. | Debugging is turned off. |
| `External`     | ✅ Use internal debugger and external software.        | ✅ Use external debugger hardware and software. |
| `VSCode`     | ✅ Use internal debugger and VS Code debugger.        | ❌ Not supported. Can be added by customer. |

## External
When debugging is set to `External`, `flash_firmware` of the [Code Interface](CodeInterface.md) will
- pause the test execution
- ask you to flash a binary using a 3-rd party debugger. You must then connect your external debugger to the DUT use it to flash the binary. If the debugger halts the binaries execution, continue it.
- resume the test execution when it detects that the DUT is ready to receive commands.

When `flash_firmware` completes, you may add breakpoints or use any other debugger feature.

:information_source: Do not confuse this debugging on the DUT with the step-by-step execution of the test sequence in Robot Framework or pytest. Test sequence "debugging" is always available.

### DUT is supported in J-Link
This debugging is based on the [SEGGER J-Link Remote Server](https://www.segger.com/products/debug-probes/j-link/tools/j-link-remote-server/) in LAN mode.
You do not need to connect any external hardware, the internal ExecutionPlatform debugger is used.

This example uses [SEGGER Ozone](https://www.segger.com/products/development-tools/ozone-j-link-debugger/) as an external debugger. However the same steps apply in a similar way to any other Debugger that supports GDB Remote Server.

1.  Start Ozone and create a new project. Select your target from the list.

    ![Ozone target](img/ozone-1.png)

2.  Next you need to specify the connection. Choose debugging interface supported by your DUT as Target Interface.
    Choose `IP`as Host Interface. Next you need to enter the IP address of your ExecutionPlatform, the hostname is not supported here.

    ![Ozone connections](img/ozone-2.png)

3.  Choose an ELF file to proceed. Note that some compilers create such an ELF-file without an extension.
    Do not use the `.bin` file because debugger information is lost and you need to specify some settings manually. This example uses `test_average_unity` from `example/misc/unity` of our software package.

4. Skip the optional settings.

5.  Connect to the target and download the program. You can now debug your application.

    ![Ozone connected](img/ozone-3.png)


### No J-Link support for DUT

1. Connect your debugger to the DUT-specific debugging port on your DUT PCB. In the ExecutionPlatform Dev variant, the DUT PCB is always exposed. For the Standalone and Rack variants, you need to open the case by loosening the 4 screws at the back.
1. Connect your debugger to a PC.
1. Start the test sequence and use the connected debugger to flash the firmware when asked during `flash_firmware`.
1. Debug your code as you wish.
1. Make sure to disconnect your debugger from the ExecutionPlatform once your debug session is finished.

    :warning: ️Some debuggers block the programming interface while connected (even if not used) and it is not possible to program the device from within the ExecutionPlatform.

    We recommend to never let the external debugger physically connected to the ExecutionPlatform except while having debug sessions.

## VSCode
:information_source: This section only applies your DUT is [supported by SEGGER J-Link](https://www.segger.com/supported-devices/jlink/).

The `VSCode` mode for debugging uses the Visual Code Debugger and attaches it to the J-Link built into your ExecutionPlatform.
This makes it possible to control both the test execution and DUT debugging from a single application. You can even debug the test sequence (`.robot` or `.py`) and the DUT source files side-by-side simultaneously.

### Requirements
- Install the [Cortex-Debug](https://marketplace.visualstudio.com/items?itemName=marus25.cortex-debug) and [Remote Control](https://marketplace.visualstudio.com/items?itemName=eliostruyf.vscode-remote-control) extension.

- Install the [J-Link Software and Documentation Pack](https://www.segger.com/downloads/jlink/#J-LinkSoftwareAndDocumentationPack) and add it to your path.

    :warning: To minimize compatibility risks, make sure to use the same version that is used by the ExecutionPlatform remote server. We document this version for each [Release](/ReleaseNotes.md).

- Python Packages:
  - [websockets 14 or newer](https://pypi.org/project/websockets/)
    ```
    pip install "websockets>=14"
    ```

### Configuration
For Debugging to work the launch configuration `Debug on ExecutionPlatform` is needed. In our software package you will find `launch.json` inside `.vscode` directory. It will look similar to this configuration:

```json
{
    "cwd": "${workspaceFolder}",
    "executable": "automatically_generated",
    "name": "Debug on ExecutionPlatform",
    "request": "launch",
    "type": "cortex-debug",
    "device": "your mcu type",
    "runToEntryPoint": "ep_app_main",
    "showDevDebugOutput": "none",
    "servertype": "jlink",
    "ipAddress": "automatically_generated",
    "svdFile": "${workspaceFolder}/bsp/.../*.svd"
},
```
:warning: pure JSON is required for processing. Comments and other extensions are not allowed. You will get `Parsing launch.json error at line ...` error when you use unsupported JSON constructs.

### How to use it:
- Set debugging to `VSCode` as described at the top of this page: the robot variable `ep_debugging` or the `with_dut_debugging` profile in Robot Framework, `EP_DEBUGGING=VSCode` in pytest.
- Select the `Debug on ExecutionPlatform` launch config.
    ![Select launch config](img/DebugVSCode_1_Select.png)
- Run a single test or a test suite.
    - Once the test calls `flash_firmware`, the test will automatically update the launch config with the EP address and firmware file from your test. Please note that `flash_firmware` expects a binary file, but for debugging you need a ELF-file. Our software stack assumes that you have both `.bin` and `.elf` file in the same folder with the same base name.
    - The test will start the VS Code Debugger through the launch config. The debugger will flash the firmware and halt in the `ep_app_main` function.
    ![Debugger active. Test waits.](img/DebugVSCode_2_Active.png)
- Continue debugging. The test will resume automatically when it detects that the DUT is ready to receive commands.

### Troubleshooting

1. You stopped debugging, but the test sequence still waits for the ExecutionPlatform to respond.


    This can happen if you debug a function that was called by `invoke`. The test sequence will block until the DUT responds to the invoke. If you stop debugging accidentally or the DUT does not respond for other reasons, the test sequence will hang. To resolve this, click on the arrow right to the stop-symbol and choose _Disconnect_.

    ![How to disconnect when DUT does not respond](img/DebugVSCode_3_Disconnect.png)
