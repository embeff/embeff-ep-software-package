# Code Interface
With the The Code Interface you can control your DUT:
- Change the firmware.
- Turn on or off the supply voltage.
- Call functions.
- Read standard output.
- Read coverage data.

When a test suite starts and when it ends, the DUT power is switched off. Between the tests of a suite the DUT stays powered and is not reset. A typical test suite has a single `flash_firmware` at the beginning of its lifecycle that automatically switches on the power.

:warning: You should never keep the DUT in power-off state and run tests with Endpoints. A typical MCU uses a protection circuit around its pins. If it is not powered, this protection may cause a short-circuit between the Endpoint and GND. The ExecutionPlatform is protected against such short-circuits, however this may lead to unexpected pin levels and test results.

## Functions

The functions are shown by their name. [Robot Framework](TestFrontend/RobotFramework.md) and [pytest](TestFrontend/pytest.md) explain how to call them.

All functions on this page are called on the static endpoint `DUT`, for example `DUT Invoke` in Robot Framework or `ep.DUT.invoke(method_name, data)` in pytest. `DUT` needs no entry in the ep-config. See [Static endpoints](Endpoints.md#static-endpoints).

`flash_firmware(file)`
- Programs the given firmware to the DUT. The firmware must be in binary `.bin` format or Intel HEX `.hex` format.
- Expects a path to a firmware.
- If a test suite uses the same firmware for all tests, call `flash_firmware` once at the beginning of its lifecycle (See [Robot Lifecycle](TestFrontend/RobotFramework.md#suites-and-their-lifecycle) or [pytest Lifecycle](TestFrontend/pytest.md#suites-and-their-lifecycle)).
- If each test uses a different firmware, you can call `flash_firmware` directly from the test case.
- The DUT must be powered off when `flash_firmware` is called, otherwise the keyword raises an error. To flash again within a suite, call `power_off` before.
- The keyword will internally (1) enable the power supply (2) flash the DUT (3) cold-start the DUT.
- In integration test mode (System block configured): waits until `ep_process_loop` is reached so that DUT is ready for processing. In system test mode (no System block): skips the wait.
- :information_source: Minimize the use of flashing, as it increases test time.
- :information_source: After flashing a firmware the DUT is automatically powered on. If you need the same firmware for all tests but you require a cold-start on each test, then call `power_off` and `power_on` before each test: in Robot Framework in the [Test Setup](https://robotframework.org/robotframework/latest/RobotFrameworkUserGuide.html#test-setup-and-teardown), in pytest in a function-scoped [fixture](https://docs.pytest.org/en/stable/how-to/fixtures.html).

`power_on()`
- Enables the power supply for the DUT.
- In integration test mode (System block configured): waits until `ep_process_loop` is reached so that DUT is ready for processing. In system test mode (no System block): skips the wait.
- Raises an error if the power is already turned on.
- Raises an error if `ep_process_loop` is not reached within 2s after power is enabled (integration test mode only).
- Note: This keywords has no effect when Remote Debugging is active.

`power_off()`
- Disables the power supply for the DUT and waits for full discharge.
- Has no effect when the power is already disabled.
- :information_source: This keywords has no effect when Remote Debugging is active.
- :information_source: This function is automatically called when a test suite starts and when it ends.

`invoke(method_name, data: ByteData)`
- Invoke functions that were registered by `ep_register` on the DUTs current binary.
- Invokes the function and waits until it is executed.
- The optional `data` is interpreted as [ByteData](Endpoints#byte-data).
- Returns the functions result as an Integer.
- The total length of `method_name` and `data` can be up to 254 bytes.

`get_output()`
- Collects the standard output from the DUT.
- The BSP redirects all standard output (for example `printf` and `std::cout`) to the ExecutionPlatform (see [Add your own BSP](SoftwarePackage.md#add-your-own-bsp)).
- `get_output` reads all output and clears the buffer.
- The return value is a list of strings. Each line break adds a new element to this list.

`get_coverage()`
- Invokes function `ep_dump_coverage` on DUT and read back the raw binary dump.
- You may need to further process the results - depending on the type of data.
- See [Use Case - Coverage](Usecase/Coverage.md) for instructions how to use this keyword.


## Limitations of `ep_register()`

`ep_register(method_name, function)` registers `function` under `method_name`. A test calls it with `invoke` and the same `method_name`.

The `ep_register()` has specific constraints that must be followed to ensure correct operation. Violating any of the conditions below may lead to test abortion.

### 1. Method name must be unique
- Each method name can be registered only once.
- Registering the same method name again aborts with `Registered ID is duplicated`.

**Example:**
```c
ep_register("abc", &abc);   // OK
ep_register("abc", &cdf);   // Aborts (duplicate method name)
```

###  2. Method Name Length Must Not Exceed 254 Characters
- `invoke` can only call method names of 254 characters or fewer: `method_name` and `data` together can be up to 254 bytes.
- `ep_register()` aborts with `Registered ID length is too long` for method names longer than 255 characters. A method name longer than the total length limit (see 3., default 200 characters) aborts as well.

**Example:**
```c
char long_name[300] = "aaaaaaaa...";  // (299 characters)
ep_register(long_name, &abc);      // Aborts due to excessive length
```
**Valid usage:**
```c
char valid_name[10] = "aaaaaaaaa";   // (9 characters)
ep_register(valid_name, &abc);     // OK
```

### 3. Total Method Name Length Limit
- When registering multiple functions, the total combined length of all method names must not exceed a configurable limit. The terminating `\0` is not counted.
- **Default limit:** 200 characters.
- **User-defined limit:** You can override the default by defining `EP_REGISTER_MAX_TOTAL_ID_LEN` (see [Changing the limits](#changing-the-limits)).
- Exceeding the limit aborts with `ep_register exceeded Total ID Limit`.

**Example:**
```c
ep_register("gpio_high", &abc);           // 9 characters
ep_register("spi_transfer_byte", &def);   // 17 characters
...
// If total length exceeds 200 across registrations:
ep_register("very_long_name...", &xyz);   // Aborts

```

### 4. Registered Function Limit
- Only a limited number of functions can be registered using `ep_register()`.
- **Default limit:** 10 registered functions.
- **User-defined limit:** You can override the default by defining `EP_REGISTER_MAX_CALLS` (see [Changing the limits](#changing-the-limits)).
- Registering more functions aborts with `ep_register exceeded Callback Registration Limit`.

**Example:**
```c
// Without setting a custom limit
char method_name[16];
for (int i = 0; i < 11; i++) {
   snprintf(method_name, sizeof(method_name), "square%d", i);
   ep_register(method_name, &square);  // Works for the first 10 times, aborts on the 11th
}
```

### Changing the limits
`EP_REGISTER_MAX_TOTAL_ID_LEN` and `EP_REGISTER_MAX_CALLS` are defaults in `src/ep_runtime.c`. They size static buffers, so choose them according to the memory of your DUT. To change them, define them when compiling `src/ep_runtime.c`. With the CMake setup of the software package it is compiled as part of the BSP library `target_bsp`:

```cmake
target_compile_definitions(target_bsp PRIVATE EP_REGISTER_MAX_CALLS=20 EP_REGISTER_MAX_TOTAL_ID_LEN=400)
```
