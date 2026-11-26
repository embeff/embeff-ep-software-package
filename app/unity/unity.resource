# Copyright (C) 2022 embeff GmbH
# SPDX-License-Identifier: Apache-2.0
# This file is part of the ExecutionPlatform distribution. More information at https://embeff.com

*** Keywords ***
Run Unity Tests
    [Arguments]    ${expectedFailures}=0
    ${retCode} =     DUT Invoke    run_tests
    ${actualList} =  DUT Get Output
    ${actualOutput} =    Catenate    SEPARATOR=${\n}    @{actualList}
    Should Be Equal As Integers    ${expectedFailures}    ${retCode}    Expected ${expectedFailures} tests to fail but actual count is ${retCode}. Unit test output:\n${actualOutput}
    RETURN    ${actualOutput}

Run Unity Tests And Compare Output
    [Arguments]    ${expectedOutput}    ${expectedFailures}=0
    ${actualOutput} =     Run Unity Tests    expectedFailures=${expectedFailures}
    Should Be Equal As Strings        ${expectedOutput}    ${actualOutput}    Test output does not match expected output.    collapse_spaces=True
