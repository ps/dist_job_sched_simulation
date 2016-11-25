COMPILER = gcc
CCFLAGS = -g -Wall

all: job_simulation

job_simulation: job_simulation.o
		$(COMPILER) -o job_simulation job_simulation.o -lpthread

job_simulation.o: job_simulation.c
		$(COMPILER) $(CCFLAGS) -c job_simulation.c

#removes all the object files created in the process
clean:
		rm -rf job_simulation *.o 
