#include <time.h>

#include <wiringPi.h>

#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#define DHTPIN	28
#define samples  20000

int pid_init_gains(float Kp_input, float Ki_input, float Kd_input);
int pid_set_limits(float low_lim_input, float high_lim_input);
int pid_reset_integral();

float pid_clip(float correction);
float pid_compute_error(int *sensor_data, int size);
float pid_compute(int *sensor_data, int size);
float timedifference_msec(struct timeval t0, struct timeval t1);
float distance();
