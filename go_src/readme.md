# Honeywell ABPD Pressure Transducer Driver - Go Implementation

Go example for the Honeywell ABPDLNN100MG2A3 0–100 mbar pressure sensor on a Raspberry Pi, using `golang.org/x/exp/io/i2c`.

## Build and run

Enable I²C on the Pi and ensure your user can access `/dev/i2c-1`. Connect the sensor to bus 1 at its factory address, `0x28`. Install Go compatible with the module's Go 1.19 declaration.

From the repository root, build and run on the Pi:

```bash
cd go_src
go build -o pressure pressure.go && ./pressure
```

Build `pressure.go` explicitly: `test.go` is an unfinished legacy example with another `main()` and an unresolved `Transducer` reference, so a package-wide `go build` is not supported. The `&&` prevents running an old executable when compilation fails.

To cross-compile for the original Raspberry Pi Zero (ARMv6) from `go_src`:

```bash
GOOS=linux GOARCH=arm GOARM=6 go build -o pressure pressure.go
```

Copy the executable to the Pi and run `./pressure`. Use the appropriate architecture for other Pi models.

## Configuration

`newSensor()` applies these defaults before opening the I²C connection:

| Setting | Default |
| --- | --- |
| `bus` | `1` (`/dev/i2c-1`) |
| `address` | `0x28` |
| `minCounts` | `0x0666` (1638) |
| `maxCounts` | `0x3999` (14745) |
| `pMin` | 0 mbar |
| `pMax` | 100 mbar |

`initialize()` derives the device path from `sensor.bus` and uses `sensor.address`. Change the defaults or the setters in `newSensor()` before `initialize()` runs. Changing these fields after initialization does not reopen the connection.

## Output and status handling

The example attempts 100 reads, waiting 100 ms after each attempt. Each read receives two bytes in one transaction, without a register-address write. It combines them in wire order, extracts the top two status bits, and masks the remaining 14 bits as pressure counts.

Example output for a normal sample:

```text
Sensor Bus : 1 (/dev/i2c-1), Address : 0x28
raw=0x0682 status=00 counts=1666
Pressure : 0.160 mmHg
```

The raw word is hexadecimal; counts are decimal; the pressure line is converted mmHg from that same sample, without a second read.

| Status | Meaning | Example behavior |
| --- | --- | --- |
| `00` | Normal data | Convert and print pressure |
| `01` | Command mode | Skip sample |
| `10` | Stale data | Skip sample |
| `11` | Diagnostic condition | Skip sample |

For example, `0x867b` contains status `10` and pressure counts `0x067b` (1659). Do not interpret the status bits as pressure or accept a stale sample merely by masking them away. The loop reports the number of skipped samples at the end; it does not classify high pressure counts as communication errors. Bus-open and I²C read failures currently cause a panic.

## Methods

- `read_counts() int`: logs raw data, status, and counts; returns counts for normal status or `-1` otherwise.
- `read_mbar() float32`: performs a read and returns pressure in mbar, or `NaN` for non-normal status.
- `read_mmhg() float32`: performs a read and returns pressure in mmHg, or `NaN` for non-normal status.
- `counts2mbar(counts int) float32`: converts valid counts using the configured calibration. Callers must reject the `-1` sentinel before using it.

The conversion is:

```text
mbar = (counts - minCounts) * (pMax - pMin) / (maxCounts - minCounts) + pMin
mmHg = mbar * 0.7500
```

Each read method takes a new sample. To express one sample in multiple units, call `read_counts()` once, check its result, and convert that result.

## Hardware check

A supplied Pi run with the corrected two-byte read contained 100 normal-status samples, no reported I²C errors, and counts from 1662 to 1674. This confirms that run's status decoding and read behavior; it is not a calibration or accuracy measurement. Re-run on the Pi after changes to verify the displayed bus and converted pressure.
