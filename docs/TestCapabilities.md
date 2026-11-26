# Test capabilities

The functions a test can use, per endpoint type: first the Robot Framework keyword, then the pytest call with its arguments and their defaults. Each heading names the page that documents the functions and the ep-config section that enables the endpoint type.

- `<CANx>`, `<GPIOPin>`, ... stand for the name of an instance in that section: `CAN: {CAN1: {...}}` gives `CAN1 Send Frame` in Robot Framework and `ep.CAN1.send_frame(...)` in pytest. These functions exist once per instance.
- Functions without a placeholder exist once, as soon as the section is present.
- pytest takes the names exactly as listed; they are case-sensitive. A Robot Framework keyword takes the same arguments, written as `Name=value` without quotes: `Pause="0ms"` becomes `Pause=0ms`. See [Robot Framework](TestFrontend/RobotFramework.md) and [pytest](TestFrontend/pytest.md).

## DUT · [CodeInterface.md](CodeInterface.md) · `invoke`, `get_output` and `get_coverage` need the ep-config section `System`

- `DUT Flash Firmware` · `ep.DUT.flash_firmware(file)`
- `DUT Invoke` · `ep.DUT.invoke(method_name, data: ByteData=bytes())`
- `DUT Power On` · `ep.DUT.power_on()`
- `DUT Power Off` · `ep.DUT.power_off()`
- `DUT Get Coverage` · `ep.DUT.get_coverage()`
- `DUT Get Output` · `ep.DUT.get_output()`
- `DUT Get Sku` · `ep.DUT.get_sku()`

## Analog-In · [Endpoints/Analog-In.md](Endpoints/Analog-In.md) · ep-config section `ANIN`

- `<ANINx> Start Periodic Measurement` · `ep.<ANINx>.start_periodic_measurement(Count, Distance, TriggerPin=None)`
- `<ANINx> Single Measurement` · `ep.<ANINx>.single_measurement(ReturnType="V")`
- `<ANINx> Get Measurement Results` · `ep.<ANINx>.get_measurement_results(Timeout="5s", ReturnType="V")`

## Analog-Out · [Endpoints/Analog-Out.md](Endpoints/Analog-Out.md) · ep-config section `ANOUT`

- `<ANOUTx> Reset` · `ep.<ANOUTx>.reset()`
- `<ANOUTx> Set Static` · `ep.<ANOUTx>.set_static(voltage)`
- `<ANOUTx> Set Sine` · `ep.<ANOUTx>.set_sine(frequency, amplitude, base, count=1, TriggerPin=None, WaitForCompletion=True)`
- `<ANOUTx> Set Square` · `ep.<ANOUTx>.set_square(frequency, amplitude, base, count=1, TriggerPin=None, WaitForCompletion=True)`
- `<ANOUTx> Set Sawtooth` · `ep.<ANOUTx>.set_sawtooth(frequency, amplitude, base, count=1, TriggerPin=None, WaitForCompletion=True)`

## CAN · [Endpoints/CAN.md](Endpoints/CAN.md) · ep-config section `CAN`

- `<CANx> Disable Automatic Strict Mode Check` · `ep.<CANx>.disable_automatic_strict_mode_check()`
- `<CANx> Start` · `ep.<CANx>.start(Bitrate)`
- `<CANx> Stop` · `ep.<CANx>.stop()`
- `<CANx> Cause Bus Error On Next Received Frame` · `ep.<CANx>.cause_bus_error_on_next_received_frame(Count, BusError)`
- `<CANx> Send Frame` · `ep.<CANx>.send_frame(Id, Data: ByteData, Extended=True, StoreInBuffer=True)`
- `<CANx> Send Remote Frame` · `ep.<CANx>.send_remote_frame(Id, Extended=True, StoreInBuffer=True)`
- `<CANx> Send Frame With Bus Error` · `ep.<CANx>.send_frame_with_bus_error(BusError, Id, Data: ByteData, Extended=True, Resend=True)`
- `<CANx> Send Frame Periodically` · `ep.<CANx>.send_frame_periodically(Id, Data: ByteData, Pause="0ms", Extended=True, Count=0xffff, StoreInBuffer=True, WaitForCompletion=True)`
- `<CANx> Check Strict Mode` · `ep.<CANx>.check_strict_mode()`
- `<CANx> Get Frames` · `ep.<CANx>.get_frames()`
- `<CANx> Wait For Frame Buffer` · `ep.<CANx>.wait_for_frame_buffer(ExpectedCount, Timeout="100ms")`

