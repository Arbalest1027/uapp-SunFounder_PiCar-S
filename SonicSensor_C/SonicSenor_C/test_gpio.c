/*
 *  dht11_string.c:
 *  Simple test program to test the wiringPi functions
 *  DHT11 test
 */


#include <wiringPi.h>

#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#define DHTPIN	7
#define samples  20000


float timedifference_msec(struct timeval t0, struct timeval t1)
{
    return (t1.tv_sec - t0.tv_sec) * 1000.0f + (t1.tv_usec - t0.tv_usec) / 1000.0f;
}


int main()
{
   int i;
   struct timeval t0;
   struct timeval t1;
   struct timeval timeout;
   float elapsed;
   float distance;

   /* pull pin down for 18 milliseconds */
   wiringPiSetup();
   pinMode( DHTPIN, OUTPUT );
   delay (10);
   digitalWrite( DHTPIN, LOW );
   delay(1);
   /* then pull it up for 40 microseconds */
   digitalWrite( DHTPIN, HIGH );
   delayMicroseconds( 40 );
   /* prepare to read the pin */
   pinMode( DHTPIN, INPUT );
   gettimeofday(&timeout, 0);
   while (!digitalRead(DHTPIN)){
     printf("0");
     gettimeofday(&t0, 0);
     elapsed = timedifference_msec(timeout, t0);
     if (elapsed > 50){
        return -1;
     }
   }      

   while (digitalRead(DHTPIN))
   {
     printf("1"); 
     gettimeofday(&t1, 0);
     elapsed = timedifference_msec(timeout, t0);
     if (elapsed > 50){
        return -1;
     }
   }
   elapsed = timedifference_msec(t0, t1);
   distance = elapsed / 1000 * 343.0 / 2;
   printf("Distance: %f cm\n", distance);
   return (distance);
}
