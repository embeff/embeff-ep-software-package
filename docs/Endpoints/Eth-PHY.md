<!-- The keyword sections of this page are generated from the Python docstrings; see tools/docs/generate_docs.py -->
# Eth-PHY Endpoint

The Eth-PHY endpoint makes it possible to test industry-typical Ethernet MAC implementations.

The endpoint implements a RMII interface to communicate with the DUT. All tests happen on the OSI Layer 2 (Data Link).
In this way the endpoint replaces a specific PHY so that the tests are independent from the physical layer.
The endpoint makes it possible to test frame sending and receiving.

Abbreviations across the document:
- r/w = read and write access
- r = read only access
- sc = self clearing

## Technicals

- Maximum number of configured endpoints: 1.
- Support for 100Mb/s speed.
- Acts as a RMII v1.2 interface.
- Compliant with IEEE 802.3-2002. _Frame_ refers to the MAC frame format (defined in 3.1.1).
- Maximum frame size is limited to 1518 Bytes for both sending and receiving.
  - 6 Bytes Destination
  - 6 Bytes Source
  - 2 Bytes Length/Type
  - up to 1500 Bytes Data
  - 4 Bytes CRC

## Configuration

This section lists the properties that can be configured for each `Eth-PHY` endpoint.

| Setting      | Description | Signal Direction from DUT | Values |
| ------------ | ----------- | ------- | ------- |
| DUT_TXD0     | Package number for the RMII TXD[0] pin.   | Output       | DUT-dependent |
| DUT_TXD1     | Package number for the RMII TXD[1] pin.   | Output       | DUT-dependent |
| TX_EN        | Package number for the RMII TX_EN pin.    | Output       | DUT-dependent |
| CRS_DV       | Package number for the RMII CRS_DV pin.   | Input        | DUT-dependent |
| DUT_RXD0     | Package number for the RMII RXD[0] pin.   | Input        | DUT-dependent |
| DUT_RXD1     | Package number for the RMII RXD[1] pin.   | Input        | DUT-dependent |
| REF_CLK      | Package number for the RMII REF_CLK pin.  | Input        | DUT-dependent |
| MDC          | Package number for the MDC pin.           | Output       | DUT-dependent |
| MDIO         | Package number for the MDIO pin.          | Input/Output | DUT-dependent |

## General behavior

The endpoint supports a strict mode that ensures tests are failing when unexpected results such as invalid frames occur.

Strict mode will be invalidated when:

