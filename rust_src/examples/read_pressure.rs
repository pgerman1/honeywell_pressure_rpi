use honeywell_pressure::{DriverError, PressureSensor};
use std::{thread, time::Duration};

const READ_INTERVAL: Duration = Duration::from_millis(100);

fn main() -> Result<(), DriverError> {

    // Initialize the pressure sensor on I2C bus 1 with address 0x28
    let mut sensor = PressureSensor::new(1, 0x28)?;

    // Read 10 samples at approximately 10 Hz, allowing fresh data between reads.
    for sample in 0..10 {
        if sample > 0 {
            thread::sleep(READ_INTERVAL);
        }
        let pressure = sensor.read_pressure_mbar()?;
        println!("Pressure: {pressure:.3} mbar");
    }

    // End of pressure reading loops
    Ok(())
}
