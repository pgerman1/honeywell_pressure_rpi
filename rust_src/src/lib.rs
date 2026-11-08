/*
 Honeywell 0-100 mbar sensor calibration and conversion utilities.
*/

pub struct Calibration {
    counts_min: u16,
    counts_max: u16,
    pressure_min_mbar: f32,
    pressure_max_mbar: f32
}

/// Default calibration for the 0-100 mbar sensor configuration
impl Default for Calibration {
    fn default() -> Self {
        Calibration {
            counts_min: 0x0666,
            counts_max: 0x3999,
            pressure_min_mbar: 0.0,
            pressure_max_mbar: 100.0
        }
    }
}

/**
    Implementation of the Calibration struct, including conversion methods.

    Provides methods to convert raw sensor counts to pressure in mbar and mmHg.
    Conversion methods include:
    - counts_to_pressure_mbar: Converts raw sensor counts to pressure in mbar.
    - counts_to_pressure_mmhg: Converts raw sensor counts to pressure in mmHg.
    */
impl Calibration {

    /// Conversion factor from mbar to mmHg
    const MBAR_TO_MMHG: f32 = 0.750062;

    /// Converts a raw sensor count to pressure in mbar using the calibration.
    pub fn counts_to_pressure_mbar(&self, counts: u16) -> f32 {
        let counts = f32 :: from (counts);
        let counts_min = f32::from(self.counts_min);
        let counts_max = f32::from(self.counts_max);

        let fraction = (counts - counts_min) / (counts_max - counts_min);
        self.pressure_min_mbar + fraction * (self.pressure_max_mbar - self.pressure_min_mbar)
    }
    pub fn counts_to_pressure_mmhg(&self, counts: u16) -> f32 {

        self.counts_to_pressure_mbar(counts) * Self::MBAR_TO_MMHG
    }
}

/// Errors related to the sensor status in the first byte of the sensor response
#[derive(Debug, PartialEq, Eq)]
pub enum SensorStatusError {
    CommandMode,
    StaleData,
    DiagnosticCondition
}

/// Decode two sensor bytes, supplied in wire order, into a raw sensor count.
pub fn decode_counts(bytes: [u8; 2]) -> Result<u16, SensorStatusError> {
    
    // grab the status bits from the first byte (bits 7 and 6)
    let status = bytes[0] >> 6;
 
    // check the status bits and return an error if necessary
    match status {
        0b00 => {},
        0b01 => return Err(SensorStatusError::CommandMode),
        0b10 => return Err(SensorStatusError::StaleData),
        0b11 => return Err(SensorStatusError::DiagnosticCondition),
        _ => unreachable!(),
    }

    Ok(((bytes[0] as u16 & 0x3F) << 8) | bytes[1] as u16)
}

/*----------------------------------------------------------------------------


----------------------------------------------------------------------------*/
use rppal::i2c::{self, I2c};

/// Errors that can occur when interacting with the pressure sensor driver.
/// I2C Error - Occurs when there is an error communicating over the I2C bus.
/// Sensor Status Error - Occurs when the sensor returns an error status.
/// Short Read - Occurs when fewer bytes than expected are read from the sensor.

#[derive(Debug)]
pub enum DriverError{
    I2cError(i2c::Error),
    SensorStatusError(SensorStatusError),
    ShortRead {received: usize},
}

/// Pressure sensor driver structure containing the I2C interface and calibration data. 
pub struct PressureSensor {
    i2c: I2c,
    calibration: Calibration,
}

/// Implementation of the PressureSensor driver methods.
impl PressureSensor {
    
    /// Create a new PressureSensor instance with the specified I2C bus and address.
    pub fn new(bus: u8, address: u16) -> Result<Self, DriverError> {
        let mut i2c = I2c::with_bus(bus).map_err(DriverError::I2cError)?;
        
        i2c.set_slave_address(address).map_err(DriverError::I2cError)?;

        Ok(Self {
            i2c,
            calibration: Calibration::default(),
        })  
    }

