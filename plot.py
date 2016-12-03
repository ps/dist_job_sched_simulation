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
#    Stores the sleep time in microseconds of worker.
WORKER_SLEEP_TIME_MSG = 888
#    Stores the number of jobs remaining to distribute.
JOBS_REMAINING_MSG = 999

def main():
	if len(sys.argv) != 2:
		print "Please provide the thread id to process"
		return
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
		x_data = int(data[1])
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

	print "Saving"
	if thread_id == -1:
		axis = [min_x - 1, max_x + 1, min_y - 1, max_y + 1]
		plt.plot(x_list, y_list, "bo-")
		plt.axis(axis)
		plt.xlabel("time in microseconds")
		plt.ylabel("master chunk distribution size")
		plt.savefig("thread%s-assign_rate.png" % thread_id)
		plt.clf()

		axis = [min_x_2 - 1, max_x_2 + 1, min_y_2 - 1, max_y_2 + 1]
		plt.plot(x_list_2, y_list_2, "bo-")
		plt.axis(axis)
		plt.xlabel("time in microseconds")
		plt.ylabel("jobs remaining")
		plt.savefig("thread%s-jobs_remaining.png" % thread_id)
	else:
		axis = [min_x - 1, max_x + 1, min_y - 1, max_y + 1]
		plt.plot(x_list, y_list, "bo-")
		plt.axis(axis)
		plt.xlabel("time in microseconds")
		plt.ylabel("worker queue size")
		plt.savefig("thread%s-queue_size.png" % thread_id)

main();