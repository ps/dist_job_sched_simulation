import matplotlib.pyplot as plt
import sys

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
	# (raw_time, adjusted_time, data)
	for line in lines:
		if len(line) == 0:
			continue
		line = line.replace(" ", "")
		line = line.replace("\n", "")
		data = line.split(",")
		x_data = int(data[1])
		y_data = int(data[2])

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

	axis = [min_x - 1, max_x + 1, min_y - 1, max_y + 1]
	plt.plot(x_list, y_list, "ro")
	plt.axis(axis)
	plt.xlabel("time in microseconds")
	if thread_id == -1:
		plt.ylabel("master chunk distribution size")
	else:
		plt.ylabel("worker queue size")
	print "Saving"
	plt.savefig("thread%s.png" % thread_id)
	print "About to show"
	plt.show()

main();