    // Read I2C sensor data and decode it into raw counts.
    pub fn read_counts(&mut self) -> Result<u16, DriverError> {
        let mut bytes = [0_u8; 2];

        let received = self.i2c.read(&mut bytes).map_err(DriverError::I2cError)?;
        if received != bytes.len() {
            return Err(DriverError::ShortRead { received });
        }
        decode_counts(bytes).map_err(DriverError::SensorStatusError)
    }

    /// Read the raw pressure counts and convert them to pressure in mbar.
    pub fn read_pressure_mbar(&mut self) -> Result<f32, DriverError> {
        let counts = self.read_counts()?;
        Ok(self.calibration.counts_to_pressure_mbar(counts))        
    }

    /// Read the raw pressure counts and convert them to pressure in mmHg.
    pub fn read_pressure_mmhg(&mut self) -> Result<f32, DriverError> {
        let counts = self.read_counts()?;
        Ok(self.calibration.counts_to_pressure_mmhg(counts))        
    }


}

/*----------------------------------------------------------------------------
    Unit tests for the Calibration struct
    - Tests for the default 0-100 mbar sensor calibration
    - Tests for conversion from counts to mbar and mmHg
    - Tests for counts below minimum and above maximum
    - Tests for conversion consistency between mbar and mmHg
    - Tests for edge cases with counts just below minimum and just above maximum
    - Tests for intermediate counts within the valid range
----------------------------------------------------------------------------*/

#[cfg(test)]
mod calc_tests {
    /// Unit tests for the decode_counts function and sensor status errors.
    use super::{Calibration};

    #[test]
    /// Test that the calibration endpoints match the pressure range in mbar.
    fn calibration_endpoints_match_pressure_range() {
        let calibration = Calibration::default();
        assert!( (calibration.counts_to_pressure_mbar(calibration.counts_min) - calibration.pressure_min_mbar).abs() < f32::EPSILON );
        assert!( (calibration.counts_to_pressure_mbar(calibration.counts_max) - calibration.pressure_max_mbar).abs() < f32::EPSILON );
         }

    #[test]
    /// Test that the maximum pressure matches the counts_max value.
    fn maximum_pressure_matches_counts_max() {
        let calibration = Calibration::default();
        assert!( (calibration.counts_to_pressure_mbar(calibration.counts_max) - calibration.pressure_max_mbar).abs() < f32::EPSILON );
    }
    #[test]
    /// Test that the minimum pressure matches the counts_min value.
    fn minimum_pressure_matches_counts_min() {
        let calibration = Calibration::default();
        assert!( (calibration.counts_to_pressure_mbar(calibration.counts_min) - calibration.pressure_min_mbar).abs() < f32::EPSILON );
    }
    #[test]
    /// Test that the maximum pressure in mmHg matches the counts_max value.
    fn maximum_pressure_matches_counts_max_mmhg() {
        let calibration = Calibration::default();
        assert!( (calibration.counts_to_pressure_mmhg(calibration.counts_max) - calibration.pressure_max_mbar * Calibration::MBAR_TO_MMHG).abs() < f32::EPSILON );
    }
    #[test]
    /// Test that the minimum pressure in mmHg matches the counts_min value.
    fn minimum_pressure_matches_counts_min_mmhg() {
        let calibration = Calibration::default();
        assert!( (calibration.counts_to_pressure_mmhg(calibration.counts_min) - calibration.pressure_min_mbar * Calibration::MBAR_TO_MMHG).abs() < f32::EPSILON );
    }
    #[test]
    /// Test that the conversion from mbar to mmHg is consistent.
    fn conversion_mbar_to_mmhg() {
        let calibration = Calibration::default();
        let pressure_mbar = calibration.counts_to_pressure_mbar(calibration.counts_min);
        let pressure_mmhg = calibration.counts_to_pressure_mmhg(calibration.counts_min);
        assert!( (pressure_mmhg - pressure_mbar * Calibration::MBAR_TO_MMHG).abs() < f32::EPSILON );
    }
    #[test]
    /// Test that the conversion from mmHg to mbar is consistent.
    fn conversion_mmhg_to_mbar() {
        let calibration = Calibration::default();
        let pressure_mbar = calibration.counts_to_pressure_mbar(calibration.counts_min);
        let pressure_mmhg = calibration.counts_to_pressure_mmhg(calibration.counts_min);
        assert!( (pressure_mbar - pressure_mmhg / Calibration::MBAR_TO_MMHG).abs() < f32::EPSILON );
    }
    #[test]
    fn interior_count_converts_to_expected_pressure(){
        let calibration = Calibration::default();

        // 4915 is 25% of the way between counts_min and counts_max, the ideal value is
        // 4914.75, so allow rounding to the nearest integer.
        let actual = calibration.counts_to_pressure_mbar(4915);

        assert!((actual - 25.0).abs() < 0.01, "got {actual} mbar");
    }
    