## CAN-FD · [Endpoints/CAN-FD.md](Endpoints/CAN-FD.md) · ep-config section `CAN_FD`

- `<CAN_FDx> Disable Automatic Strict Mode Check` · `ep.<CAN_FDx>.disable_automatic_strict_mode_check()`
- `<CAN_FDx> Start` · `ep.<CAN_FDx>.start(BitrateArbitration="500 kbit/s", BitrateData="2 Mbit/s")`
- `<CAN_FDx> Stop` · `ep.<CAN_FDx>.stop()`
- `<CAN_FDx> Cause Bus Error On Next Received Frame` · `ep.<CAN_FDx>.cause_bus_error_on_next_received_frame(Count, BusError)`
- `<CAN_FDx> Send Frame` · `ep.<CAN_FDx>.send_frame(Id, Data: ByteData, Extended=True, FrameMode="FD_BitrateSwitch", StoreInBuffer=True)`
- `<CAN_FDx> Send Remote Frame` · `ep.<CAN_FDx>.send_remote_frame(Id, Extended=True, StoreInBuffer=True)`
- `<CAN_FDx> Send Frame With Bus Error` · `ep.<CAN_FDx>.send_frame_with_bus_error(BusError, Id, Data: ByteData, Extended=True, FrameMode="FD_BitrateSwitch", Resend=True)`
- `<CAN_FDx> Send Frame Periodically` · `ep.<CAN_FDx>.send_frame_periodically(Id, Data: ByteData, Extended=True, FrameMode="FD_BitrateSwitch", Pause="0ms", Count=0xffff, StoreInBuffer=True, WaitForCompletion=True)`
- `<CAN_FDx> Check Strict Mode` · `ep.<CAN_FDx>.check_strict_mode()`
- `<CAN_FDx> Get Frames` · `ep.<CAN_FDx>.get_frames()`
- `<CAN_FDx> Wait For Frame Buffer` · `ep.<CAN_FDx>.wait_for_frame_buffer(ExpectedCount, Timeout="100ms")`

## EEPROM-SPI · [Endpoints/EEPROM-SPI.md](Endpoints/EEPROM-SPI.md) · ep-config section `EEPROM_SPI`

- `<EEPROM_SPIx> Erase` · `ep.<EEPROM_SPIx>.erase()`
- `<EEPROM_SPIx> Set Memory` · `ep.<EEPROM_SPIx>.set_memory(data)`
- `<EEPROM_SPIx> Read Memory Address` · `ep.<EEPROM_SPIx>.read_memory_address(address, count)`
- `<EEPROM_SPIx> Read Memory` · `ep.<EEPROM_SPIx>.read_memory()`

## Eth-PHY · [Endpoints/Eth-PHY.md](Endpoints/Eth-PHY.md) · ep-config section `ETH_PHY`

