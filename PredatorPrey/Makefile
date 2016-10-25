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

TARGET = predator_prey

COMPILE_C   = $(CC) $(CFLAGS) $(I_PATH) -MD -c $< -o $@
LINK_BINARY = $(LD) $(LDFLAGS) $^ $(addprefix -l, $(L_FILES)) -o $@

# This var is used to add C files that are not in project folders
С_FILES +=
# This var contains folders with code in project
PRJ_C_SRC_DIRS = ./

# Object file to be produced
OBJS += $(patsubst %.c, %.o, $(C_FILES))
OBJS += $(patsubst %.c, %.o, $(wildcard  $(addsuffix /*.c, $(PRJ_C_SRC_DIRS))))

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
	@for i in $$(seq 1 10); do \
		point=`echo "scale = 3; ($$i * 0.1 + 0.8)" | bc`; \
		./$(TARGET) $(EXEC_FLAGS) -v -A 0.667 -B 1.333 -C 1 -D 1 -p $$point -P $$point -f "data_$$i.dat"; \
	done

plot:
	gnuplot plot.gp
	@open plot.pdf

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
