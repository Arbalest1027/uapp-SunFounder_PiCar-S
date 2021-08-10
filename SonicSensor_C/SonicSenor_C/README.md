# PID Control Algorithm Implepentation

## Installation
- make the directory to generate 'pid.so' file.
- This shared library should be then copied to the 'examples' directory and interfaced with .py script
- Run with python3 pid_line_follower.py

## Usage
### Following API functions are implemented
- pid_init_gains(float Kp_input, float Ki_input, float Kd_input)
This function is used to set the tuning parameter for PID. It needs to be called only once. Returns 0 on success. If Ki_input and/or Kd_input is 0, 
then the control action gets converted to P, PI or PD respectively.
- int pid_set_limits(float low_lim_input, float high_lim_input)
This function sets the output limits on the correction value. The limits are usually governed by the mechanical constraints of the system. It needs to
be called only once. Returns 0 on success.
- void pid_reset_integral()
This resets the integral term manually if the user wants to reset the PID algorithm. Returns 0 on success.
- float pid_compute(int *sensor_data, int size)
This function copmutation the correction based on sensor readings of line following sensor. Currently it accespts the digital values between 0 and 1
for each sensor. It needs to called in the main loop to get correction for that state. Returns the correction as a single precision float.

### Running python driver code
- python3 pid_line_follower.py -v -t THRESHOLD
- -v is verbose flag that prints out sensor data and correction by PID algorithm

## Ideal Parameters
- THRESHOLD = 30 for dark wooden background. THRESHOLD = 150 for lighter background
- Use calibrate.py to get an ideal THRESHOLD value in real time
- PID gain: Kp = 5, Ki = 0, Kd = 0

## Limitations
- The current PID settings supports all smooth turns but unpredictable output for sharp turns.
- Car is able to follow a sharp turn upto 30deg
