# Coverage

You can instruct the compiler to generate coverage information and read/process this data with the ExecutionPlatform. This functionality can be used in combination with any app (Unit testing, benchmarking, hardware/software integration testing).

The documentation and explanations here are for gcc-based compilers and relate to their gcov library.
The [official gcov documentation](https://gcc.gnu.org/onlinedocs/gcc/Gcov-Intro.html) contains a concise introduction in the concepts and workings of code coverage in gcc.

To understand the multiple-step process of generating readable coverage, consider the following diagram:
![Steps involved in getting coverage information](Coverage.drawio.png)

:information_source: Open the `gcov` example from our [live demo](https://embeff.com/ep-demo-2/) for a practical demonstration and access to the code files mentioned below.

You need to perform the following steps once to analyzer code coverage for your tests:
1. Instruct your compiler to generate coverage for the relevant source files.
    - Link your program with `--coverage`.
    - Compile your program with `--fprofile-info-section`. Note that this flag is only available for gcc versions starting with **gcc 11.1**. For older versions, see below.
    - Compile the source files you want to analyze with `--coverage`. Use a minimum of source files to limit the amount of coverage data.
2. Add source code that streams the coverage data. You may copy-paste our `ep_gcov.c` from the gcov demo.

To get coverage for a specific scenario:

1. Run your binary on the ExecutionPlatform so that the code of interest is executed.
    - Call `flash_firmware` of the [Code Interface](../CodeInterface.md).
    - Call `invoke` to call the part of the code you want to analyze.

2. Call `get_coverage` and process the data so that `.gcda` files are generated.
    - Store the results from `get_coverage` in a file.
    - Process the file with a tool like `arm-none-eabi-gcov-tool` to generate the `.gcda` files.
    - You may copy-paste our `GCOV.py` library from our demo that implements these steps.
3. Call a tool of your choice to analyze the coverage data.
    - For example: Invoke `gcovr` from the root of your source directory to generate human or machine-readable results: `gcovr --gcov-executable arm-none-eabi-gcov`

The calls of steps 1 and 2 in your test:

<!-- tabs:start -->

#### **Robot Framework**

```robot
DUT Flash Firmware    ${ep_binary_dir}/my_firmware.hex
DUT Invoke    my_function
${coverage} =    DUT Get Coverage
```

#### **pytest**

```python
ep.DUT.flash_firmware(f"{BINARY_DIR}/my_firmware.hex")
ep.DUT.invoke("my_function")
coverage = ep.DUT.get_coverage()    # bytes
with open("coverage.bin", "wb") as f:
    f.write(coverage)
```

<!-- tabs:end -->

## gcc versions prior to 11.1
The `-fprofile-info-section` flag described in the example above was introduced only with gcc 11.1.
Older gcc versions can also generate the required coverage information.
However this involves more work for embedded targets (more boilerplate code on both target and host).

Take a look at <a href="https://github.com/nasa-jpl/embedded-gcov">embedded-gcov</a> if you want to implement on-target coverage for older gcc versions.

To get started, you can simply add the project source files and modify `gcov-printf.c` to stream the coverage data to the ExecutionPlatform:

```
#include <ep/app.h>
/* Include any files you need to get your write_bytes() function */
/* that does the actual serial output in your system. */
/*
 * INT32
 * write_bytes(INT32 fd,
 *       CHAR_T const *buf,
 *       UINT32 nbyte);
 * where
 * @param [in]     fd (which serial port)
 * @param [in,out] *buf (bytes to send out)
 * @param [in]     nbyte (number of bytes to send)
 *
 * @return -1 if error, otherwise returns byte count written to UART
 */
#define write_bytes(fd, buf, n)  ep_internal_resp(EP_RESP_COVERAGE_RAW, buf, n)
```
