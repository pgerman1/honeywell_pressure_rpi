use honeywell_pressure::{DriverError, PressureSensor};

fn main() -> Result<(), DriverError> {
    let mut sensor = PressureSensor::new(1, 0x28)?;

    let pressure = sensor.read_pressure_mbar()?;
    println!("Pressure: {pressure:.3} mbar");

    Ok(())
}