- `<ETH_PHYx> Start` · `ep.<ETH_PHYx>.start(PHY_Address=0, Modes_of_Operation=['10 Mb/s Full Duplex', '10 Mb/s Half Duplex'], Auto_Negotiation_Ability=True, Auto_Negotiation_Duration="50ms", Auto_Negotiation_Result=None)`
- `<ETH_PHYx> Stop` · `ep.<ETH_PHYx>.stop()`
- `<ETH_PHYx> Disable Automatic Strict Mode Check` · `ep.<ETH_PHYx>.disable_automatic_strict_mode_check()`
- `<ETH_PHYx> Expect Reset` · `ep.<ETH_PHYx>.expect_reset(Duration)`
- `<ETH_PHYx> Update Auto Negotiation Parameters` · `ep.<ETH_PHYx>.update_auto_negotiation_parameters(Duration, Result=None)`
- `<ETH_PHYx> Expect Auto Negotiation` · `ep.<ETH_PHYx>.expect_auto_negotiation(Duration)`
- `<ETH_PHYx> Send Frame` · `ep.<ETH_PHYx>.send_frame(Destination, Source, Type=None, Data: ByteData=bytes())`
- `<ETH_PHYx> Send Frame With Corrupted Fcs` · `ep.<ETH_PHYx>.send_frame_with_corrupted_fcs(Destination, Source, Type=None, Data: ByteData=bytes())`
- `<ETH_PHYx> Send Frame Periodically` · `ep.<ETH_PHYx>.send_frame_periodically(Destination, Source, Type=None, Data: ByteData=bytes(), Pause="0ms", Count=0xffff, WaitForCompletion=True)`
- `<ETH_PHYx> Write Management Register` · `ep.<ETH_PHYx>.write_management_register(Register_Address, Value)`
- `<ETH_PHYx> Set Status` · `ep.<ETH_PHYx>.set_status(Property, Value)`
- `<ETH_PHYx> Check Strict Mode` · `ep.<ETH_PHYx>.check_strict_mode()`
- `<ETH_PHYx> Get Received Frames` · `ep.<ETH_PHYx>.get_received_frames()`
- `<ETH_PHYx> Read Management Register` · `ep.<ETH_PHYx>.read_management_register(Register_Address)`
- `<ETH_PHYx> Get Control` · `ep.<ETH_PHYx>.get_control(Property)`

## FMI · [Endpoints/FMI.md](Endpoints/FMI.md) · ep-config section `FMI`

- `FMI Load Model` · `ep.FMI.load_model(fmu_file)`
- `FMI Start Model` · `ep.FMI.start_model()`
- `FMI Stop Model` · `ep.FMI.stop_model()`
- `FMI Get Output` · `ep.FMI.get_output(variable)`

## GPIO · [Endpoints/GPIO.md](Endpoints/GPIO.md) · ep-config section `GPIO`

- `GPIO Measure Between Rising Edges` · `ep.GPIO.measure_between_rising_edges(start_io, stop_io)`
- `GPIO Measure Between Falling Edges` · `ep.GPIO.measure_between_falling_edges(start_io, stop_io)`
- `GPIO Measure Between Rising And Falling Edge` · `ep.GPIO.measure_between_rising_and_falling_edge(start_io, stop_io)`
- `GPIO Measure Between Falling And Rising Edge` · `ep.GPIO.measure_between_falling_and_rising_edge(start_io, stop_io)`
- `<GPIOPin> Set High` · `ep.<GPIOPin>.set_high()`
- `<GPIOPin> Set Low` · `ep.<GPIOPin>.set_low()`
- `<GPIOPin> Set Tristate` · `ep.<GPIOPin>.set_tristate()`
- `GPIO Set Pins` · `ep.GPIO.set_pins(high=[], low=[], tristate=[])`
- `GPIO Generate High Pulse On Pin` · `ep.GPIO.generate_high_pulse_on_pin(pin, length, WaitForCompletion=True)`
- `GPIO Generate Low Pulse On Pin` · `ep.GPIO.generate_low_pulse_on_pin(pin, length, WaitForCompletion=True)`
- `<GPIOPin> Read` · `ep.<GPIOPin>.read()`
- `GPIO Read Pins` · `ep.GPIO.read_pins(pins: List=[])`
- `GPIO Get Measurement Result` · `ep.GPIO.get_measurement_result(id, timeout="5s", ReturnType="s")`

## I2C-Target · [Endpoints/I2C-Target.md](Endpoints/I2C-Target.md) · ep-config section `I2C_TARGET`

