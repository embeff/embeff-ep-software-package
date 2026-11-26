# Copyright (C) 2022 embeff GmbH
# SPDX-License-Identifier: Apache-2.0
# This file is part of the ExecutionPlatform distribution. More information at https://embeff.com

*** Settings ***
Library        EPLibrary   ${ep_url}    ${ep_binary_dir}/gpio.ep-config.zip
Library        Collections
Test Tags      app-unity
Resource       ${ep_app_dir}/unity/unity.resource
Resource       ${ep_app_dir}/cmake.resource

*** Keywords ***
Build and Flash
    [Arguments]    ${target}
    CMake build    ${ep_binary_dir}    ${target}
    DUT Flash Firmware    ${ep_binary_dir}/example/${target}.hex

*** Test Cases ***
Run example test case and print results
    Build and Flash    test_average_unity
    ${output} =     Run Unity Tests     expectedFailures=0
    Log To Console    ${\n}${output}
