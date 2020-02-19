#include "MPU9250_Acc_Gyro.h"

MPU9250_Acc_Gyro::MPU9250_Acc_Gyro(const char * i2cDeviceFilePath) : i2cObject(i2cDeviceFilePath) {
  i2cObject.addressSet(MPU9250_ADDRESS);
}

void MPU9250_Acc_Gyro::begin(void) {

  i2cObject.writeByte(PWR_MGMT_1, 0x00); // Clear sleep mode bit (6), enable all sensors
  i2cObject.writeByte(PWR_MGMT_1, 0x01); // Auto select clock source to be PLL gyroscope reference if ready else

  // Configure Gyro and Thermometer
  // Disable FSYNC and set thermometer and gyro bandwidth to 41 and 42 Hz, respectively;
  // minimum delay time for this setting is 5.9 ms, which means sensor fusion update rates cannot
  // be higher than 1 / 0.0059 = 170 Hz
  // DLPF_CFG = bits 2:0 = 011; this limits the sample rate to 1000 Hz for both
  // With the MPU9250, it is possible to get gyro sample rates of 32 kHz (!), 8 kHz, or 1 kHz
  i2cObject.writeByte(CONFIG, 0x03);

  // Set sample rate = gyroscope output rate/(1 + SMPLRT_DIV)
  i2cObject.writeByte(SMPLRT_DIV, 0x04);  // Use a 200 Hz rate; a rate consistent with the filter update rate

  // Set gyroscope full scale range
  uint8_t c = i2cObject.readByte(GYRO_CONFIG); // get current GYRO_CONFIG register value
  c = c & ~0x03; // Clear Fchoice bits [1:0]
  c = c & ~0x18; // Clear GFS bits [4:3]
  c = c | Gscale << 3; // Set full scale range for the gyro
  i2cObject.writeByte( GYRO_CONFIG, c ); // Write new GYRO_CONFIG value to register

  // Set accelerometer full-scale range configuration
  c = i2cObject.readByte(ACCEL_CONFIG); // get current ACCEL_CONFIG register value
  c = c & ~0x18;  // Clear AFS bits [4:3]
  c = c | Ascale << 3; // Set full scale range for the accelerometer
  i2cObject.writeByte( ACCEL_CONFIG, c); // Write new ACCEL_CONFIG register value

  // Set accelerometer sample rate configuration
  // It is possible to get a 4 kHz sample rate from the accelerometer by choosing 1 for
  // accel_fchoice_b bit [3]; in this case the bandwidth is 1.13 kHz
  c = i2cObject.readByte(ACCEL_CONFIG2); // get current ACCEL_CONFIG2 register value
  c = c & ~0x0F; // Clear accel_fchoice_b (bit 3) and A_DLPFG (bits [2:0])
  c = c | 0x03;  // Set accelerometer rate to 1 kHz and bandwidth to 41 Hz
  i2cObject.writeByte(ACCEL_CONFIG2, c); // Write new ACCEL_CONFIG2 register value

  // Configure Interrupts and Bypass Enable
  // Set interrupt pin active high, push-pull, hold interrupt pin level HIGH until interrupt cleared,
  // clear on read of INT_STATUS, and enable I2C_BYPASS_EN so additional chips
  // can join the I2C bus and all can be controlled by the Arduino as master
  i2cObject.writeByte(INT_PIN_CFG, 0x22);
  i2cObject.writeByte(INT_ENABLE, 0x01);  // Enable data ready (bit 0) interrupt
}

void MPU9250_Acc_Gyro::read(void) {
  uint8_t block_Acc[6];  // x/y/z accelerometer registers data to be stored here
  uint8_t block_Gyr[6];  // x/y/z gyroscope registers data to be stored here

  if(i2cObject.readByte(INT_STATUS) & 0x01) { // wait for magnetometer data ready bit to be set
  i2cObject.readBlock(ACCEL_XOUT_H, sizeof(block_Acc), block_Acc);  // Read the six raw data and ST2 registers sequentially into data array
  raw.acc_x = ((int16_t)block_Acc[0] << 8) | block_Acc[1] ;  // Turn the MSB and LSB into a signed 16-bit value
  raw.acc_y = ((int16_t)block_Acc[2] << 8) | block_Acc[3] ;  // Data stored as big Endian
  raw.acc_z = ((int16_t)block_Acc[4] << 8) | block_Acc[5] ;

  i2cObject.readBlock(GYRO_XOUT_H, sizeof(block_Gyr), block_Gyr);  // Read the six raw data and ST2 registers sequentially into data array
  raw.gyr_x = ((int16_t)block_Gyr[0] << 8) | block_Gyr[1] ;  // Turn the MSB and LSB into a signed 16-bit value
  raw.gyr_y = ((int16_t)block_Gyr[2] << 8) | block_Gyr[3] ;  // Data stored as big Endian
  raw.gyr_z = ((int16_t)block_Gyr[4] << 8) | block_Gyr[5] ;
  }

}
