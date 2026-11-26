# Robot Framework
Robot Framework tests use the library `EPLibrary` from the Python package [embeff-ep](https://github.com/embeff/embeff-ep). It takes the address and credentials of your EP and an ep-config, and provides the functions of the [Code Interface](../CodeInterface.md) and of all endpoints in this configuration as keywords:

```robot
*** Settings ***
Library    EPLibrary    ${ep_url}    ${ep_binary_dir}/gpio.ep-config.zip
```

See [Getting Started](../GettingStarted.md) for how to set up your EP and your environment, and [Software Package](../SoftwarePackage.md#use-software-package-to-write-and-run-tests) for how to run the tests in your own project.

## Before you start
If you have not run Robot Framework tests on the EP yet, set up your environment as described in [Getting Started](../GettingStarted.md):
- [Requirements](../GettingStarted.md#requirements): run `pip install -r requirements.txt` in the root folder of the [Software Package](../SoftwarePackage.md). Its `requirements.txt` installs embeff-ep in the version of your EP, and `robotcode`.
- [VS Code configuration](../GettingStarted.md#vs-code-configuration): set `ep_url` in `robot.toml` and place the `.ep-config.zip` files in the `build` directory.
- [Run example in VS Code](../GettingStarted.md#run-example-in-vs-code): run the hello world example to check the setup.

## Suites and their Lifecycle
A suite is a logical group of tests that share one ep-config. Robot Framework creates suites automatically from `.robot` files. Suites are equivalent to pytest modules.

Robot Framework offers two lifecycle functions for suites:
- A keyword defined as `Suite Setup` in the Settings section is executed before running any of the suite's test cases or child test suites.
- A keyword defined as `Suite Teardown` in the Settings section is executed after all of the suite's test cases or child test suites.

There are similar settings for tests: `Test Setup` and `Test Teardown`

Find more information in the [Robot Framework User Guide](https://robotframework.org/robotframework/latest/RobotFrameworkUserGuide.html#suite-setup-and-teardown)



## Calling endpoint functions
The endpoint pages show each function by its name, for example `set_high()` on the [GPIO](../Endpoints/GPIO.md) page. In Robot Framework, a keyword is the name of the endpoint instance followed by the function name:

| Endpoint page | ep-config | Keyword |
| - | - | - |
| `set_high()` on the GPIO page | GPIO endpoint `PA0` | `PA0 Set High` |
| `send_frame(Id, Data: ByteData, ...)` on the CAN page | CAN endpoint `CAN1` | `CAN1 Send Frame` |
| `set_pins(high=[], ...)` on the GPIO page | none, `GPIO` is a [static endpoint](../Endpoints.md#static-endpoints) | `GPIO Set Pins` |

:information_source: Robot Framework examples write the function name with spaces and each word capitalized: `set_high()` becomes `Set High`, as keyword completion in VS Code shows it. Robot Framework ignores case, spaces and underscores in keyword names, so `PA0 set_high` calls the same keyword.

The functions of the other static endpoints are called the same way: `DUT Flash Firmware`, `SYSTEM Start Model`, `FMI Load Model`.

Arguments follow the keyword, separated by at least two spaces. Named arguments are written as `Name=value`. Values need no quotes: the endpoint page shows `Pause="0ms"`, the keyword takes `Pause=0ms`.

## Example: GPIO
The ep-config defines two GPIO endpoints: `BUTTON` on a pin the firmware reads as an input, and `LED` on a pin the firmware drives. The test only writes `BUTTON` and only reads `LED`, as the DUT would see a real button and a real LED. It checks that the firmware switches on the LED within 10 ms after the button is pressed:

```robot
*** Settings ***
Library    EPLibrary    ${ep_url}    ${ep_binary_dir}/button.ep-config.zip

*** Test Cases ***
LED turns on within 10ms after button press
    BUTTON Set High
    ${measurement} =    GPIO Measure Between Falling And Rising Edge    BUTTON    LED
    BUTTON Set Low    # the button pulls its pin low when pressed

    ${reaction_ms} =    GPIO Get Measurement Result    ${measurement}    ReturnType=ms
    Should Be True    ${reaction_ms} < 10
    ${level} =    LED Read
    Should Be Equal    ${level}    HIGH
```

`BUTTON` and `LED` are the instances from the ep-config. The measurement runs on the static `GPIO` endpoint, which takes the pin names as arguments.

## Unit tests and benchmarks
The `unity`, `gtest` and `bench` apps come with keywords that run unit tests on the DUT and measure the runtime of functions. Load them with `Resource    ${ep_app_dir}/unity/unity.resource`, `gtest/gtest.resource` or `bench/bench.resource` and call `Run Unity Tests`, `Run Gtest Tests` or `Do Benchmark`. See [Unit Tests](../Usecase/UnitTest.md) and [Benchmarks](../Usecase/Benchmark.md).

## Combine endpoint functions in user keywords
Each endpoint provides a minimal set of functions that you combine for your use case (see [Design philosophy](../Endpoints.md#design-philosophy)).
Robot Framework supports this approach natively, as it lets you easily create custom [user keywords](https://robotframework.org/robotframework/latest/RobotFrameworkUserGuide.html#creating-user-keywords) based on the predefined endpoint functions.

## Compare floating-point results
Endpoint functions return [physical values](../Endpoints.md#return-values) as floating-point numbers.

Robot Framework has a [specific way to convert floating-point strings to numbers](https://robotframework.org/robotframework/latest/libraries/BuiltIn.html#Should%20Be%20Equal%20As%20Numbers). If you want to compare a result to a very small value, make sure that you compare against floating point values (wrap numeric value in `${}`).

Comparison without converting from string to float always works
```robot
    ${time_s} =    Set Variable    ${0.000000970}   # 970 ns
    Should Be Equal         ${time_s}    ${970e-9}     # OK
    Should Not Be Equal     ${time_s}    ${971e-9}     # OK
```

If you want to use strings, use the `precision` parameter in keywords like `Should Be Equal As Numbers`. By default, Robot uses a precision of only `6`. This means for example that comparing nanosecond-results that are stored in seconds, will not work. Comparison with string-float-conversion do only work up to micro-prefix by default
```robot
    Should Be Equal As Numbers        ${time_s}    970e-9                   # OK
    # The default precision of 6 means, that 970e-9 is rounded to 1e-6 internally.
    Should Not Be Equal As Numbers    ${time_s}    971e-9                   # FAIL 1e-06 == 1e-06
```
By using the optional `precision` parameter, everything works as expected.
```robot
    Should Not Be Equal As Numbers    ${time_s}    971e-9  precision=9      # OK. Will work for nano-prefix.
```

## Handle byte data
Functions with a [_ByteData_](../Endpoints.md#byte-data) argument accept bytes, strings and lists. These are different types in Robot Framework.
This section explains how you best choose a type and how you can handle them.

All examples are using `DUT Invoke`. This is a keyword that accepts a _ByteData_ argument for its payload.

### Strings
Everything is by default a string in Robot Framework. You can use the `\x` prefix to denote a hexadecimal byte value within a string. Typically you will either use strings that are pure plain text, or you use strings that are pure hexadecimal.

```robot
    DUT Invoke  print_payload  Hey
    DUT Invoke  get_length     \xFF\x48\x65\x85\x79
```

Strings are easy to use. You can check their length and access single elements.
```robot
    ${plain} =    Set Variable    Hey
    ${hex} =      Set Variable    \xFF\x48\x65\x85\x79

    Length Should Be    ${plain}    3
    Length Should Be    ${hex}      5

    Should Be Equal    ${plain[1]}  e
    Should Be Equal    ${hex[2]}    \x65
```

As everything is a string in Robot, you can also very easily concatenate them.
```robot
    ${s1} =    Set Variable    \xAB\xCD
    ${s2} =    Set Variable    ${s1}\xEF
    Should Be Equal    ${s2}    \xAB\xCD\xEF
```

Drawbacks of using strings:
- Visual comparison for strings containing non-ascii hex values is difficult. Robot displays all characters, even if they do not have a readable representation.
- Strings can contain unicode characters. Unicode characters use more than a single byte. So the string length will not equal the byte length. Converting unicode strings to bytes will lead to an error. For this reason a ByteData argument that contains unicode characters will be rejected in the keyword.

### Bytes
Robot Framework supports a distinct bytes type. Bytes are created by converting them from other types, most notably from strings.
Visual comparison for bytes is well-supported. Robot Framework will display all non-readable characters with their hexadecimal value.

```robot
    ${bytes} =    Convert To Bytes    \xA1\xA2
    Length Should Be    ${bytes}    2
    Should Be Equal    ${bytes[1]}    ${0xA2}

    DUT Invoke  get_length     ${bytes}

```

If you want to compare bytes and strings, you should first convert the string to a bytes (`Convert to Bytes`) and then compare (for example `Should Be Equal`).


Drawbacks of using bytes:
- They are immutable. You can not modify them.
- They can not be concatenated in a user-friendly way.

### Lists
The Collections standard library provides support for lists in Robot Framework.
To use their functions, you need to include `Library  Collections` in the settings-section of your test sequence.
Lists can be used to store byte data and are very flexible.

```robot
    ${l} =    Create List    ${0xAB}    ${0xCD}
    Dut Invoke    get_length    ${l}
```

Lists offer the simplest way to modify or concatenate lists of bytes.
```robot
    ${l} =    Create List    ${0xAB}    ${0xCD}
    Append To List    ${l}    ${0xEF}
    Set List Value    ${l}    1    ${0x00}

    ${bytes} =            Convert To Bytes  ${l}
    ${bytesExpected} =    Convert To Bytes    \xAB\x00\xEF
    Should Be Equal    ${bytes}    ${bytesExpected}
```

If you want to compare lists and bytes, you should first convert the list to bytes (`Convert to Bytes`) and then compare (for example `Should Be Equal`).

Drawbacks of using lists:
- You must take care to only store values in each element that can be interpreted as a byte (ie, has the numerical representation of 0-255).
