COMPILER = gcc
CCFLAGS = -g -Wall
LIBS = -lpthread
C_FILES = $(wildcard *.c)
OBJ_DIR = obj
OBJ_FILES = $(addprefix $(OBJ_DIR)/,$(notdir $(C_FILES:.c=.o)))

OBJ = job_simulation.o \
	  log.o \
	  jobs.o \
	  job_operations.o \
	  worker_node.o \
	  master_node.o \
	  utils.o

all: dirs job_simulation

dirs: 
		mkdir -p $(OBJ_DIR)

job_simulation: $(OBJ_FILES)
		$(COMPILER) -o $@ $^ $(CCFLAGS) $(LIBS)

$(OBJ_DIR)/%.o: %.c job_simulation.h
		$(COMPILER) $(CCFLAGS) -c $< -o $@


#removes all the object files created in the process
clean:
		rm -rf job_simulation obj *.dat *.png small_* mid_* large_* varied_*
