/* FXOS8700Q Example Program
 * Copyright (c) 2014-2015 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "FXOS8700Q.h"
#include "mbed.h"
#include <cmath>
#include <cstdio>
#include <limits>

I2C i2c(PTE25, PTE24);

FXOS8700QAccelerometer acc(i2c, FXOS8700CQ_SLAVE_ADDR1); 
FXOS8700QMagnetometer mag(i2c, FXOS8700CQ_SLAVE_ADDR1);

const float PI = 3.141593;
motion_data_units_t acc_data;

float filt_x, filt_y, filt_z, faX, faY, faZ, gn_aX, gn_aY, gn_aZ; //gn stands for "gravity neglected"

float roll;
float pitch;

int main(void) {

  acc.enable();
  mag.enable();

  ThisThread::sleep_for(1s);

  while (true) {

    // unit based results
    acc.getAxis(acc_data);
    acc.getX(faX);
    acc.getY(faY);
    acc.getZ(faZ);

    // Calculate Roll and Pitch (rotation around X-axis, rotation around Y-axis)
    pitch = atan(-1 * acc_data.y / sqrt(pow(acc_data.x, 2) + pow(acc_data.z, 2))) * 180 / PI;
    roll = atan(-1 * acc_data.x / sqrt(pow(acc_data.y, 2) + pow(acc_data.z, 2))) * 180 / PI;

    // neglecting gravity
    gn_aX = acc_data.x + sin(roll * PI / 180);
    gn_aY = acc_data.y + sin(pitch * PI / 180);

    //simple low pass filter
    filt_x = 0.85 * filt_x + 0.15 * gn_aX;
    filt_y = 0.85 * filt_y + 0.15 * gn_aY;
    filt_z = 0.85 * filt_z + 0.15 * acc_data.z;


    printf("$%.3f %.3f %.3f %.3f %.3f %.3f %.3f %.3f %.3f %.3f;",
           filt_x, filt_y, filt_z, gn_aX, gn_aY, acc_data.x, acc_data.y, acc_data.z, roll, pitch);


    ThisThread::sleep_for(10ms);
  }
}