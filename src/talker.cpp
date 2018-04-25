#include "ros/ros.h"
#include "sensor_msgs/Imu.h"

#include <AK8963AK8963_Magnetometer.h>
#include <MPU9250_Acc_Gyro.h>
#include <AK8963AK8963_Magnetometer.cpp>
#include <MPU9250_Acc_Gyro.cpp>
#include <I2CBus.cpp>
#include <unistd.h>
#include <math.h>
#include <limits.h>

/* Constants */
#define PI                                (3.14159265F);
#define GYRO_SENSITIVITY_2000DPS          (0.070F)
#define SENSORS_GRAVITY_EARTH             (9.80665F)              /**< Earth's gravity in m/s^2 */
#define SENSORS_GRAVITY_STANDARD          (SENSORS_GRAVITY_EARTH)
#define SENSORS_DPS_TO_RADS               (0.017453293F)          /**< Degrees/s to rad/s multiplier */
#define SENSORS_MILIGAUSS_TO_TESLA        (10000000)

int main(int argc, char **argv)
{
  const char* i2cDevice = "/dev/i2c-1";
  MPU9250_Acc_Gyro acc_gyro(i2cDevice);
  AK8963_Magnetometer mag(i2cDevice);

  acc_gyro.begin();
  mag.begin();

  ros::init(argc, argv, "imu_node");
  ros::NodeHandle n;

  ros::Publisher chatter_pub = n.advertise<sensor_msgs::Imu>("imu", 100);

  ros::Rate loop_rate(10);

  int count = 0;
  while (ros::ok())
  {

    sensor_msgs::Imu imu;

    acc_gyro.read();
    mag.read();

    imu.header.stamp = ros::Time::now();
    imu.header.frame_id = "imu_link";

    imu.orientation_covariance = {0.0025, 0, 0, 0, 0.0025, 0, 0, 0, 0.0025};
    imu.angular_velocity_covariance = {0.0025, 0, 0, 0, 0.0025, 0, 0, 0, 0.0025};
    imu.linear_acceleration_covariance = {0.0025, 0, 0, 0, 0.0025, 0, 0, 0, 0.0025};

    roll = (float)atan2(acc_gyro.raw.acc_y, acc_gyro.raw.acc_z);
    imu.orientation.x = roll

    if (acc_gyro.raw.acc_y * sin(roll) + acc.raw.acc_gyro.raw.acc_z * cos(roll))==0{
      if (acc_gyro.raw.acc_x>0){
        pitch = PI / 2
      } else{
        pitch = -PI / 2
      }
    }else{
      pitch = (float)atan(-acc_gyro.raw.acc_x / (acc_gyro.raw.acc_y * sin(roll) + acc_gyro.raw.acc_z * cos(roll)));
    }

    imu.orientation.y = pitch;

    yaw = (float)atan2(mag.raw.z * sin(roll) - mag.raw.y * cos(roll), mag.raw.x * cos(pitch) + mag.raw.y * sin(pitch) * sin(roll) + mag.raw.z * sin(pitch) * cos(roll));
    imu.orientation.z = yaw;

    imu.orientation.w = 1;

    imu.angular_velocity.x = acc_gyro.raw.gyr_x * GYRO_SENSITIVITY_2000DPS * SENSORS_DPS_TO_RADS;
    imu.angular_velocity.y = acc_gyro.raw.gyr_y * GYRO_SENSITIVITY_2000DPS * SENSORS_DPS_TO_RADS;
    imu.angular_velocity.z = acc_gyro.raw.gyr_z * GYRO_SENSITIVITY_2000DPS * SENSORS_DPS_TO_RADS;

    imu.linear_acceleration.x = acc_gyro.raw.acc_x * SENSORS_GRAVITY_STANDARD;
    imu.linear_acceleration.y = acc_gyro.raw.acc_y * SENSORS_GRAVITY_STANDARD;
    imu.linear_acceleration.z = acc_gyro.raw.acc_z * SENSORS_GRAVITY_STANDARD;

    chatter_pub.publish(imu);

    ros::spinOnce();

    loop_rate.sleep();
    ++count;
  }


  return 0;
}
