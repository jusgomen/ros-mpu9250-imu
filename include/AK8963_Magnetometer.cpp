#include "AK8963_Magnetometer.h"

AK8963_Magnetometer::AK8963_Magnetometer(const char * i2cDeviceFilePath) : i2cObject(i2cDeviceFilePath) {
  i2cObject.addressSet(AK8963_ADDRESS);
}

void AK8963_Magnetometer::begin(void) {
  i2cObject.writeByte(AK8963_CNTL, 0x0F);
  i2cObject.writeByte(AK8963_CNTL, Mscale << 4 | Mmode);
}

void AK8963_Magnetometer::read(void) {
  uint8_t block[7];  // x/y/z gyro register data, ST2 register stored here, must read ST2 at end of data acquisition
  if(i2cObject.readByte(AK8963_ST1) & 0x01) { // wait for magnetometer data ready bit to be set
  i2cObject.readBlock(AK8963_XOUT_L, sizeof(block), block);  // Read the six raw data and ST2 registers sequentially into data array
  uint8_t c = block[6]; // End data read by reading ST2 register
    if(!(c & 0x08)) { // Check if magnetic sensor overflow set, if not then report data
    raw.x = ((int16_t)block[1] << 8) | block[0] ;  // Turn the MSB and LSB into a signed 16-bit value
    raw.y = ((int16_t)block[3] << 8) | block[2] ;  // Data stored as little Endian
    raw.z = ((int16_t)block[5] << 8) | block[4] ;
   }
  }
}
