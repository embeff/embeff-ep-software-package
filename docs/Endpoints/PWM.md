<!-- The keyword sections of this page are generated from the Python docstrings; see tools/docs/generate_docs.py -->
## PWM Endpoint
The PWM endpoint makes it possible to measure and evaluate pulse-width encoded signals.
A complimentary channel can be configured to include dead time measurements.

## Technicals
- Maximum number of configured endpoints: 10.
- Time resolution: 10ns.
- Measurement accuracy < 20ns.

## Configuration
This section lists the properties that can be configured for each `PWM` endpoint.

| Setting      | Description | Values |
| ----------- | ----------- | ------- |
| OUT  | Package pin on the DUT for the high side PWM output. | DUT-dependent |
| OUTn   | Optional DUT package pin for the low side PWM output. OUTn acts as a complimentary channel to OUT. | DUT-dependent |

### Example
```yaml
PWM:
  PWM0:
    OUT: 64
    OUTn: 63
```

## General behavior
_Active_ and _Default_ define two logical levels that each channel can have. Typically the _Default_ will be LOW and _Active_ will be HIGH but this can be reversed when `start_measuring` is called.

Each measurement starts with a change from the default level on OUT. It ends with the next change from the default level to active on OUT.
In both figures, 2 pulses are measured. The last pulse is started, but never finished with start of yet another pulse. Therefore only the first 2 pulses are taken into account by the observing functions described below.

![PWM started with Default=Low](PWM_DefaultLow.drawio.png)

![PWM started with Default=High](PWM_DefaultHigh.drawio.png)


### Complementary channel OUTn
If OUTn is set in the endpoint configuration, t<sub>d1</sub> and t<sub>d2</sub> are measured by continuously comparing the OUTn channel to the OUT channel.
All other functionality is unaffected.

:information_source: Measurements for duty cycle and frequency are always performed only on the OUT channel.

![Complementary mode with dead times](PWM_Complementary.drawio.png)

## Setup & Teardown
A running measurement is stopped after each test.

:information_source: `stop_measuring` must be called prior to any observing function.

## Test frontend

Setup, controlling and observing functions are shown by their name. [Robot Framework](../TestFrontend/RobotFramework.md) and [pytest](../TestFrontend/pytest.md) explain how to call them on an endpoint instance.

### Setup functions

The PWM endpoint has no setup functions.

### Controlling functions

<a id="start-measuring-keyword"></a>`start_measuring(DefaultState="Low")`
- The endpoint starts measuring.
- Resets all measurements.
- `DefaultState`: `Low` | `High`.
- Returns the _test time_ when the PWM endpoint started to measure.

<a id="stop-measuring-keyword"></a>`stop_measuring()`
- The endpoint stops measuring.
- Returns the _test time_ when the PWM endpoint stopped to measure.

### Observing functions

<a id="get-frequency-keyword"></a>`get_frequency(ReturnType="Hz", MaxAllowedDeviation="5%")`
- Returns the average frequency of all observed pulses.
- :warning: The average is calculated as the inverse of the observed period lengths. This means that a first pulse of period 1ms (1kHz) and a second pulse of period 2ms (500Hz) will yield an average frequency of 666Hz.
- The endpoint calculates the maximal deviation from the average across all observed pulses. If this deviation is larger than `MaxAllowedDeviation` in percent, a `DeviationError` is raised.
- The frequency is returned as a float in the given `ReturnType` unit.

<a id="get-duty-cycle-keyword"></a>`get_duty_cycle(MaxAllowedDeviation="5%")`
- Returns the average duty cycle of all observed pulses as a number between 0 and 1.
- The endpoint calculates the maximal deviation from the average across all observed pulses. If this deviation is larger than `MaxAllowedDeviation` in percent, a `DeviationError` is raised.

<a id="get-frequency-and-deviations-keyword"></a>`get_frequency_and_deviations(ReturnType="Hz")`
- Returns 3 values as a float in the given `ReturnType` unit:
    - `Avg`: Average frequency of all observed pulses.
    - `Min`: Minimal frequency of all observed pulses.
    - `Max`: Maximal frequency of all observed pulses.

<a id="get-duty-cycle-and-deviations-keyword"></a>`get_duty_cycle_and_deviations()`
- Returns 3 values. All are numbers between 0 and 1:
    - `Avg`: Average duty cycle of all observed pulses. Calculated by dividing the average active time to the average period.
    - `Min`: Minimal duty cycle of all observed pulses. Calculated by dividing the minimum active time to the period when the minimum active time was observed.
    - `Max`: Maximal duty cycle of all observed pulses. Calculated by dividing the maximum active time to the period when the maximum active time was observed.

<a id="get-pulse-count-keyword"></a>`get_pulse_count()`
- Returns the number of pulses that were observed.

<a id="get-first-change-from-default-state-keyword"></a>`get_first_change_from_default_state()`
- Returns the _test time_ when OUT first changed from its default state after `start_measuring` was called.
- Note: This function is intended for situations where you need to know precisely at what moment the PWM started.

<a id="get-last-change-to-default-state-keyword"></a>`get_last_change_to_default_state()`
- Returns the _test time_ when OUT last changed to its default state before `stop_measuring` was called.
- If the signal was not still at the default state when `stop_measuring` was called, an error is raised.
- Note: This function is intended for situations where you need to know precisely at what moment the PWM stopped.

<a id="get-min-and-max-dead-time-1-keyword"></a>`get_min_and_max_dead_time_1(ReturnType="s")`
- The dead time t<sub>d1</sub> measures the time between OUT going to Default and OUTn going to Active. It is always positive.
- This function is only available if OUTn is configured. Otherwise a `EndpointUsageError` is raised.
- Returns 2 values as a float in the given `ReturnType` unit:
    - `Min`: Minimal dead time across all observed pulses.
    - `Max`: Maximal dead time across all observed pulses.

<a id="get-min-and-max-dead-time-2-keyword"></a>`get_min_and_max_dead_time_2(ReturnType="s")`
- The dead time t<sub>d2</sub> measures the time between OUTn going to Default and OUT going to Active. It is always positive.
- This function is only available if OUTn is configured. Otherwise a `EndpointUsageError` is raised.
- Returns 2 values as a float in the given `ReturnType` unit:
    - `Min`: Minimal dead time across all observed pulses.
    - `Max`: Maximal dead time across all observed pulses.

## Model functions

The PWM endpoint has no model functions.

## Callbacks

The PWM endpoint has no callbacks.

# Interaction with other endpoints
1. You may use an additional GPIO endpoint to test error scenarios (break pin).

    The purpose of a break function is to protect power switches driven by PWM signals.  Assume your hardware supports a physical break pin that is connected to pin PB10. You want to test this behavior. The following test case can be used to determine the time between you assert the break pin and the stopping of the PWM generation.

    ```
    PWM1 Start Measuring

    # Trigger the hardware break
    ${time_break_activated} =   PB10 Set High
    Sleep   100ms

    PWM1 Stop Measuring
    ${time_pwm_last_active} =  PWM1 Get Last Change To Default State

    ${time_reaction_s} =  Evaluate  ${time_pwm_last_active} - ${time_break_activated}
    Should Be True  ${time_reaction_s} < 20e-3  PWM break took longer than 20ms.

    ```
