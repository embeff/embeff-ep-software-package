<!-- The keyword sections of this page are generated from the Python docstrings; see tools/docs/generate_docs.py -->
# CAN Endpoint
The CAN endpoint makes it possible to test CAN implementations.
No transceiver is involved, the endpoint works on the digital pin level.
A dominant state is indicated with a logical 0. Therefore the endpoint and DUT TX lines are wired with a logical-and internally. The CAN Endpoint uses a recessive (logical 1) on the signal line if inactive.

![](CAN-connection.drawio.png)

## Technicals
- Maximum number of configured endpoints: 3.
- Support for CAN 2.0A and CAN 2.0B (standard and extended frames).
- Supported bit rates: 50,100,125,250,500,800,1000 kbit/s.
- Digital signals (excluding transceiver) are used.
- Frame buffer size: Unlimited

## Configuration
This section lists the properties that can be configured for each `CAN` endpoint.

| Setting      | Description | Values |
| ----------- | ----------- | ------- |
| RX      | Package number for the CAN RX pin.  | DUT-dependent |
| TX      | Package number for the CAN TX pin.  | DUT-dependent |

### Example
```yaml
CAN:
  CAN1:
    TX: 20
    RX: 19
```

## General behavior
Both received and sent frames are recorded in a frame buffer.
The frame buffer stores the frames in a FIFO manner.

A frame can be either
- a data frame
- a remote frame
- an error frame

When the endpoint transmits a dominant bit but sees a recessive bit on the bus, an error frame is sent autonomously. This is the only situation, in which the endpoint transmits an error frame autonomously.

The endpoint supports a strict mode that ensures tests are failing when an error is seen on the bus.
Strict mode will be invalidated when:
- An error frame is observed on the bus. Error message `RemoteError: Strict mode invalidated (An error frame was observed on the bus)`.
- A not acknowledged frame is observed on the bus. Error message `RemoteError: Strict mode invalidated (A frame was not acknowledged)`

Automatic Strict mode Check is enabled by default and will fail in teardown, it can be disabled with `disable_automatic_strict_mode_check`.

## Setup & Teardown
`stop` is automatically called and the frame buffer is cleared after each test.

## Test frontend

Setup, controlling and observing functions are shown by their name. [Robot Framework](../TestFrontend/RobotFramework.md) and [pytest](../TestFrontend/pytest.md) explain how to call them on an endpoint instance.

### Setup functions

<a id="disable-automatic-strict-mode-check-keyword"></a>`disable_automatic_strict_mode_check()`
- Disable the strict mode check in Teardown for the current test.

<a id="start-keyword"></a>`start(Bitrate)`
- The endpoint starts participating on the bus when Start is called. CAN frames are acknowledged.
- The frame buffer is cleared. Received frames are recorded.

<a id="stop-keyword"></a>`stop()`
- The endpoint does not participate anymore on the bus. CAN frames are not acknowledged.
- No further frames are recorded in the frame buffer.
- Using any function except `start` or `get_frames` will create an `EndpointUsageError: Endpoint not started.`.

<a id="cause-bus-error-on-next-received-frame-keyword"></a>`cause_bus_error_on_next_received_frame(Count, BusError)`
- The endpoint will corrupt the contents of the next `Count` frame(s) that are received so the given can bus error occurs.
- Note: This method can be used to cause a transmit error on the DUT.
- `BusError`
    1. NoAck: The endpoint will not acknowledge the frame. The DUT should recognize that the frame sent was not acknowledged.
    2. Form: The endpoint will overwrite the CRC delimiter to dominant and send an active error frame.
    3. ArbitrationLost: After detecting the start of a frame, the endpoint will also start sending a frame of the highest priority (0) with no data. This will cause the DUT to loose during arbitration.

### Controlling functions

<a id="send-frame-keyword"></a>`send_frame(Id, Data: ByteData, Extended=True, StoreInBuffer=True)`
- Sends a data frame.
- Blocks until the frame is seen on the bus. If you do not want blocking behavior, use `send_frame_periodically` below with `WaitForCompletion=False`.

<a id="send-remote-frame-keyword"></a>`send_remote_frame(Id, Extended=True, StoreInBuffer=True)`
- Send a remote frame.
- Blocks until the frame is seen on the bus.
- Note: There are no CAN-FD Remote Frames. Instead, this keyword sends a non-FD remote frame.

