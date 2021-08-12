#include "pid.h"
#include <unistd.h>

static float integral_error = 0;
static float prev_error = 0;
static clock_t start_time = 0;

static float Kp = 0, Ki = 0, Kd = 0;
static float low_lim  = 0, high_lim = 0;
static const int weights[] = {-4, -2, 0, 2, 4};

int pid_init_gains(float Kp_input, float Ki_input, float Kd_input) {
    Kp = Kp_input;
    Ki = Ki_input;
    Kd = Kd_input;
    start_time = clock();
    return 0;
}

int pid_set_limits(float low_lim_input, float high_lim_input) {
    low_lim = low_lim_input;
    high_lim = high_lim_input;
    return 0;
}

int pid_reset_integral() {
    integral_error = 0;
    return 0;
}

float pid_clip(float correction) {
    if (correction < low_lim)
        correction =  low_lim;
    if (correction > high_lim)
        correction =  high_lim;
    return correction;
}

float pid_compute_error(int *sensor_data, int size) {
    float error = 0;
    float count = 0;

    int idx = 0;
    for (idx = 0; idx < size; idx++) {
        if(sensor_data[idx] == 1) {
            error += (float)weights[idx];
            count += 1;
        }
    }
    if (count == 0)
        return 0;

    return (error / count);
}

float pid_compute(int *sensor_data, int size) {
    float error = pid_compute_error(sensor_data, size);

    clock_t end_time = clock();
    clock_t diff = end_time - start_time;
    start_time = end_time;

    integral_error += error * diff;
    float derivative_error = (error - prev_error) * diff;

    float correction = Kp*error + Ki*integral_error + Kd*derivative_error;
    correction = pid_clip(correction);
    return correction;
}


float timedifference_msec(struct timeval t0, struct timeval t1)
{
    return (t1.tv_sec - t0.tv_sec) * 1000.0f + (t1.tv_usec - t0.tv_usec) / 1000.0f;
}


float cal_distance()
{
   int i;
   struct timeval t0;
   struct timeval t1;
   struct timeval timeout;
   struct timeval elapsed;
   double time_elapsed;
   double distance;

   /* pull pin down for 18 milliseconds */
   wiringPiSetupGpio();
   pinMode( DHTPIN, OUTPUT );
   digitalWrite( DHTPIN, LOW );
   delayMicroseconds( 10 );
   digitalWrite( DHTPIN, HIGH );
   delayMicroseconds( 25 );
   digitalWrite( DHTPIN, LOW );
   pinMode( DHTPIN, INPUT );
   gettimeofday(&timeout, 0);

      while (!digitalRead(DHTPIN)){
         gettimeofday(&t0, 0);
         timersub(&t0, &timeout, &elapsed); 
	time_elapsed = elapsed.tv_usec/1000.0;
         if (time_elapsed > 50){
            return -1;
         }
      }      
      while (digitalRead(DHTPIN))
      { 
        gettimeofday(&t1, 0);
        timersub(&t0, &timeout, &elapsed);
	time_elapsed = elapsed.tv_usec/1000.0;
         if (time_elapsed > 50){
            return -2;
         }
      }
      timersub(&t1, &t0, &elapsed); 
      time_elapsed = elapsed.tv_sec + elapsed.tv_usec/1000000.0;
      distance = time_elapsed * 343.0 / 2.0;
   if (distance >= 0 ){
      return (distance * 100.0);
   } else {
      return -1; 
   }
}

float distance(){
   float distance = 0;
   for (int i = 0;i < 5; i++){
      distance += cal_distance();
   }
   return (distance / 5);
}

