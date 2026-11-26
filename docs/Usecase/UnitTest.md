# Unit tests
Running your unit tests on the actual target hardware has multiple advantages. Only then you use the same tools and libraries for firmware and tests.
Tests have full access to all peripherals.

The ExecutionPlatform has built-in support for unit testing. It supports Google Test and Unity. There exists an app for both frameworks. Each app folder (`app/gtest`, `app/unity`) also provides helpers to run your tests on the DUT and read back the results: a `.resource` file for Robot Framework and a Python module for pytest. To get started take a look at the `example/misc` folder.

Both apps run the tests with `invoke` and read the results with `get_output` of the [Code Interface](../CodeInterface.md), so the ep-config of your test suite must contain the `System` block (see [Configurations](../EP_Config.md#configurations)).

# gtest
Adapts the [Google Test framework](https://github.com/google/googletest) so that tests can run without modification on-target on the ExecutionPlatform.

- Google Test and Google Mock are supported.
- The app is based on the [official repository release 1-11.0](https://github.com/google/googletest/releases/tag/release-1.11.0)
- The official version is not changed in any way. You can change the version by replacing the contents of ```third_party/googletest ``` directory.
- You must not define a ```main``` or ```ep_app_main``` function. The EP runtime provides ```main``` and the app provides ```ep_app_main```, which registers your tests as ```run_tests```.
- Link your executable against ```ep_app_gtest```.
- Do not link against ```gtest```, ```gtest_main``` or ```gmock```. ```gtest``` and ```gmock``` are added automatically.
- In case the binary is getting to large for the limited flash available, split your test files into different test projects.

To run gtest apps from your tests:

<!-- tabs:start -->

#### **Robot Framework**

1. Add `Resource   ${ep_app_dir}/gtest/gtest.resource`
1. Add `DUT Flash Firmware <path_to_binary>.bin`
1. Add `Run Gtest Tests` or `Run Gtest Tests And Compare Output`

See `example/misc/gtest/test_average_gtest.robot` for more information.

#### **pytest**

1. Add `from gtest.gtest import run_gtest_tests`
1. Call `ep.DUT.flash_firmware("<path_to_binary>.bin")`
1. Call `run_gtest_tests(ep)` or `run_gtest_tests_and_compare_output(ep, expected_output)`

See `example/misc/gtest/test_average_gtest.py` for more information.

<!-- tabs:end -->

# unity
Adapts the [Unity test framework ](http://www.throwtheswitch.org/unity) so that tests can run without modification on-target on the ExecutionPlatform.

- The app is based on v2.5.2 of unity.
- The official version is not changed in any way. You can change the version by replacing the contents of ```third_party/Unity```.
- The ```app/unity/include``` directory contains the ```unity_config.h```. This is the default configuration except that the outputs are redirected to be captured by the ExecutionPlatform.
- You must define a ```int run_tests()```function that calls ```UNITY_BEGIN()```, followed by all desired ```RUN_TEST``` calls and returning ```UNITY_END()```.
- You must not define a ```main``` or ```ep_app_main``` function. The EP runtime provides ```main``` and the app provides ```ep_app_main```, which registers your ```run_tests``` function.
- Link your executable against ```ep_app_unity```.
- Do not link against ```unity```. This dependency is added automatically.
- In case the binary is getting to large for the limited flash available, split your test files into different test projects.

To run unity apps from your tests:

<!-- tabs:start -->

#### **Robot Framework**

1. Add `Resource   ${ep_app_dir}/unity/unity.resource`
1. Add `DUT Flash Firmware <path_to_binary>.bin`
1. Add `Run Unity Tests` or `Run Unity Tests And Compare Output`

#### **pytest**

1. Add `from unity.unity import run_unity_tests`
1. Call `ep.DUT.flash_firmware("<path_to_binary>.bin")`
1. Call `run_unity_tests(ep)` or `run_unity_tests_and_compare_output(ep, expected_output)`

<!-- tabs:end -->

## Example
This example is located in `example/misc/unity`

```cpp
#include <unity.h>

int8_t AverageThreeBytes(int8_t a, int8_t b, int8_t c)
{
    return (int8_t)(((int16_t)a + (int16_t)b + (int16_t)c) / 3);
}

void test_AverageThreeBytes_should_AverageMidRangeValues(void)
{
    TEST_ASSERT_EQUAL_HEX8(40, AverageThreeBytes(30, 40, 50));
    TEST_ASSERT_EQUAL_HEX8(40, AverageThreeBytes(10, 70, 40));
    TEST_ASSERT_EQUAL_HEX8(33, AverageThreeBytes(33, 33, 33));
}

void setUp() { }
void tearDown() { }

int run_tests() {
    UNITY_BEGIN();
    RUN_TEST(test_AverageThreeBytes_should_AverageMidRangeValues);
    int testFailureCount = UNITY_END();
    return testFailureCount;
}
```

```cmake
ep_app_example( APP ep_app_unity
                NAME test_average_unity
                SOURCES test_average_unity.c)
```

<!-- tabs:start -->

#### **Robot Framework**

```robot
*** Settings ***
Library        EPLibrary   ${ep_url}    ${ep_binary_dir}/gpio.ep-config.zip
Resource       ${ep_app_dir}/unity/unity.resource

*** Test Cases ***
Run example test case and print results
    DUT Flash Firmware    ${ep_binary_dir}/example/test_average_unity.hex
    ${output} =     Run Unity Tests     expectedFailures=0
    Log To Console    ${output}
```

#### **pytest**

```python
import os

from unity.unity import run_unity_tests
from gpio.test import EP    # imports gpio.ep-config.zip

BINARY_DIR = os.environ["EP_BINARY_DIR"]


ep = EP.fixture()


def test_run_example_test_case_and_print_results(ep: EP):
    ep.DUT.flash_firmware(f"{BINARY_DIR}/example/test_average_unity.hex")
    output = run_unity_tests(ep, expected_failures=0)
    print(output)
```

<!-- tabs:end -->
