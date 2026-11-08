use honeywell_pressure::{DriverError, PressureSensor};

fn main() -> Result<(), DriverError> {

    // Initialize the pressure sensor on I2C bus 1 with address 0x28
    let mut sensor = PressureSensor::new(1, 0x28)?;

    // Read and print the pressure 10 times
    for _ in 0..10 {
        let pressure = sensor.read_pressure_mbar()?;
        println!("Pressure: {pressure:.3} mbar");
    }

    // End of pressure reading loops
    Ok(())
}