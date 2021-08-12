from SunFounder_Line_Follower import Line_Follower
from picar import front_wheels
from picar import back_wheels
import picar
import sys, getopt
from datetime import datetime
import time
import ctypes
import os

picar.setup()

REFERENCES = [200, 200, 200, 200, 200]
#calibrate = True
calibrate = False
forward_speed = 40
backward_speed = 30
turning_angle = 40

max_off_track_count = 40

delay = 0.0005

# ua = Ultrasonic_Avoidance.Ultrasonic_Avoidance(20)
picar.setup()
forward_speed = 60
pre_time = int((datetime.utcnow() - datetime(1970,1,1)).total_seconds()*1000)
# pre_distance = ua.get_distance()
fw = front_wheels.Front_Wheels(db='config')
bw = back_wheels.Back_Wheels(db='config')
lf = Line_Follower.Line_Follower()
bw.ready()

lf.references = REFERENCES
fw.ready()
bw.ready()
fw.turning_max = 45
BASE_SPEED = 50
BASE_ANGLE = 90
ANGLE_OFFSET = 15


def wrap_func(f, restype, argtypes):
	f.restype = restype
	f.argtypes = argtypes



def stop():
	bw.stop()
	fw.turn_straight()
	current_angle = 90

def get_sensor_readings():
	analog = lf.read_analog()
	digital = []
	for value in analog:
		if value < THRESHOLD: digital.append(1)
		else: digital.append(0)
	return digital

def main():
	global turning_angle
	global forward_speed
	global pre_distance
	global pre_time
	off_track_count = 0
	bw.speed = forward_speed

	a_step = 3
	b_step = 10
	c_step = 30
	d_step = 45
	bw.forward()
	while True:
		lt_status_now = get_sensor_readings()
		# print(lt_status_now)
		distance = int(pid_lib.distance())
		time1 = int((datetime.utcnow() - datetime(1970, 1, 1)).total_seconds() * 1000)
		print(str(distance) + ' cm')

		if distance > 0 and distance < 75:
			# with open(f1, 'a') as file:
			# 	file.write(str(time1) + ' ' + str(distance) + '\n')
			# with open(f2, 'a') as file:
			# 	file.write(str(time1) + ' ' + str(speed) + '\n')
			pre_time = time1
			if distance <= 60:
				forward_speed = 60 - (80 - distance)
			bw.forward()
			bw.speed = forward_speed
		else:
			bw.forward()
			bw.speed = forward_speed
		# Angle calculate
		if lt_status_now == [0, 0, 1, 0, 0]:
			step = 0
		elif lt_status_now == [0, 1, 1, 0, 0] or lt_status_now == [0, 0, 1, 1, 0]:
			step = a_step
		elif lt_status_now == [0, 1, 0, 0, 0] or lt_status_now == [0, 0, 0, 1, 0]:
			step = b_step
		elif lt_status_now == [1, 1, 0, 0, 0] or lt_status_now == [0, 0, 0, 1, 1]:
			step = c_step
		elif lt_status_now == [1, 0, 0, 0, 0] or lt_status_now == [0, 0, 0, 0, 1]:
			step = d_step

		# Direction calculate
		if lt_status_now == [0, 0, 1, 0, 0]:
			off_track_count = 0
			fw.turn(90)
		# turn right
		elif lt_status_now in ([0, 1, 1, 0, 0], [0, 1, 0, 0, 0], [1, 1, 0, 0, 0], [1, 0, 0, 0, 0]):
			off_track_count = 0
			turning_angle = int(90 - step)
		# turn left
		elif lt_status_now in ([0, 0, 1, 1, 0], [0, 0, 0, 1, 0], [0, 0, 0, 1, 1], [0, 0, 0, 0, 1]):
			off_track_count = 0
			turning_angle = int(90 + step)
		elif lt_status_now == [0, 0, 0, 0, 0]:
			off_track_count += 1
			if off_track_count > max_off_track_count:
				# tmp_angle = -(turning_angle - 90) + 90
				tmp_angle = (turning_angle - 90) / abs(90 - turning_angle)
				tmp_angle *= fw.turning_max
				bw.speed = backward_speed
				bw.backward()
				fw.turn(tmp_angle)

				lf.wait_tile_center()
				bw.stop()

				fw.turn(turning_angle)
				time.sleep(0.2)
				bw.speed = forward_speed
				bw.forward()
				time.sleep(0.2)



		else:
			off_track_count = 0

		fw.turn(turning_angle)
		time.sleep(delay)


if __name__ == '__main__':
	argv = sys.argv[1:]
	THRESHOLD = 0
	verbose = False
	try:
		opts, args = getopt.getopt(argv,"vt:",["threshold="])
	except getopt.GetoptError:
		print ('pid_line_follower.py -t <threshold>')
		sys.exit(2)

	for opt, arg in opts:
		if opt == '-v':
			verbose = True

		elif opt in ("-t", "--threshold"):
			THRESHOLD = float(arg)

	picar.setup()
	lf = Line_Follower.Line_Follower()
	fw = front_wheels.Front_Wheels(db='config')
	bw = back_wheels.Back_Wheels(db='config')

	pid_lib_name = os.path.abspath(".") + "/" + "pid.so"
	pid_lib = ctypes.CDLL(pid_lib_name)

	wrap_func(pid_lib.pid_init_gains, ctypes.c_int, [ctypes.c_float, ctypes.c_float, ctypes.c_float])
	wrap_func(pid_lib.pid_set_limits, ctypes.c_int, [ctypes.c_float, ctypes.c_float])
	wrap_func(pid_lib.pid_compute, ctypes.c_float, [ctypes.POINTER(ctypes.c_int), ctypes.c_int])
	wrap_func(pid_lib.distance, ctypes.c_float, [])

	pid_lib.pid_init_gains(5, 0, 0)
	pid_lib.pid_set_limits(-20, 20)

	try:
		stop()
		main()
	except KeyboardInterrupt:
		stop()