- `<I2C_TARGETx> Start` · `ep.<I2C_TARGETx>.start(AckedAddresses: List)`
- `<I2C_TARGETx> Stop` · `ep.<I2C_TARGETx>.stop()`
- `<I2C_TARGETx> Respond To Read` · `ep.<I2C_TARGETx>.respond_to_read(Address, ResponseData: ByteData)`
- `<I2C_TARGETx> Cause Arbitration Loss` · `ep.<I2C_TARGETx>.cause_arbitration_loss()`
- `<I2C_TARGETx> Cause Bus Error` · `ep.<I2C_TARGETx>.cause_bus_error()`
- `<I2C_TARGETx> Disable Automatic Strict Mode Check` · `ep.<I2C_TARGETx>.disable_automatic_strict_mode_check()`
- `<I2C_TARGETx> Get All Written Data` · `ep.<I2C_TARGETx>.get_all_written_data(Address)`
- `<I2C_TARGETx> Get Write Transfers` · `ep.<I2C_TARGETx>.get_write_transfers()`
- `<I2C_TARGETx> Wait For Data Written` · `ep.<I2C_TARGETx>.wait_for_data_written(Address, ExpectedCount, Timeout="100ms")`
- `<I2C_TARGETx> Wait For Data Read` · `ep.<I2C_TARGETx>.wait_for_data_read(Address, ExpectedCount, Timeout="100ms")`
- `<I2C_TARGETx> Get Read Transfers` · `ep.<I2C_TARGETx>.get_read_transfers()`
- `<I2C_TARGETx> Read Minimal Clock Period` · `ep.<I2C_TARGETx>.read_minimal_clock_period(ReturnType="s")`
- `<I2C_TARGETx> Read Maximal Clock Period` · `ep.<I2C_TARGETx>.read_maximal_clock_period(ReturnType="s")`
- `<I2C_TARGETx> Check Strict Mode` · `ep.<I2C_TARGETx>.check_strict_mode()`

## Incremental-Encoder · [Endpoints/Incremental-Encoder.md](Endpoints/Incremental-Encoder.md) · ep-config section `IE`

- `<IEx> Stop` · `ep.<IEx>.stop()`
- `<IEx> Set` · `ep.<IEx>.set(PulseRate, SymmetryError="0deg", PhaseError="0deg")`

## Memory-Mapped-I2C · [Endpoints/MEMORY-MAPPED-I2C.md](Endpoints/MEMORY-MAPPED-I2C.md) · ep-config section `MEMORY_MAPPED_I2C`

- `<MEMORY_MAPPED_I2Cx> Reset` · `ep.<MEMORY_MAPPED_I2Cx>.reset()`
- `<MEMORY_MAPPED_I2Cx> Set Register` · `ep.<MEMORY_MAPPED_I2Cx>.set_register(register, value)`
- `<MEMORY_MAPPED_I2Cx> Read Register` · `ep.<MEMORY_MAPPED_I2Cx>.read_register(address)`
- `<MEMORY_MAPPED_I2Cx> Check Strict Mode` · `ep.<MEMORY_MAPPED_I2Cx>.check_strict_mode()`

## Memory-Mapped-SPI · [Endpoints/MEMORY-MAPPED-SPI.md](Endpoints/MEMORY-MAPPED-SPI.md) · ep-config section `MEMORY_MAPPED_SPI`

- `<MEMORY_MAPPED_SPIx> Reset` · `ep.<MEMORY_MAPPED_SPIx>.reset()`
- `<MEMORY_MAPPED_SPIx> Set Register` · `ep.<MEMORY_MAPPED_SPIx>.set_register(address, value)`
- `<MEMORY_MAPPED_SPIx> Read Register` · `ep.<MEMORY_MAPPED_SPIx>.read_register(address)`

## Pin-Follow · [Endpoints/Pin-Follow.md](Endpoints/Pin-Follow.md) · ep-config section `PIN_FOLLOW`

- `<PIN_FOLLOWx> Enable` · `ep.<PIN_FOLLOWx>.enable(Delay, Inverted=False)`
- `<PIN_FOLLOWx> Disable` · `ep.<PIN_FOLLOWx>.disable()`
- `<PIN_FOLLOWx> Disable Automatic Strict Mode Check` · `ep.<PIN_FOLLOWx>.disable_automatic_strict_mode_check()`
- `<PIN_FOLLOWx> Check Strict Mode` · `ep.<PIN_FOLLOWx>.check_strict_mode()`

## PWM · [Endpoints/PWM.md](Endpoints/PWM.md) · ep-config section `PWM`

