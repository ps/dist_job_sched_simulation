import matplotlib.pyplot as plt
import sys


 #   Stores the assignment rate/size at current timestamp.
JOB_ASSIGNMENT_RATE_MSG = 444
#    Stores time at which master/worker started processing.
START_PROCESSING_MSG = 555
#    Stores time at wchich master/worker ended processing.
END_PROCESSING_MSG = 666
#    Stores the queue size of worker at current timestamp.
WORKER_QUEUE_SIZE_MSG = 777
#    Stores the number of jobs remaining to distribute.
JOBS_REMAINING_MSG = 999

NUM_JOBS = 0
JOB_TYPE = "ERROR"
NUM_WORKERS = 0
WORKER_QUEUE_SIZE = 0
MAX_JOBS_ASSIGN = 0
NODE_SELECTION = "ERROR"
DIST_MODE = "ERROR"

def parse_config(filename):
	global NUM_JOBS, JOB_TYPE, NUM_WORKERS, WORKER_QUEUE_SIZE, MAX_JOBS_ASSIGN, NODE_SELECTION, DIST_MODE
	lines = open(filename).read().split("\n")

	jobs = ["NONE", "SMALL", "MID", "LARGE", "VARIED"]
	node_selection = ["NONE", "SEQUENTIAL", "RANDOM", "SHORTEST QUEUE"]
	dist_mode = ["NONE", "CONSTANT", "LINEAR", "EXPONENTIAL"]

	line_num = 1
	while line_num < len(lines):
		
		if len(lines[line_num]) == 0:
			line_num = line_num + 1
			continue
		if line_num == 1:
			NUM_JOBS = int(lines[line_num])
		elif line_num == 4:
			JOB_TYPE = jobs[int(lines[line_num])]
		elif line_num == 7:
			NUM_WORKERS = int(lines[line_num])
		elif line_num == 10:
			WORKER_QUEUE_SIZE = int(lines[line_num])
		elif line_num == 13:
			MAX_JOBS_ASSIGN = int(lines[line_num])
		elif line_num == 16:
			NODE_SELECTION = node_selection[int(lines[line_num])]
		elif line_num == 19:
			DIST_MODE = dist_mode[int(lines[line_num])]
		elif line_num == 22 and DIST_MODE == "CONSTANT":
			DIST_MODE = "CONSTANT K=%s" % lines[line_num] 
		line_num = line_num + 1


def main():
	if len(sys.argv) != 3:
		print "Please provide the thread id to process and config file! python plot.py <thread-id> <sample.conf>"
		return

	parse_config(sys.argv[2])
	thread_id = int(sys.argv[1])
	print "Processing thread %s" % thread_id

	filename = "thread%s.dat" % thread_id
	lines = open(filename).read().split("\n")
	x_list = []
	y_list = []
	minint = -sys.maxint - 1
	max_x = minint
	min_x = sys.maxint
	max_y = minint
	min_y = sys.maxint

	x_list_2 = []
	y_list_2= []
	max_x_2 = minint
	min_x_2 = sys.maxint
	max_y_2 = minint
	min_y_2 = sys.maxint

	# (raw_time, adjusted_time, data, message id)
	for line in lines:
		if len(line) == 0:
			continue
		line = line.replace(" ", "")
		line = line.replace("\n", "")
		data = line.split(",")
		x_data = int(data[1]) / 1000.0
		y_data = int(data[2])
		msg_id = int(data[3])

		if thread_id == -1 and msg_id == JOBS_REMAINING_MSG:
			if x_data > max_x_2:
				max_x_2 = x_data
			if x_data < min_x_2:
				min_x_2 = x_data
			if y_data > max_y_2:
				max_y_2 = y_data
			if y_data < min_y_2:
				min_y_2 = y_data

			x_list_2.append(x_data)
			y_list_2.append(y_data)
		else:
			if x_data > max_x:
				max_x = x_data
			if x_data < min_x:
				min_x = x_data
			if y_data > max_y:
				max_y = y_data
			if y_data < min_y:
				min_y = y_data

			x_list.append(x_data)
			y_list.append(y_data)

	points = zip(x_list, y_list)
	sorted_points = sorted(points)
	x_list = [point[0] for point in sorted_points]
	y_list = [point[1] for point in sorted_points]


	print "Saving"
	if thread_id == -1:
		points = zip(x_list_2, y_list_2)
		sorted_points = sorted(points)

		x_list_2 = [point[0] for point in sorted_points]
		y_list_2 = [point[1] for point in sorted_points]

		axis = [0, max_x + 1, 0, max_y + 1]

		plt.figure().suptitle("Master Node Distribution", fontsize=14, fontweight="bold")
		plt.subplot(111)
		plt.plot(x_list, y_list, "bo-")
		plt.axis(axis)
		plt.title("%s (%s) Jobs, %s distribution, %s node selection" % (NUM_JOBS, JOB_TYPE, DIST_MODE, NODE_SELECTION))
		plt.xlabel("time in miliseconds")
		plt.ylabel("master chunk distribution size")
		plt.savefig("thread%s-assign_rate.png" % thread_id)
		plt.clf()

		axis = [0, max_x_2 + 1, 0, max_y_2 + 1]
		plt.figure().suptitle("Master Node Jobs Remaining", fontsize=14, fontweight="bold")
		plt.subplot(111)
		plt.plot(x_list_2, y_list_2, "bo-")
		plt.axis(axis)
		plt.title("%s distribution, %s node selection" % (DIST_MODE, NODE_SELECTION))
		plt.xlabel("time in miliseconds")
		plt.ylabel("jobs (%s) remaining" % JOB_TYPE)
		plt.savefig("thread%s-jobs_remaining.png" % thread_id)
	else:
		axis = [0, max_x + 1, 0, 51]
		plt.figure().suptitle("Node %s" % thread_id, fontsize=14, fontweight="bold")
		plt.subplot(111)
		plt.plot(x_list, y_list, "bo-")
		plt.axis(axis)
		plt.title("%s (%s) Jobs, %s distribution" % (NUM_JOBS, JOB_TYPE, DIST_MODE))
		plt.xlabel("time in miliseconds")
		plt.ylabel("worker queue size")
		plt.savefig("thread%s-queue_size.png" % thread_id)

main();