    #[test]
    // test that counts below min produce negative pressures
    fn counts_below_min_produce_negative_pressure() {
        let calibration = Calibration::default();
        let actual = calibration.counts_to_pressure_mbar(calibration.counts_min - 1);
        assert!(actual < 0.0, "got {actual} mbar");
    }
    #[test]
    // test that counts above max produce pressures greater than the maximum
    fn counts_above_max_produce_high_pressure() {
        let calibration = Calibration::default();
        let actual = calibration.counts_to_pressure_mbar(calibration.counts_max + 1);
        assert!(actual > calibration.pressure_max_mbar, "got {actual} mbar");
    }

}


#[cfg(test)]
mod range_tests {
    use super::{Calibration, decode_counts};

    /// Test that the conversion from counts to pressure works for the maximum range.
    #[test]
    fn decodes_max_range() {
        let calibration = Calibration::default();
        let counts = decode_counts([0x39, 0x99]).unwrap();
        let pressure = calibration.counts_to_pressure_mbar(counts);

        assert_eq!(pressure, 100.0);
    }

    /// Test that the conversion from counts to pressure works for the minimum range.
    #[test]
    fn decodes_min_range() {
        let calibration = Calibration::default();
        let counts = decode_counts([0x06, 0x66]).unwrap();
        let pressure = calibration.counts_to_pressure_mbar(counts);
        assert_eq!(pressure, 0.0);
    }
}

#[cfg(test)]
mod decode_counts_tests {
    use super::{decode_counts};
    #[test]
    fn decodes_normal_pressure_counts() {
        assert_eq!(decode_counts([0x06, 0x66]), Ok(1638));
        assert_eq!(decode_counts([0x39, 0x99]), Ok(14745));
    }

    #[test]
    fn preserves_high_byte_first_order() {
        assert_eq!(decode_counts([0x12, 0x34]), Ok(0x1234));
    }

    #[test]
    fn decodes_pressure_field_boundaries() {
        assert_eq!(decode_counts([0x00, 0x00]), Ok(0));
        assert_eq!(decode_counts([0x3F, 0xFF]), Ok(16383));
    }

    #[test]
    fn decodes_zero_counts() {
        assert_eq!(decode_counts([0x00, 0x00]), Ok(0));
    }
}

#[cfg(test)]
mod decode_counts_error_tests {
    use super::{decode_counts, SensorStatusError};
    #[test]
    fn rejects_command_mode() {
        assert_eq!(
            decode_counts([0x46, 0x66]),
            Err(SensorStatusError::CommandMode)
        );
    }

    #[test]
    fn rejects_stale_data() {
        assert_eq!(
            decode_counts([0x86, 0x66]),
            Err(SensorStatusError::StaleData)
        );
    }

    #[test]
    fn rejects_diagnostic_condition() {
        assert_eq!(
            decode_counts([0xC6, 0x66]),
            Err(SensorStatusError::DiagnosticCondition)
        );
    }
}