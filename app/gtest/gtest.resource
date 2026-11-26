# Copyright (C) 2022 embeff GmbH
# SPDX-License-Identifier: Apache-2.0
# This file is part of the ExecutionPlatform distribution. More information at https://embeff.com

*** Keywords ***
Run Gtest Tests
    [Arguments]    ${expectFailures}=False
    ${retCode} =     DUT Invoke    run_tests
    ${actualList} =  DUT Get Output
    ${actualOutput} =    Catenate    SEPARATOR=${\n}    @{actualList}
    IF    ${expectFailures}
        Should Be True         ${retCode}    Expecting failures but gtest return code is 0 (indicating no failed tests). Unit test output:\n${actualOutput}
    ELSE
        Should Not Be True     ${retCode}    Expecting no failures but gtest return code is 1 (indicating any number of failures). Unit test output:\n${actualOutput}
    END
    RETURN    ${actualOutput}

Run Gtest Tests And Compare Output
    [Arguments]    ${expectedOutput}    ${expectFailures}=False
    ${actualOutput} =     Run Gtest Tests    expectFailures=${expectFailures}
    Log    ${expectedOutput}
    Log    ${actualOutput}
    Should Be Equal As Strings        ${expectedOutput}    ${actualOutput}    Test output does not match expected output.    collapse_spaces=True
