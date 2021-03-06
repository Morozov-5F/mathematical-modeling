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
I_PATH   = /usr/local/include

TARGET = pendulum

COMPILE_C   = $(CC) $(CFLAGS) -MD -c $< -o $@
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
EXEC_FLAGS = -A 0.5 -t 1e-1 -T 20 -F 0
EXEC_FLAGS_ANIM = -V 1 -t 1e-1 -T 20 -F 0
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
	./$(TARGET) $(EXEC_FLAGS) -V 2 -e harmonic -f "harmonic.dat"
	./$(TARGET) $(EXEC_FLAGS) -V 2 -e exponential -f "exp.dat"
	./$(TARGET) $(EXEC_FLAGS) -V 2 -e general -f "general.dat"

prepare_animation:
	@echo "Preparing animation for velocities -6 to 6"
	@./$(TARGET) $(EXEC_FLAGS_ANIM) -A -5.5 -v -f "general1.dat"
	@./$(TARGET) $(EXEC_FLAGS_ANIM) -A -4.5 -f "general2.dat"
	@./$(TARGET) $(EXEC_FLAGS_ANIM) -A -3.5 -f "general3.dat"
	@./$(TARGET) $(EXEC_FLAGS_ANIM) -A -2.5 -f "general4.dat"
	@./$(TARGET) $(EXEC_FLAGS_ANIM) -A -1.5 -f "general5.dat"
	@./$(TARGET) $(EXEC_FLAGS_ANIM) -A -0.5 -f "general6.dat"
	@./$(TARGET) $(EXEC_FLAGS_ANIM) -A 0.5 -f "general7.dat"
	@./$(TARGET) $(EXEC_FLAGS_ANIM) -A 1.5  -f "general8.dat"
	@./$(TARGET) $(EXEC_FLAGS_ANIM) -A 2.5  -f "general9.dat"
	@./$(TARGET) $(EXEC_FLAGS_ANIM) -A 3.5  -f "general10.dat"
	@./$(TARGET) $(EXEC_FLAGS_ANIM) -A 4.5  -f "general11.dat"
	@./$(TARGET) $(EXEC_FLAGS_ANIM) -A 5.5 -f "general12.dat"

model_approximations:
	@./$(TARGET) -A 3.15 -t 1e-1 -T 20 -F 0 -f "general_model1.dat" -e general
	@./$(TARGET) -A 3.0 -t 1e-1 -T 20 -F 0 -f "general_model2.dat" -e general
	@./$(TARGET) -A 2.9 -t 1e-1 -T 20 -F 0 -f "general_model3.dat" -e general
	@./$(TARGET) -A 3.14 -t 1e-1 -T 20 -F 0 -f "approx_model1.dat" -e exponential
	@./$(TARGET) -A 3.0 -t 1e-1 -T 20 -F 0 -f "approx_model2.dat" -e exponential
	@./$(TARGET) -A 2.9 -t 1e-1 -T 20 -F 0 -f "approx_model3.dat" -e exponential
	gnuplot compare.gp

plot:
	gnuplot plot.gp

animation:
	@echo "Plotting animation"
	@gnuplot animate_plot.gp

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
