Place all the code in this folder to /example
To run
```
make
```
then
```
Python3 pid_line_follower.py -v -t 100
```

Some part of this code was used from the pid_shared folder. Make sure you have wiringPi installed in your device. I shall modify, improve this code once the basic test are 
finished and successful. 
Once placed an obstacle, this code should have a faster detection.

#Limitations
- The ultra sonic with current code supports reliable real-time distant measurement within a range of 0-100cm.
- With current settings, the ultra sonic sensor is possible to measure speed under certain conditions.
  - The car has to go straight towards the obstacle 
  - The obstacle must cover the whole sensor detection area which is around 25 degrees in the front of the sensor
  - Due to the noise of the sensor, data collected requires smoothing functions to get roughly accurate results.
