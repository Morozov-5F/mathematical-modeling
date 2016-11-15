# Used binaries
CC = gcc
LD = gcc
RM = rm -rf

.PHONY: clean clean_all plot example_general all_data prepare_animate animation

# Flags for c compiler
CFLAGS   = -ansi -Wall -Wpedantic
# Flags for linker
LDFLAGS	 = -L/usr/local/lib
# Shared libraries to link
L_FILES  = gsl gslcblas m
# Include folders
I_PATH   = -I/usr/local/include

TARGET = interspecies_communication

COMPILE_C   = $(CC) $(CFLAGS) $(I_PATH) -MD -c $< -o $@
LINK_BINARY = $(LD) $(LDFLAGS) $^ $(addprefix -l, $(L_FILES)) -o $@

# This var is used to add C files that are not in project folders
С_FILES +=
# This var contains folders with code in project
PRJ_C_SRC_DIRS = ./

# Object file to be produced
OBJS += $(patsubst %.c, %.o, $(C_FILES))
OBJS += $(patsubst %.c, %.o, $(wildcard  $(addsuffix /*.c, $(PRJ_C_SRC_DIRS))))

DATA_DIR = "first"
DATA_DIR += "second"
DATA_DIR += "third"
DATA_DIR += "fourth"

# Files to remove during clean
TOREMOVE += $(patsubst %.c, %.o, $(C_FILES))
TOREMOVE += $(patsubst %.c, %.d, $(C_FILES))
TOREMOVE += $(addsuffix /*.o,    $(PRJ_C_SRC_DIRS))
TOREMOVE += $(addsuffix /*.d,    $(PRJ_C_SRC_DIRS))
TOREMOVE += $(addsuffix .dbg,    $(TARGET))
# Remove plot data
TOREMOVE += $(addsuffix /*.png,  $(PRJ_C_SRC_DIRS))
TOREMOVE += $(addsuffix /*.gif,  $(PRJ_C_SRC_DIRS))
TOREMOVE += $(addsuffix /*.ps,   $(PRJ_C_SRC_DIRS))
TOREMOVE += $(addsuffix /*.svg,  $(PRJ_C_SRC_DIRS))
TOREMOVE += $(addsuffix /*.dat,  $(PRJ_C_SRC_DIRS))
TOREMOVE += $(addsuffix /*.dat,  $(DATA_DIR))
TOREMOVE += $(addsuffix /*.pdf,  $(DATA_DIR))
TOREMOVE += $(addsuffix /*.log,  $(PRJ_C_SRC_DIRS))

# Execute flags
EXEC_FLAGS = -t 1e-1 -T 20
EXEC_FLAGS_ANIM = -t 1e-1 -T 20
TOTAL_TRIES = 5

$(TARGET): $(OBJS)
	@echo "Linking object files: " $<
	$(LINK_BINARY)

%.o: %.c
	@echo "Compiling C file: " $<
	@$(COMPILE_C)

clean:
	@echo "Cleaning: " $(TOREMOVE)
	@$(RM) $(TOREMOVE)

clean_all: clean
	@echo "Cleaning build directory"
	@$(RM) $(TARGET)

example_data:
	chmod +x $(TARGET)
	@echo "First case"
	@mkdir -p first
	@for i in $$(seq 1 10); do \
		N1=`echo "scale = 3; ($$i * 0.1)" | bc`; \
		N2=`echo "scale = 3; ($$i * 0.1 - 0.1)" | bc`; \
		R1=3; \
		A1=2; \
		B1=1; \
		R2=2; \
		A2=1; \
		B2=1; \
		(cd first && ../$(TARGET) $(EXEC_FLAGS) -A $$A1 -B $$B1 -C $$R1 -D $$A2 -E $$B2 -F $$R2 -p $$N1 -P $$N2 -f "data_$$i.dat"); \
	done
	@echo "Second case"
	@mkdir -p second
	@for i in $$(seq 1 10); do \
		N1=`echo "scale = 3; ($$i * 0.1)" | bc`; \
		N2=`echo "scale = 3; ($$i * 0.1 - 0.1)" | bc`; \
		R1=2; \
		A1=1; \
		B1=2; \
		R2=3; \
		A2=1; \
		B2=1; \
		(cd second && ../$(TARGET) $(EXEC_FLAGS) -A $$A1 -B $$B1 -C $$R1 -D $$A2 -E $$B2 -F $$R2 -p $$N1 -P $$N2 -f "data_$$i.dat"); \
	done
	@echo "Third case"
	@mkdir -p third
	@for i in $$(seq 0 10); do \
		for j in $$(seq 0 10); do \
			N1=`echo "scale = 3; ($$i * 0.5)" | bc`; \
			N2=`echo "scale = 3; ($$j * 0.5)" | bc`; \
			NUM=`echo "scale = 3; ($$i * 11 + $$j)" | bc`; \
			R1=2; \
			A1=4; \
			B1=1; \
			R2=4; \
			A2=1; \
			B2=1; \
			(cd third && ../$(TARGET) $(EXEC_FLAGS) -A $$A1 -B $$B1 -C $$R1 -D $$A2 -E $$B2 -F $$R2 -p $$N1 -P $$N2 -f "data_$$NUM.dat"); \
		done; \
	done
	@echo "Fourth case"
	@mkdir -p fourth
	@for i in $$(seq 0 10); do \
		for j in $$(seq 0 10); do \
			N1=`echo "scale = 3; ($$i * 0.1)" | bc`; \
			N2=`echo "scale = 3; ($$j * 0.1)" | bc`; \
			NUM=`echo "scale = 3; ($$i * 11 + $$j)" | bc`; \
			R1=3; \
			A1=1; \
			B1=3; \
			R2=2; \
			A2=1; \
			B2=1; \
			(cd fourth && ../$(TARGET) $(EXEC_FLAGS) -A $$A1 -B $$B1 -C $$R1 -D $$A2 -E $$B2 -F $$R2 -p $$N1 -P $$N2 -f "data_$$NUM.dat"); \
		done; \
	done

plot:
	@echo "Plotting first target"
	@(cd first && gnuplot plot.gp)
	@echo "Plotting second target"
	@(cd second && gnuplot plot.gp)
	@echo "Plotting third target"
	@(cd third && gnuplot plot.gp)
	@echo "Plotting fourth target"
	@(cd fourth && gnuplot plot.gp)

info:
	@echo "### Diagnostic info ###"
	@echo "Project directories:" $(PRJ_C_SRC_DIRS)
	@echo "Project name: " $(TARGET)
	@echo "C Files:      " $(wildcard  $(addsuffix *.c, $(PRJ_C_SRC_DIRS))) $(C_FILES)
	@echo "Object files: " $(OBJS)
	@echo "C compiler:   " $(CC)
	@echo "Include path: " $(I_PATH)
	@echo "LD flags:     " $(LDFLAGS)
	@echo "LD files:     " $(L_FILES)
	@echo "CC flags:     " $(CFLAGS)
	@echo "Execute flags " $(EXEC_FLAGS)
