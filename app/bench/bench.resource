# Copyright (C) 2022 embeff GmbH
# SPDX-License-Identifier: Apache-2.0
# This file is part of the ExecutionPlatform distribution. More information at https://embeff.com

** Settings ***
Library    Collections

*** Keywords ***
Do Benchmark    
    [Documentation]    Measures the runtime of a ep_bench_invoke call on the DUT.
    ...
    ...                \${function} must be a registered method on the DUT that calls ep_bench_invoke.
    ...
    ...                \${args} is a string that is supplied to the \${function}.
    ...                The string is intended to be parsed on the DUT and applied
    ...                as arguments to the benchmarked function.

    [Arguments]    ${function}    ${args}=${EMPTY}
    ${retCode} =    Dut Invoke    ep_print_reference_pin
    Should Be Equal    ${retCode}    ${0}    Benchmark could not determine reference pin name. 

    ${name_out} =    Dut Get Output
    Length Should Be    ${name_out}    1    DUT output for benchmark is corrupted. At this point it should contain just the reference pin name.
    ${ep_reference_pin} =    Set Variable     ${name_out}[0]

    ${id} =     GPIO Measure Between Rising And Falling Edge    ${ep_reference_pin}    ${ep_reference_pin}
    ${retCode} =     DUT Invoke    ${function}    ${args}
    Should Be Equal    ${retCode}    ${0}    Benchmark invoke did not complete sucessfully.
    ${actualTime_ns} =     GPIO Get Measurement Result  ${id}    ReturnType=ns

    ${out} =     DUT Get Output
    Length Should Be    ${out}    1    DUT output for benchmark is corrupted. It should contain only argument and return values.
    ${json}=    evaluate    json.loads('''${out}[0]''')    json
    ${retVal}=    Get From Dictionary    ${json}    ret

    RETURN    ${actualTime_ns}    ${retVal}