- `<PWMx> Start Measuring` · `ep.<PWMx>.start_measuring(DefaultState="Low")`
- `<PWMx> Stop Measuring` · `ep.<PWMx>.stop_measuring()`
- `<PWMx> Get Frequency` · `ep.<PWMx>.get_frequency(ReturnType="Hz", MaxAllowedDeviation="5%")`
- `<PWMx> Get Duty Cycle` · `ep.<PWMx>.get_duty_cycle(MaxAllowedDeviation="5%")`
- `<PWMx> Get Frequency And Deviations` · `ep.<PWMx>.get_frequency_and_deviations(ReturnType="Hz")`
- `<PWMx> Get Duty Cycle And Deviations` · `ep.<PWMx>.get_duty_cycle_and_deviations()`
- `<PWMx> Get Pulse Count` · `ep.<PWMx>.get_pulse_count()`
- `<PWMx> Get First Change From Default State` · `ep.<PWMx>.get_first_change_from_default_state()`
- `<PWMx> Get Last Change To Default State` · `ep.<PWMx>.get_last_change_to_default_state()`
- `<PWMx> Get Min And Max Dead Time 1` · `ep.<PWMx>.get_min_and_max_dead_time_1(ReturnType="s")`
- `<PWMx> Get Min And Max Dead Time 2` · `ep.<PWMx>.get_min_and_max_dead_time_2(ReturnType="s")`

## SPI-Slave · [Endpoints/SPI-Slave.md](Endpoints/SPI-Slave.md) · ep-config section `SPI`

- `<SPIx> Reset` · `ep.<SPIx>.reset()`
- `<SPIx> Set Mode` · `ep.<SPIx>.set_mode(CPOL, CPHA)`
- `<SPIx> Set Response Bytes` · `ep.<SPIx>.set_response_bytes(data: ByteData)`
- `<SPIx> Set Chip Select Use To` · `ep.<SPIx>.set_chip_select_use_to(CS)`
- `<SPIx> Disable Automatic Strict Mode Check` · `ep.<SPIx>.disable_automatic_strict_mode_check()`
- `<SPIx> Read Written Bytes` · `ep.<SPIx>.read_written_bytes()`
- `<SPIx> Read Minimal Clock Period` · `ep.<SPIx>.read_minimal_clock_period(ReturnType="s")`
- `<SPIx> Read Maximal Clock Period` · `ep.<SPIx>.read_maximal_clock_period(ReturnType="s")`
- `<SPIx> Check Strict Mode` · `ep.<SPIx>.check_strict_mode()`

## System · [Endpoints/System.md](Endpoints/System.md) · needs the System licence, no ep-config section

- `SYSTEM Start Model` · `ep.SYSTEM.start_model(file)`
- `SYSTEM Disable Automatic Strict Mode Check` · `ep.SYSTEM.disable_automatic_strict_mode_check()`
- `SYSTEM Stop Model` · `ep.SYSTEM.stop_model()`
- `SYSTEM Set Input` · `ep.SYSTEM.set_input(variable, value)`
- `SYSTEM Check Strict Mode` · `ep.SYSTEM.check_strict_mode()`
- `SYSTEM Get Output` · `ep.SYSTEM.get_output(variable)`

## UART · [Endpoints/UART.md](Endpoints/UART.md) · ep-config section `UART`

- `<UARTx> Disable Automatic Strict Mode Check` · `ep.<UARTx>.disable_automatic_strict_mode_check()`
- `<UARTx> Start` · `ep.<UARTx>.start(Bitrate, Parity, StopBits, DataBits)`
- `<UARTx> Stop` · `ep.<UARTx>.stop()`
- `<UARTx> Enable Loopback` · `ep.<UARTx>.enable_loopback()`
- `<UARTx> Transmit` · `ep.<UARTx>.transmit(Data: ByteData)`
- `<UARTx> Transmit Periodically` · `ep.<UARTx>.transmit_periodically(Data: ByteData, Count=None, Pause="0ms", WaitForCompletion=True)`
- `<UARTx> Transmit With Different Setup` · `ep.<UARTx>.transmit_with_different_setup(Data: ByteData, Bitrate=None, Parity=None, StopBits=None, DataBits=None)`
- `<UARTx> Check Strict Mode` · `ep.<UARTx>.check_strict_mode()`
- `<UARTx> Get Received` · `ep.<UARTx>.get_received()`
- `<UARTx> Get Received With Details` · `ep.<UARTx>.get_received_with_details()`
- `<UARTx> Wait For Received Buffer` · `ep.<UARTx>.wait_for_received_buffer(ExpectedCount, Timeout="100ms")`
