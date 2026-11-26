# Model capabilities

The functions a Python model ([System endpoint](Endpoints/System.md)) can use, per endpoint type. Endpoint types not listed here are not reachable from a model. Each heading names the page that documents the functions and the ep-config section that enables the endpoint type.

- A model reaches an instance by its name in the ep-config: for `GPIO: {GPIO1: {...}}` it calls `self.GPIO1.set_high()`. The System endpoint is `self.SYSTEM`.
- Functions starting with `on_` register a callback that the endpoint calls on an event.

## Analog-Out · [Endpoints/Analog-Out.md](Endpoints/Analog-Out.md) · ep-config section `ANOUT`

- `set_static(voltage)`
- `reset()`

## CAN · [Endpoints/CAN.md](Endpoints/CAN.md) · ep-config section `CAN`

- `send_frame(Id: int | str, Data: bytes | str, Extended=True, StoreInBuffer=True) -> bool`
- `on_frame_received(callback: Callable[[int, bytes], None])`

## CAN-FD · [Endpoints/CAN-FD.md](Endpoints/CAN-FD.md) · ep-config section `CAN_FD`

- `send_frame(Id: int | str, Data: bytes | str, Extended=True, FrameMode: str='FD_BitrateSwitch', StoreInBuffer=True) -> bool`
- `on_frame_received(callback: Callable[[int, bytes], None])`

## GPIO · [Endpoints/GPIO.md](Endpoints/GPIO.md) · ep-config section `GPIO`

- `set_high()`
- `set_low()`
- `set_tristate()`
- `read_as_bool()`
- `on_rising(callback: Callable[[], None])`
- `on_falling(callback: Callable[[], None])`

## I2C-Target · [Endpoints/I2C-Target.md](Endpoints/I2C-Target.md) · ep-config section `I2C_TARGET`

- `on_write_transfer(callback: Callable[[int, bytes], None])`
- `on_read_transfer(callback: Callable[[int, int], None])`

## Memory-Mapped-I2C · [Endpoints/MEMORY-MAPPED-I2C.md](Endpoints/MEMORY-MAPPED-I2C.md) · ep-config section `MEMORY_MAPPED_I2C`

- `on_register_set(callback: Callable[[int, int], None])`

## System · [Endpoints/System.md](Endpoints/System.md) · needs the System licence, no ep-config section

- `on_tick(interval, callback: Callable[[], None])`

## UART · [Endpoints/UART.md](Endpoints/UART.md) · ep-config section `UART`

- `on_bytes_received(callback: Callable[[bytes], None])`
