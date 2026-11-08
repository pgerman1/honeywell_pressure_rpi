# Honeywell ABPD Pressure Transducer Driver — Rust

[Back to repository overview](../readme.md)

The `honeywell_pressure` library owns its I²C connection and returns `Result` values for communication failures, incomplete reads, and non-normal sensor status. It reads two bytes in wire order, separates the status bits from the 14-bit pressure field, and converts counts to mbar or mmHg.

The current calibration is fixed to the development sensor's 0–100 mbar range, using `0x0666` and `0x3999` as its minimum and maximum calibrated counts. Other sensor ranges require changes to the calibration. Bus and address are configurable when constructing the driver.

## Requirements

- A current stable Rust toolchain and Cargo; [rustup](https://rust-lang.org/tools/install/) is the recommended installation method. Rust 1.63 is too old for the current dependencies.
- The Rust dependency `rppal = "0.22.1"`, downloaded automatically by Cargo. The Rust implementation does not require pigpio.
- For hardware reads: a Raspberry Pi with I²C enabled, a connected and powered sensor, and permission to access the selected `/dev/i2c-*` device.

## Build and test

Run the following commands from the `rust_src` directory:

```bash
cargo test --locked
cargo build --locked --example read_pressure
```

The 19 unit tests cover calibration and byte decoding without accessing sensor hardware. Passing them does not verify the physical I²C connection.

## Run on the Raspberry Pi

The [read_pressure example](examples/read_pressure.rs) uses bus **1** and address **0x28**. Adjust these arguments if your setup differs.

```bash
cargo run --locked --example read_pressure
```

A hardware run on the development Pi successfully produced:

```text
Pressure: 0.214 mbar
```

This demonstrates a successful read, not a calibration or accuracy measurement. The current example attempts ten consecutive readings without a delay and exits on the first error, including stale data. It does not yet implement polling delays or retries.

## Library usage

```rust
use honeywell_pressure::{DriverError, PressureSensor};

fn main() -> Result<(), DriverError> {
    let mut sensor = PressureSensor::new(1, 0x28)?;
    let pressure = sensor.read_pressure_mbar()?;
    println!("Pressure: {pressure:.3} mbar");
    Ok(())
}
```

The driver provides `read_counts()`, `read_pressure_mbar()`, and `read_pressure_mmhg()`. Each method performs a new sensor read. To express the same sample in both units, read counts once and use `Calibration::default()` with `counts_to_pressure_mbar()` and `counts_to_pressure_mmhg()`.

Errors distinguish I²C failures (`DriverError::I2cError`), short reads (`DriverError::ShortRead`), and sensor status (`DriverError::SensorStatusError`). Sensor status errors include command mode, stale data, and diagnostic conditions. Stale data means the sample is not fresh; callers can wait before attempting another read.

## Cross-compilation

You can also build on a workstation or build server using [cross](https://github.com/cross-rs/cross), with Docker or Podman available. Select a target matching the Pi's CPU and installed operating system:

| Pi environment | Rust target |
| --- | --- |
| 64-bit Raspberry Pi OS | `aarch64-unknown-linux-gnu` |
| 32-bit Raspberry Pi OS on an ARMv7-capable or newer Pi | `armv7-unknown-linux-gnueabihf` |
| Original ARMv6 Pi or Pi Zero | `arm-unknown-linux-gnueabihf` |

For example, from `rust_src`, build for a 64-bit Pi:

```bash
cargo install cross --locked
cargo test --locked
cross build --locked --release --target aarch64-unknown-linux-gnu --example read_pressure
```

Copy `target/aarch64-unknown-linux-gnu/release/examples/read_pressure` to the Pi and run it with `./read_pressure`. Rust is not required on the Pi to run the compiled executable. The target system libraries must also be compatible with the build environment. Cross-compilation has not yet been verified on the development hardware.
