# Configurations
An ep-config determines which Endpoints are available and what DUT pins are used for it. Each test suite may use a different configuration.
A configuration is a file with the extension `.ep-config.zip`. You may use an unlimited number of different configurations.

:warning: It is currently forbidden to rename a .ep-config.zip file.

A configuration is defined in YAML. The available configuration settings for each endpoint are listed in the respective endpoint documentation. For a starting point, we provide an example configuration for each DUT that is using all available digital pins as `GPIO` endpoints. You find this configuration in the [customer portal](EP_Config#creating-a-new-configuration).

Each configuration must contain a `System` block unless you are writing system tests for firmware that does not include the EP runtime package. It defines how the ExecutionPlatform is communicating with the DUT at runtime.

- **Integration test mode** (System block present): the EP communicates with the DUT firmware via UART. `invoke` and the other functions of the [Code Interface](CodeInterface.md) that talk to the DUT are available, and its `flash_firmware` and `power_on` wait until the firmware reaches `ep_process_loop`.
- **System test mode** (System block omitted): no UART communication with the DUT. Neither `invoke` nor the other functions of the [Code Interface](CodeInterface.md) that talk to the DUT are available and the wait for `ep_process_loop` is skipped.

The following block shows a typical UART based configuration in **integration test mode**. The DUT transmits serial data at pin 101 and receives it at pin 102 with a baudrate of 115200 bits/s. This definition must match the [UART settings in your BSP](SoftwarePackage?id=add-your-own-bsp).
```
System:
  UART:
    DUT_TX: 101
    DUT_RX: 102
    Baudrate: 115200
```


:information_source: To see the specific configuration for a `.ep-config.zip` file, you may extract it with a tool such as [7-Zip](https://www.7-zip.org/) and open the `config.yaml` file. This file is read-only. If you want to change the configuration, look [below](#creating-a-new-configuration).

## Schema for configuration file
Our [software package](GettingStarted#downloads-in-customer-portal) contains jsonschema for checking configuration, it is located at `.vscode/ep-config.schema.json`.
The schema provides auto-completion and validation for writing configurations.

### VSCode
Requirements:
- [redhat.vscode-yaml](https://marketplace.visualstudio.com/items?itemName=redhat.vscode-yaml) extension

Schema is automatically enabled for all files ending with the `.ep-config.yaml` extension.

:information_source: For Analyzer outputs defined as numbers schema will issue an error. This issue can be fixed by defining Analyzer outputs as strings:
```
Analyzer:
  Output:
    "0": SYSTEM.TEST_RUNNING
    "1": PA0.Pin
    "2": CAN1.RX
    "3": CAN1.TX
    "4": SPI1.SCK
    "5": SPI1.MISO
    "6": SPI1.MOSI
    "7": PB2.Pin
```

## Example configuration
```
System:
  UART:
    DUT_TX: 101
    DUT_RX: 102
    Baudrate: 115200

GPIO:
  PA0:
    Pin: 34
    Pull: None
  PB1:
    Pin: 47
    Pull: None
  PB2:
    Pin: 48
    Pull: None

SPI:
  SPI1:
    SCK: 35
    MISO: 42
    MOSI: 43
    nCS0: 40

CAN:
  CAN1:
    TX: 20
    RX: 19

UART:
  UART2:
    DUT_RX: 122
    DUT_TX: 119

ANOUT:
  PA1_ADC:
    Pin: 37
  PA2_ADC:
    Pin: 38

Analyzer:
  Output:
    "0": SYSTEM.TEST_RUNNING
    "1": PA0.Pin
    "2": CAN1.RX
    "3": CAN1.TX
    "4": SPI1.SCK
    "5": SPI1.MISO
    "6": SPI1.MOSI
    "7": PB2.Pin
```

# Creating a new configuration

1. Open https://customer.embeff.com/ and log-in.
2. Open the `Configurations` tab and click on `Add new`.
3. Enter a name of your choice to easily identify the configuration later:
    - The name is the Python package name of the configuration. pytest uses `import <name>.test`, Robot Framework loads `<name>.ep-config.zip` with `EPLibrary`, and models use `import <name> as config`.
    - The name has 1 to 40 characters. Use only letters, digits and `_`. The first character must be a letter, and Python keywords such as `class` are not allowed.
4. Select the DUT you want to use the configuration with. This selection decides which package pins are available for you to use.
5. Enter the configuration in a valid YAML format.
    - Look in the sample configuration (`gpio`) for a starting point which contains all available digital pins.
    - Look in the documentation for the required Endpoint to find information about its configuration options.
6. At `Built for versions` select the EP Software versions you want to use the configuration with.
7. Click on `Add` to start building the configuration. The building takes place in the background and typically takes between 10 and 30 minutes to complete.
7. Open the `Configuration Download` tab. Note that the latest EP Software version is the default for the version filter.
8. Select all configurations and click download. Extract the downloaded zip file. Be aware that it is currently forbidden to rename individual `.ep-config.zip` files. However you can freely change their directory. Use the `.ep-config.zip` files from your test sequences.

# Command line usage (`ep-config`)
The Python package [embeff-ep](https://github.com/embeff/embeff-ep) includes `ep-config`, a command line tool covering the same `validate`/`build` workflow as the portal UI, for use in automation (CI, scripts) or by AI agents. It does not create or change anything in the `Configurations` tab - builds triggered this way are one-off and not saved to your account.

`validate` and `build` need an API secret: open `API access` from the settings menu (top right) in the [customer portal](https://customer.embeff.com), generate a secret, and pass it via the `EP_CONFIG_SECRET` environment variable (preferred) or `--secret`.

```
export EP_CONFIG_SECRET=<secret from the portal>

# Check that a configuration is valid for a DUT/EP version:
ep-config validate --dut <DUT_SKU> --version <VERSION> my_config.ep-config.yaml

# Build it and download the result. Blocks until the build farm finishes (typically 10 to 30 minutes):
ep-config build --dut <DUT_SKU> --version <VERSION> --output-folder output/ my_config.ep-config.yaml

# Inspect an existing .ep-config.zip file (offline, no secret needed):
ep-config info my_config.ep-config.zip
```

`build` names the resulting ep-config after the input configuration and writes it into `--output-folder`.
Both `my_config.ep-config.yaml` and `my_config.yaml` become `my_config.ep-config.zip`.
The name follows the same rules as in the portal, see [Creating a new configuration](#creating-a-new-configuration).

# How to store ep-config files?
Configurations are binary files with a size of about 4 MB.
For each (non-bugfix) ExecutionPlatform update, the `ep-config` file changes as well.

We therefore recommend to

- commit the YAML file next to your test sequences.
- use a shared drive or binary artifact manager to store and distribute the `ep-config` files.

This way you have full traceability and version control. If a test file needs a change n the `ep-config`, for example a new Endpoint, you can commit both files together.