<a id="send-frame-with-bus-error-keyword"></a>`send_frame_with_bus_error(BusError, Id, Data: ByteData, Extended=True, Resend=True)`
- The endpoint will sent the given frame and corrupt it according to the given error.
- Typically, the DUT should detect the bus error and react with an appropriate error frame.
- If an error frame is seen, it is recorded in the frame buffer.
- If resend is active and an error frame is seen, the frame will be sent again (uncorrupted) after the error frame. In this case the function blocks until the uncorrupted frame is sent on the bus.
- Note: This method can be used to cause a receive error on the DUT.
- `BusError`
    1. Form: CRC delimiter will be set to dominant.
    2. CRC: Checksum will be inverted.
    3. Stuff: Stuff bits will not be inserted into frame.

<a id="send-frame-periodically-keyword"></a>`send_frame_periodically(Id, Data: ByteData, Pause="0ms", Extended=True, Count=0xffff, StoreInBuffer=True, WaitForCompletion=True)`
- The endpoint sends `Count` frames with the same content.
- If `Count` is not set, frames are sent until Stop is called. `WaitForCompletion` must be set to `False`.
- The pause determines the desired time distance between the end of one frame and the beginning of the next frame. If there are higher-priority frames pending on the bus, the actual pause might be longer than desired. Minimal pause is 0us, maximal 1s.
- `StoreInBuffer=False` can be used to prevent quickly exhausting the frame buffer. For example sending frames periodically might be used to test high bus load for an extended amount of time.

### Observing functions

<a id="check-strict-mode-keyword"></a>`check_strict_mode()`
- Fails test if strict mode is invalid.

<a id="get-frames-keyword"></a>`get_frames()`
- List of frames in the buffer. Includes received frames and frames that were sent by the endpoint.
- Each frame contains the following attributes:
    - `time`: The _test time_ it was fully received.
    - `type`:
        - Data, DataWithoutAck - Valid data frame with/without Acknowledge.
        - RemoteRequest, RemoteRequestWithoutAck - Valid remote request frame with/without Acknowledge.
        - Error - frame contains an error.
- Depending on the frame type more attributes are available

    |                           | id | extended | data | error |
    | -----                     | -- | -------- | ---- | ----- |
    | Data(WithoutAck)          | ☑ | ☑        | ☑    | - |
    | RemoteRequest(WithoutAck) | ☑ | ☑        | -    | - |
    | Error                     | -  | -        | -     | ☑ |

    - `id`: The frame identifier.
    - `extended`: False|True.
    - `data`: The frame payload as bytes.
    - `error`: Reason for the error frame (determined by the Endpoint).

<a id="wait-for-frame-buffer-keyword"></a>`wait_for_frame_buffer(ExpectedCount, Timeout="100ms")`
- Waits until `ExpectedCount` of frames are available in the buffer.
- There is no upper limit for `ExpectedCount`. Increase `Timeout` accordingly when waiting for a large number of frames.

## Model functions

These functions can be called from the [Python-based environment simulation](System.md).

```python
def send_frame(self, Id: int | str, Data: bytes | str, Extended=True, StoreInBuffer=True) -> bool
```
- Sends a data frame.
- Does not block: the frame is written into a free hardware send buffer and the call returns without waiting for the frame to reach the bus.
- Returns `True` if the frame was queued, `False` if there was not free hardware send buffer and the frame will not be sent.

## Callbacks

This endpoint provides callbacks that can be used inside the [Python-based environment simulation](System.md).

```python
def on_frame_received(self, callback: Callable[[int, bytes], None])
```
- Registers a callback that is called when a frame is received. The callback receives the frame ID and frame data as parameters when a new frame is received on the Endpoint.
- Note that all frames will still be available in test sequence through `get_frames`.

The following example shows the usage of this callback in the model for a CAN Endpoint named `CAN1`. The test sequence reads the output variables from the `SYSTEM` endpoint, not from `CAN1`: `SYSTEM Get Output` in Robot Framework, `ep.SYSTEM.get_output()` in pytest. See [System](System.md).

```python
class SystemModel(config.Configuration):
    def __init__(self):
        self.outputs = {}
        self.outputs['FrameCount'] = 0
        self.outputs['FrameBytesTotal'] = 0

    def can_frame_received(self, frame_id, data):
        self.outputs["FrameCount"] += 1
        self.outputs["FrameBytesTotal"] += len(data)

    def start(self):
        self.CAN1.on_frame_received(self.can_frame_received)
```