- An invalid frame is received (specified frame length inconsistent or CRC error).
- The receive buffer is exhausted so that the last received frames are discarded.
- The DUT attempts to alter the value of a bit in a [management register](#management-registers) flagged as r (read only) for the DUT.

Automatic Strict mode Check is enabled by default and will fail in teardown, it can be disabled with `disable_automatic_strict_mode_check`.

Note that all strict mode errors are cleared on [`start`](#start-keyword). Using [`start`](#start-keyword) more than once per test could cause strict mode invalidations to go unnoticed.


## Capabilities
### Management Registers
Once started, the endpoint resembles a specific PHY that can be managed through 32 management registers. 3 of these registers have a pre-defined
purpose and provide limited read/write access, while all others are freely accessible for both the endpoint and the DUT:

| Register              | Endpoint access | DUT access |
| :-------------------- | :-------------: | :--------: |
| Control (0)           | r               | r/w        |
| Status (1)            | r/w             | r          |
| 2-14                  | r/w             | r/w        |
| Extended Status (15)  | r               | r          |
| 16-31                 | r/w             | r/w        |

The DUT can access these registers through the MDC and MDIO pins. The endpoint
can access these registers using following keywords:

| Keyword                       | Description |
| ----------------------------- | ----------- |
| `get_control`                 | Reads any single property of the control register (0)
| `set_status`                  | Sets bits 1, 2 and 4 of the status register (1)
| `read_management_register`    | Reads the whole contents of any register
| `write_management_register`   | Writes the contents of registers 2-14 and 16-31

The 3 pre-defined registers have following bit definitions:

#### Control register (0) bit definitions
| Bit(s) | Name                     | Description                                                                 | DUT access  | Endpoint access | Default | Setting Dependency   |
| ------ | ------------------------ | --------------------------------------------------------------------------- | :---------: | :-------------: | ------- | -------------------- |
| 15     | Reset                    | 1 = PHY reset<br>0 = normal operation                                       | r/w, sc     | r               | 0       |
| 14     | Loopback                 | unsupported                                                                 | r           | r               | 0       |
| 13     | Speed Selection (LSB)    | <table> <body> <tr> <td>Bit 6</td> <td>Bit 13</td> <td>Value</td> </tr> <tr> <td>1</td> <td>1</td> <td>Reserved</td> </tr> <tr> <td>1</td> <td>0</td> <td>1000 Mb/s (unsupported)</td> </tr> <tr> <td>0</td> <td>1</td> <td>100 Mb/s</td> <tr> <td>0</td> <td>0</td> <td>10 Mb/s</td> </tr> </tbody> </table> | r/w (r) | r | 1 (0) | Dependent on the PHY's available modes of operations specified at [`start`](#start-keyword):<br> If the PHY is able to operate at both 10 Mb/s and 100 Mb/s speeds, this bit is writable. (Otherwise it is not). If the PHY is able to operate at 100 Mb/s, this bit defaults to 1. (Otherwise it defaults to 0.) |
| 12     | Auto-Negotiation Enable  | 1 = enable auto-negotiation process<br>0 = disable auto-negotiation process | r/w (r)     | r               | 1 (0)   | Dependent on the PHY's [auto-negotiation](#auto-negotiation) ability specified at [`start`](#start-keyword):<br>If the PHY is able to perform auto-negotiation, this bit is writable and defaults to 1.<br>(Otherwise it is not writable and defaults to 0.) |
| 11     | Power Down               | 1 = power down<br>0 = normal operation                                      | r/w         | r               | 0       |
| 10     | Isolate                  | 1 = electrically Isolate PHY from RMII<br>0 = normal operation              | r/w         | r               | 0       |
| 9      | Restart Auto-Negotiation | 1 = restart auto-negotiation process<br>0 = normal operation                | r (r/w), sc | r               | 0       | Dependent on the PHY's [auto-negotiation](#auto-negotiation) ability specified at [`start`](#start-keyword):<br>If the PHY is able to perform auto-negotiation, this bit is writable.<br>(Otherwise it is not writable.) |
| 8      | Duplex Mode              | 1 = full duplex<br>0 = half duplex                                          | r/w (r)     | r               | 1 (0)   | Dependent on the PHY's available modes of operations specified at [`start`](#start-keyword):<br> If the PHY is able to perform both half and full duplex communication, this bit is writable. (Otherwise it is not). If the PHY is able to perform full duplex communication, this bit defaults to 1. (Otherwise it defaults to 0.) |
| 7      | Collision Test           | unsupported                                                                 | r           | r               | 0       |
| 6      | Speed Selection (MSB)    | see bit 13                                                                  | r           | r               | 0       |
| 5      | Unidirectional enable    | unsupported                                                                 | r           | r               | 0       |
| 4:0    | Reserved                 | Write as 0, ignore on read                                                  | r           | r               | 0       |

#### Status register (1) bit definitions
- Some properties of the status register define the PHY's functional abilities while others provide basic link status information.
- The properties defining functional abilities are set during execution of the [`start`](#start-keyword) keyword and can not be altered during runtime.
- The properties providing link status information are writable during runtime by using the [`set_status`](#set-status-keyword) keyword.

| Bit | Name                      | Description                                                                                                                          | DUT access | Endpoint access | Default        |
| --- | ------------------------- | ------------------------------------------------------------------------------------------------------------------------------------ | :--------: | :-------------: | -------------- |
| 15  | 100BASE-T4                | 1 = PHY able to perform 100BASE-T4<br>0 = PHY not able to perform 100BASE-T4                                                         | r          | r               | Set at [`start`](#start-keyword) |
| 14  | 100BASE-X Full Duplex     | 1 = PHY able to perform full duplex 100BASE-X<br>0 = PHY not able to perform full duplex 100BASE-X                                   | r          | r               | Set at [`start`](#start-keyword) |
| 13  | 100BASE-X Half Duplex     | 1 = PHY able to perform half duplex 100BASE-X<br>0 = PHY not able to perform half duplex 100BASE-X                                   | r          | r               | Set at [`start`](#start-keyword) |
| 12  | 10 Mb/s Full Duplex       | 1 = PHY able to operate at 10 Mb/s in full duplex mode<br>0 = PHY not able to operate at 10 Mb/s in full duplex mode                 | r          | r               | Set at [`start`](#start-keyword) |
| 11  | 10 Mb/s Half Duplex       | 1 = PHY able to operate at 10 Mb/s in half duplex mode<br>0 = PHY not able to operate at 10 Mb/s in half duplex mode                 | r          | r               | Set at [`start`](#start-keyword) |
| 10  | 100BASE-T2 Full Duplex    | 1 = PHY able to perform full duplex 100BASE-T2<br>0 = PHY not able to perform full duplex 100BASE-T2                                 | r          | r               | Set at [`start`](#start-keyword) |
| 9   | 100BASE-T2 Half Duplex    | 1 = PHY able to perform half duplex 100BASE-T2<br>0 = PHY not able to perform half duplex 100BASE-T2                                 | r          | r               | Set at [`start`](#start-keyword) |
| 8   | Extended Status           | 1 = Extended status information in register 15<br>0 = No extended status information in register 15                                  | r          | r               | 0              |
| 7   | Unidirectional ability    | 1 = PHY able to transmit from media independent interface regardless of whether the PHY has determined that a valid link has been established<br>0 = PHY able to transmit from media independent interface only when the PHY has determined that a valid link has been established | r | r | 0 |
| 6   | MF Preamble Suppression   | 1 = PHY will accept management frames with preamble suppressed<br>0 = PHY will not accept management frames with preamble suppressed | r          | r               | 0              |
| 5   | Auto-Negotiation Complete | 1 = auto-negotiation process completed<br>0 = auto-negotiation process not completed                                                 | r          | r               | 0, Set by PHY  |
| 4   | Remote Fault              | 1 = remote fault condition detected<br>0 = no remote fault condition detected                                                        | r          | r/w             | 0              |
| 3   | Auto-Negotiation Ability  | 1 = PHY is able to perform auto-negotiation<br>0 = PHY is not able to perform auto-negotiation                                       | r          | r               | Set at [`start`](#start-keyword) |
| 2   | Link Status               | 1 = link is up<br>0 = link is down                                                                                                   | r          | r/w             | 0              |
| 1   | Jabber Detect             | 1 = jabber condition detected<br>0 = no jabber condition detected                                                                    | r          | r/w             | 0              |
| 0   | Extended Capability       | 1 = extended register capabilities<br>0 = basic register set capabilities only                                                       | r          | r               | 0              |

#### Extended Status register (15) bit definitions
| Bit(s) | Name     | Description                | DUT access  | Endpoint access | Default |
| ------ | -------- | -------------------------- | :---------: | :-------------: | ------- |
| 15:0   | Reserved | Write as 0, ignore on read | r           | r               | 0       |

Refer to IEEE 802.3-2002 Clause 22 for more information on management registers.

### Unidirectional Ability
For the sake of simplicity, this endpoint does not support the use of 'unidirectional ability'. Bit 5 of the [control register](#control-register-0-bit-definitions) and bit 7 of the [status register](#status-register-1-bit-definitions) are permanently set to 0.

### Preamble Suppression
For the sake of simplicity, this endpoint does not support preamble suppression. Bit 6 of the [status register](#status-register-1-bit-definitions) is permanently set to 0.

[//]: # (Preamble suppression is implemented, but only tested in simulation. If a user requests the feature, it can quickly be enabled. The following 3 comments are intended to replace the current preamble suppression documentation in that case.)

[//]: # (This endpoint is able to simulate PHYs that are capable of preamble suppression on the MDIO interface. Use the parameter `Preamble Suppression
Ability` of the `start` keyword to specify whether the simulated PHY should provide this functionality to the DUT or not. If enabled, the
PHY can respond to MDIO transactions without preambles or with shortened preambles.)

[//]: # (Note that even if the feature is enabled, at least one transaction with a full preamble has to occur on the bus to establish synchronization.
The PHY only responds to transactions with suppressed preambles, if a synchronization has been established previously. It will ignore all
transactions that occur before bus synchronization. Citing IEEE 802.3-2002 Clause 22:)

[//]: # (> A PHY shall observe a sequence of 32 contiguous one bits on MDIO with
32 corresponding cycles on MDC before it responds to any transaction. <br>
If the STA determines that every PHY that is connected to the MDIO signal is able to accept management
frames that are not preceded by the preamble pattern, then the STA may suppress the generation of the
preamble pattern, and may initiate management frames with the ST pattern.)

### Auto-Negotiation
This endpoint is able to simulate auto-negotiation processes. Use the parameter `Auto-Negotiation Ability` of the [`start`](#start-keyword) keyword to
specify whether the simulated PHY should provide this functionality to the DUT or not. Additionally, use the parameter `Auto-Negotiation
Result` to predetermine the result of upcoming negotiations. Use the parameter `Auto-Negotiation Duration` to specify the amount of time
a negotiation takes. You can use the keyword [`update_auto_negotiation_parameters`](#update-auto-negotiation-parameters-keyword) to update these parameters at runtime.

If the endpoint was configured to provide this feature, it can be enabled or disabled by setting/clearing 'Auto-Negotiation Enable' (bit 12) in the [control register](#control-register-0-bit-definitions) through the
MDIO interface. If enabled, auto-negotiation is simulated at PHY power-up, after a PHY reset and after manual initiation by the DUT (see [`expect_reset`](#expect-reset-keyword)
and [`expect_auto_negotiation`](#expect-auto-negotiation-keyword) keywords). The DUT can manually restart the auto-negotiation by setting 'Restart Auto-Negotiation process'
(bit 9) in the [control register](#control-register-0-bit-definitions). When the negotiation starts, 'Auto-Negotiation Complete' (bit 5) in the
[status register](#status-register-1-bit-definitions) is set to 0. Once the predetermined negotiation duration has passed, the bit is set to 1 and - if previously specified at [`start`](#start-keyword) or using the [`update_auto_negotiation_parameters`](#update-auto-negotiation-parameters-keyword) keyword - the predetermined result is applied in management registers 4, 5 and 6. If the negotiation result is not specified by at least one of the two keywords, management registers 4, 5 and 6 can be used as generic registers to simulate other PHY specific behaviors.

### Reset
The DUT is able to reset the PHY by setting 'Reset' (bit 15) in the [control register](#control-register-0-bit-definitions). This action resets the
status and control registers to their default states. This bit is self-clearing, and remains logic 1 until the reset process is completed. Write transactions
to the control register are not accepted until the reset process is completed.

### Power down
The DUT is able to place the PHY in a simulated low-power consumption state by setting 'Power Down' (bit 11) in the [control register](#control-register-0-bit-definitions).
Clearing bit 11 allows for normal operation. While in the power-down state, the pins CRS_DV, DUT_RXD0, DUT_RXD1 and REF_CLK are all held to logic 0 and signals on the pins
DUT_TXD0, DUT_TXD1 and TX_EN are ignored. The behavior of the MDIO interface (pins MDIO and MDC) and the [management register](#management-registers) remains unaltered while
powered down.

### Isolation
The DUT is able to force the PHY to electrically isolate its data paths from the RMII by setting 'Isolate' (bit 10) in the [control register](#control-register-0-bit-definitions).
Clearing bit 10 allows for normal operation. When isolated from the RMII, the PHY ignores signals on the pins DUT_TXD0, DUT_TXD1 and TX_EN, and the pins CRS_DV, DUT_RXD0, DUT_RXD1
and REF_CLK are put into tristate. The behavior of the MDIO interface (pins MDIO and MDC) and the [management register](#management-registers) remains unaltered while in isolation.

:information_source: For normal operation, both bit 10 and 11 of the [control register](#control-register-0-bit-definitions) have to be cleared to zero. REF_CLK will restart withing 0.5 s after both bit 11 and 10 are cleared to zero.

### Collision Test
Collision test functionality is not supported. Bit 7 of the [control register](#control-register-0-bit-definitions) is permanently set to 0.

### Loopback
Loopback across the RMII is impossible since the collision detection is done in the MAC, which assumes independent receive and transmit control signals. Loopback is therefore not
supported. Bit 14 of the [control register](#control-register-0-bit-definitions) is permanently set to 0.

## Setup & Teardown
`stop` is automatically called and the receive buffer is cleared after each test.

## Test frontend

Setup, controlling and observing functions are shown by their name. [Robot Framework](../TestFrontend/RobotFramework.md) and [pytest](../TestFrontend/pytest.md) explain how to call them on an endpoint instance.

### Setup functions

<a id="start-keyword"></a>`start(PHY_Address=0, Modes_of_Operation=['10 Mb/s Full Duplex', '10 Mb/s Half Duplex'], Auto_Negotiation_Ability=True, Auto_Negotiation_Duration="50ms", Auto_Negotiation_Result=None)`
- Starts generating REF_CLK.
- Enables receiver.
- Defines the abilities of the simulated PHY and sets the [status register](Eth-PHY.md#status-register-1-bit-definitions) accordingly.
- Prepares [auto-negotiation](Eth-PHY.md#auto-negotiation) processes.
- `PHY_Address` (5 bit integer): The device address of the simulated PHY. The endpoint will ignore all MDIO frames with different addresses.
- `Modes_of_Operation` (list): List of modes of operation the PHY is able to perform. Can be any non empty subset of: <br> `10 Mb/s Full Duplex`, `10 Mb/s Half Duplex`, `100BASE-T4`, `100BASE-X Full Duplex`, `100BASE-X Half Duplex`, `100BASE-T2 Full Duplex`, `100BASE-T2 Half Duplex`
- `Auto_Negotiation_Ability` (bool):
    - 1: The PHY simulates an auto-negotiation process at power-up, after a reset and after manual initiation.
    - 0: The PHY never simulates an auto-negotiation.
- `Auto_Negotiation_Duration` (duration): Duration of upcoming simulated auto-negotiations.
    - Resolution: 10ns
    - Maximum duration: 42s
- `Auto_Negotiation_Result` (list): The result of upcoming simulated auto-negotiations. If omitted, registers 4, 5 and 6 are not affected by auto-negotiations. Otherwise, a list of 3 16-bit values shall be passed. These three values are written into registers 4, 5 and 6 respectively when upcoming auto-negotiations finish.

<a id="stop-keyword"></a>`stop()`
- Stops generating REF_CLK.
- Disables receiver.

<a id="disable-automatic-strict-mode-check-keyword"></a>`disable_automatic_strict_mode_check()`
- Disable the strict mode check in Teardown for the current test.

<a id="expect-reset-keyword"></a>`expect_reset(Duration)`
- Sets the duration of the next PHY [reset operation](Eth-PHY.md#reset) initiated by the DUT.
- The DUT can manually reset the PHY by setting 'Reset' (bit 15) in the [control register](Eth-PHY.md#control-register-0-bit-definitions).
- If no DUT-initiated reset operation completes between the call of this keyword and test teardown, the strict mode is invalidated.
- If a DUT-initiated reset operation completes before this keyword has been called, the strict mode is invalidated.
- `Duration`: Duration before the PHY clears 'Reset' (bit 15) in the [control register](Eth-PHY.md#control-register-0-bit-definitions) after it was set by the DUT.
    - Resolution: 10ns
    - Maximum duration: 42s

<a id="update-auto-negotiation-parameters-keyword"></a>`update_auto_negotiation_parameters(Duration, Result=None)`
- Sets the result and duration of upcoming auto-negotiations.
- `Duration`: Predetermined duration of upcoming auto-negotiations.
    - Resolution: 10ns
    - Maximum duration: 42s
- `Result` (list): The result of upcoming simulated auto-negotiations. If left as `None`, registers 4, 5 and 6 are not affected by auto-negotiations. Otherwise, a list of 3 16-bit values shall be passed. These three values are written into registers 4, 5 and 6 respectively when upcoming auto-negotiations finish.

<a id="expect-auto-negotiation-keyword"></a>`expect_auto_negotiation(Duration)`
- The DUT can manually initiate an auto-negotiation by setting 'Restart Auto-Negotiation process' (bit 9) in the [control register](Eth-PHY.md#control-register-0-bit-definitions).
- If no manual initiation completes between the call of this keyword and test teardown, the strict mode is invalidated.
- If a manual initiation completes before this keyword has been called, the strict mode is invalidated.
- `Duration`: Duration before the PHY clears 'Restart Auto-Negotiation process' (bit 9) in the control register after it was set by the DUT.
    - Resolution: 10ns
    - Maximum duration: 42s
- Note: this duration is different from the actual negotiation duration, specified with the [`start`](Eth-PHY.md#start-keyword) or [`update_auto_negotiation_parameters`](Eth-PHY.md#update-auto-negotiation-parameters-keyword) keyword.

### Controlling functions

<a id="send-frame-keyword"></a>`send_frame(Destination, Source, Type=None, Data: ByteData=bytes())`
- `Destination` & `Source` are in format: XX:XX:XX:XX:XX:XX
- Blocks until the frame is sent.

<a id="send-frame-with-corrupted-fcs-keyword"></a>`send_frame_with_corrupted_fcs(Destination, Source, Type=None, Data: ByteData=bytes())`
- `Destination` & `Source` are in format: XX:XX:XX:XX:XX:XX
- The CRC calculation within the FCS field is changed so that receivers should discard this frame as invalid.
- Blocks until the frame is sent.

<a id="send-frame-periodically-keyword"></a>`send_frame_periodically(Destination, Source, Type=None, Data: ByteData=bytes(), Pause="0ms", Count=0xffff, WaitForCompletion=True)`
- `Destination` & `Source` are in format: XX:XX:XX:XX:XX:XX
- The endpoint sends `Count` frames with the same content.
- If `Count` is not set, frames are sent until Stop is called. `WaitForCompletion` must be set to `False`.
- The pause determines the desired time distance between the end of one frame and the beginning of the next frame. If [`send_frame`](Eth-PHY.md#send-frame-keyword) or [`send_frame_with_corrupted_fcs`](Eth-PHY.md#send-frame-with-corrupted-fcs-keyword) are used in parallel, the actual pause might be longer than desired. Minimal pause is 0us, maximal 1s.

<a id="write-management-register-keyword"></a>`write_management_register(Register_Address, Value)`
- Generic function to write to the [management registers](Eth-PHY.md#management-registers) of the simulated PHY.
- `Register_Address`: Integer in the range 2-14 or 16-31
- `Value`: Any 16-bit value

<a id="set-status-keyword"></a>`set_status(Property, Value)`
- Sets a single property of the PHY's [status register](Eth-PHY.md#status-register-1-bit-definitions) without altering others.
- `Property`: literal, see below
- `Value`: bool, see below
- The following `Property` and `Value` combinations are supported:

  | Property                  | Value description                                                                | Default Value | Bit |
  | ------------------------- | -------------------------------------------------------------------------------- | ------------- | --- |
  | Remote Fault              | 1 = remote fault condition detected <br> 0 = no remote fault condition detected  | 0             | 4
  | Link Status               | 1 = link is up <br> 0 = link is down                                             | 1             | 2
  | Jabber Detect             | 1 = jabber condition detected <br> 0 = no jabber condition detected              | 0             | 1

### Observing functions

<a id="check-strict-mode-keyword"></a>`check_strict_mode()`
- Fails test if strict mode is invalid.

<a id="get-received-frames-keyword"></a>`get_received_frames()`
- Returns a list of all received frames.
- Each element in the list contains the following attributes for the valid frame:
    - `destination`: The 48-bit destination address.
    - `source`: The 48-bit source address.
    - `data`: The payload (MAC Client Data). 46-1500 bytes length.
    - `type`: The EtherType indicating the MAC client protocol. `None` if the underlying frame is encoding the payload length instead.
    - `crc`: The frame check sequence (CRC) value.
    - `time`: The _test time_ when TX_EN was asserted.
- Following attributes are available for frames containing error(s):
    - `time`: The _test time_ when TX_EN was asserted.
    - `errors`: Contains detected errors:
        - `Maximum frame size of 1518 bytes exceeded, excess data is discarded.`
        - `Minimum frame size is 64 bytes, received packet has [length].`
        - `Preamble error.`
        - `Start frame delimiter error.`
        - `CRC error.`
    - `raw`: Contains received data.

<a id="read-management-register-keyword"></a>`read_management_register(Register_Address)`
- Generic function to read from any of the 32 [management registers](Eth-PHY.md#management-registers) of the simulated PHY.
- Returns the 16-bit value of the register specified using `Register_Address`.
- `Register_Address`: Integer between 0 and 31

<a id="get-control-keyword"></a>`get_control(Property)`
- Reads a single property of the PHY's [control register](Eth-PHY.md#control-register-0-bit-definitions).
- To read all properties at once, the keyword `read_management_register` can be used with `Register_Address` = 0.
- `Property`: literal, following properties can be read using this keyword:

  | Property                  | Return Type | Return value description                                                                            | Bit(s) |
  | ------------------------- | ----------- | ----------------------------------------------------------------------------------------------------- | ------ |
  | Speed Selection           | string      | `10 Mb/s`, `100 Mb/s` (The endpoint currently only supports up to 100Mb/s. [See here](Eth-PHY.md#technicals)) | 13, 6
  | Auto-Negotiation Enable   | bool        | `1`: enable auto-negotiation process <br> `0`: disable auto-negotiation process                     | 12
  | Power Down                | bool        | `1`: power down <br> `0`: normal operation                                                          | 11
  | Isolate                   | bool        | `1`: electrically isolate PHY from MII or GMII <br> `0`: normal operation                           | 10
  | Duplex-Mode               | bool        | `1`: full duplex <br> `0`: half duplex                                                              | 8

- Properties 'Reset' (bit 15) and 'Restart Auto-Negotiation process' (bit 9) are self clearing and can be assessed using the [`expect_reset`](Eth-PHY.md#expect-reset-keyword) and [`expect_auto_negotiation`](Eth-PHY.md#expect-auto-negotiation-keyword) keywords.

## Model functions

The Eth-PHY endpoint has no model functions.

## Callbacks

The Eth-PHY endpoint has no callbacks.
