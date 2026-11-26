# Version Compatibility
All ExecutionPlatform components share one version number: `<major>.<minor>.<patch>`.

The ExecutionPlatform checks compatibility automatically and refuses to run tests when components do not fit together.

**`<major`> and `<minor>`> must match**. The `<patch>` is not regarded.

So an EP running `9.5.2` works with components from `9.5.0`, `9.5.1` and `9.5.2`, but not with anything from `9.4.x` or `10.0.x`.

## Components

| Component | Where it comes from | Checked against EP |
| --- | --- | --- |
| ExecutionPlatform | The device itself, updated via [System update](Service.md#system-update). This is the reference version all other components are compared to. | — |
| `embeff-ep` Python package (`EPLibrary`, pytest plugin) | `requirements.txt` in the [Software Package](SoftwarePackage.md) | On connect |
| Runtime / BSP | `src/` in the [Software Package](SoftwarePackage.md), compiled into the DUT binary | When the DUT binary is flashed |
| [`.ep-config.zip`](EP_Config.md) | Built in the [customer portal](https://customer.embeff.com) for a selected EP version | On connect |

You find the version of your device at `http://<ep-serial-number or ip>`.

## After a system update
A bugfix update (for example `9.5.1` → `9.5.2`) needs no action on your side.

After a major or minor update (for example `9.4.x` → `9.5.0`):

- Download the matching [Software Package](GettingStarted.md#downloads-in-customer-portal) from the [customer portal](https://customer.embeff.com) and update your local copy. At a minimum `src/` and `app/` must be updated. Re-compile your DUT binaries so they contain the new runtime.
- [Re-create](EP_Config.md#creating-a-new-configuration) your `.ep-config.zip` files for the new version and distribute them to your test sequences.
