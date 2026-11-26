# Copyright (C) 2022 embeff GmbH
# SPDX-License-Identifier: Apache-2.0
# This file is part of the ExecutionPlatform distribution. More information at https://embeff.com

*** Settings ***
Library           EPLibrary   ${ep_url}    ${ep_binary_dir}/gpio.ep-config.zip
Suite Setup       Suite Setup
Test Tags         dut-independent
Resource          ${ep_app_dir}/cmake.resource
Documentation     Demonstrate Code Interface usage. 
...               DUT Flash Firmware    -     Programs a given binary to the microcontroller (DUT).
...               DUT Invoke            -     Call a given function at test runtime on the DUT.
...               DUT Get Output        -     Get everything the firmware wrote to standard output.
...                
...               For more details see https://ep-docs.embeff.com/#/CodeInterface

*** Keywords ***
Suite Setup
    CMake build           ${ep_binary_dir}    example_hello_world
    DUT Flash Firmware    ${ep_binary_dir}/example/example_hello_world.hex

*** Test Cases ***
Invoke Square With 3 Gets 9
    ${res} =     Dut Invoke  square  3
    Should Be Equal    ${9}    ${res}

Invoke Square With -5 Gets 25
    ${res} =     Dut Invoke  square  -5
    Should Be Equal    ${25}    ${res}

Call Function That Prints MCU-related Output
    Dut Invoke    print_registers
    ${out} =     DUT Get Output
    # out is a list of all lines print. Convert to string.
    ${outString} =    Catenate    SEPARATOR=${\n}    @{out}
    Log To Console    ${\n} ${outString}
