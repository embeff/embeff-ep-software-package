# Copyright (C) 2026 embeff GmbH
# SPDX-License-Identifier: Apache-2.0
# This file is part of the ExecutionPlatform distribution. More information at https://embeff.com

*** Settings ***
Library        EPLibrary   ${ep_url}    ${ep_binary_dir}/gpio.ep-config.zip
Library        Collections
Test Tags      app-gtest
Resource       ${ep_app_dir}/gtest/gtest.resource
Resource       ${ep_app_dir}/cmake.resource

*** Keywords ***
Build and Flash
    [Arguments]    ${target}
    CMake build    ${ep_binary_dir}    ${target}
    DUT Flash Firmware    ${ep_binary_dir}/example/${target}.hex

*** Test Cases ***
Run example testcase and print results
    Build and Flash    test_average_gtest
    ${output} =     Run Gtest Tests     expectFailures=0
    Log To Console    ${\n}${output}
