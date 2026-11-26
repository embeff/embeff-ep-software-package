<!-- The keyword sections of this page are generated from the Python docstrings; see tools/docs/generate_docs.py -->
# Incremental-Encoder Endpoint
The Incremental-Encoder endpoint makes it possible to generate quadrature-encoded signals.

## Technicals
- Maximum number of configured endpoints: 5.
- Maximal pulse rate: 25 million pulses per second.
- Minimal pulse rate: 0.1 pulses per second.
- Minimum pulse discretization: 10ns.

## Configuration
This section lists the properties that can be configured for each `IE` endpoint.

| Setting      | Description | Values |
| ----------- | ----------- | ------- |
| OUT_A  | Package number for DUT pin that receives the A signal. | DUT-dependent |
| OUT_B  | Package number for DUT pin that receives the B signal. | DUT-dependent |

### Example
```yaml
IE:
 IE0:
  OUT_A: 49
  OUT_B: 50
```

## General behavior
Once `set` is called, the endpoint generates quadrature-encoded signals continuously.

Ideal quadrature-encoded signals have 2 properties
- They are symmetric: The duty cycle of both A and B is 180° (equivalent to 50%).
- B follows A with a phase difference of 90°.

Real quadrature-encoded signals deviate from the ideal. The endpoint supports a _symmetry error_ and a _phase error_ to reflect this.
- The _symmetry error_ changes the duty cycle. A positive error increases the duty cycle above 180°. A negative error decreases the duty cycle below 180°.
- The _phase error_ changes the phase difference. A positive error increases the phase difference above 90°. A negative error decreases the phase difference below 90°.

![10 pulses per second.](Incremental-Encoder-NoError.drawio.png)

![10 pulses per second.](Incremental-Encoder-SymmetryError.drawio.png)

![10 pulses per second.](Incremental-Encoder-PhaseError.drawio.png)

## Setup & Teardown
`stop` is automatically called after each test.

## Test frontend

Setup, controlling and observing functions are shown by their name. [Robot Framework](../TestFrontend/RobotFramework.md) and [pytest](../TestFrontend/pytest.md) explain how to call them on an endpoint instance.

### Setup functions

<a id="stop-keyword"></a>`stop()`
- A and B are driven LOW.

### Controlling functions

<a id="set-keyword"></a>`set(PulseRate, SymmetryError="0deg", PhaseError="0deg")`
- Pulse rate determines how many pulses are emitted per second. Both A and B will each follow this pulse rate.
- The pulse rate can be positive or negative to indicate a direction. In case of negative pulse rate, A and B outputs are interchanged internally.
- The unit of the pulse rate is `Hz`.
- You may call `set` multiple times without calling `stop` in between. This can be useful to indicate a change of direction at runtime.
- The symmetry error must be between -90° and +90°.
- The phase error must be between -45° and +45°.
- The resulting signal must obey the _Minimum pulse discretization_ specified in the Technicals section. If this discretization would change any signal parameter by more than 1%, a `EndpointUsageError` is raised.
- This function returns immediately.

### Observing functions

The Incremental-Encoder endpoint has no observing functions.

## Model functions

The Incremental-Encoder endpoint has no model functions.

## Callbacks

The Incremental-Encoder endpoint has no callbacks.
