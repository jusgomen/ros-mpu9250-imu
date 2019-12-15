#ifndef MPU9250_Acc_Gyro_h
#define MPU9250_Acc_Gyro_h

#include <stdint.h>
#include "I2CBus.h"

/* Device I2C Address */
#define MPU9250_ADDRESS   0x68    // Device address when ADO = 0
#define WHO_AM_I_MPU9250  0x75    // Should return 0x71

/* Register Addresses */
#define ACCEL_XOUT_H     0x3B
#define GYRO_XOUT_H      0x43

#define SMPLRT_DIV       0x19
#define CONFIG           0x1A
#define GYRO_CONFIG      0x1B
#define ACCEL_CONFIG     0x1C
#define ACCEL_CONFIG2    0x1D

#define INT_PIN_CFG      0x37
#define INT_ENABLE       0x38
#define INT_STATUS       0x3A

#define PWR_MGMT_1       0x6B // Device defaults to the SLEEP mode
#define PWR_MGMT_2       0x6C

// Set initial input parameters
enum Ascale {
  AFS_2G = 0,
  AFS_4G,
  AFS_8G,
  AFS_16G
};

enum Gscale {
  GFS_250DPS = 0,
  GFS_500DPS,
  GFS_1000DPS,
  GFS_2000DPS
};

// Specify sensor full scale
uint8_t Gscale = GFS_2000DPS;
uint8_t Ascale = AFS_2G;

typedef struct MPU9250AccelData_s {
  MPU9250AccelData_s()
  : gyr_x(0), gyr_y(0), gyr_z(0), acc_x(0), acc_y(0), acc_z(0) {}
  int16_t gyr_x;
  int16_t gyr_y;
  int16_t gyr_z;
  int16_t acc_x;
  int16_t acc_y;
  int16_t acc_z;
} MPU9250AccelData;

class MPU9250_Acc_Gyro {
  public:
    MPU9250AccelData raw;

    MPU9250_Acc_Gyro(const char * i2cDeviceFilePath);

    void begin(void);
    void read(void);
    
  private:
    I2CBus i2cObject;
};

#endif
