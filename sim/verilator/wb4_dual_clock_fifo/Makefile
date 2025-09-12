
#################     General Arguments     #################
NPROCS = 4 # To run this sim a processor with at least to threads is required
NUM1   = 1 # Define the integer 1 for general purpose
NUM2   = 2 # Amount of threads to be used by verilators trace, 2 is the max.

# Check if the OS is Linux or Mac or BSD and detect the amount of threads aviable
OS := $(shell uname -s)
ifeq ($(OS),Linux)
  NPROCS := $(shell grep -c ^processor /proc/cpuinfo)
else ifeq ($(OS),Darwin)
  NPROCS := $(shell system_profiler | awk '/Number of CPUs/ {print $$4}{next;}')
endif # $(OS)
# Select one threads less than the amount avialable, leave one thread for the rest of the system
#THREADS := $(shell expr $(NPROCS) - $(NUM1))
THREADS := $(shell expr $(NPROCS))

#################     Verilator Arguments     #################
#-MAKEFLAGS -fuse-ld=mold\
#-CFLAGS -std=c++17 \
#-CFLAGS "-std=c++14 -no-pie -fuse-ld=mold" \
## --runtime-debug \
# -CFLAGS "-std=c++11"
# -LDFLAGS "-lstdc++ -lm -lsystemc -luvm-systemc" \
#  --default-language 1364-2005 \
#  --timing \


EXTRA_ARGS += \
-DVL_THREADED \
-LDFLAGS "-lstdc++ -lm -lsystemc -luvm-systemc" \
-j $(THREADS)

# Optimize
VERILATOR_FLAGS += --x-assign 0

# Coverage
VERILATOR_FLAGS += --assert --assert-case --coverage --coverage-underscore --trace-coverage

VERILATOR_FLAGS += \
--default-language 1800-2012 \
--trace-fst \
--trace-structs \
--trace-max-array 2048 \
--trace-threads $(NUM2) \
--threads $(THREADS) \
--build-jobs $(THREADS) \
--timescale-override 1ps/1ps \
--timing \
--sc \
--exe

VERILATOR_ARGS += $(EXTRA_ARGS) $(VERILATOR_FLAGS)

#################     Verilator Arguments     #################

TB_FRAMEWORK = uvmsc

UUT         = wb4_dual_clock_fifo
TB_TOP      = tb_top_$(UUT)
TOP_MODULE ?= $(TB_TOP)


UUT_DIR = ../../../src
TB_DIR  = ../../../tb/$(UUT)/$(TB_FRAMEWORK)
SUB_DIR = ../../../sub
SIM_DIR = sim/verilator/$(UUT)/obj_dir


VERILOG_SOURCES  = \
$(SUB_DIR)/generic_sbram/src/generic_sbram.v \
$(SUB_DIR)/cdc_lib/src/synchronizers/synchronizer.v \
$(SUB_DIR)/cdc_lib/src/async_fifo/rd_ctrl.v \
$(SUB_DIR)/cdc_lib/src/async_fifo/wr_ctrl.v \
$(SUB_DIR)/cdc_lib/src/async_fifo/wr_ctrl_n2one.v \
$(SUB_DIR)/cdc_lib/src/async_fifo/dual_clock_fifo.v \
$(SUB_DIR)/cdc_lib/src/async_fifo/dual_clock_fifo_n2one.v \
$(UUT_DIR)/wb4_dual_clock_fifo_1_to_1.v \
$(UUT_DIR)/wb4_dual_clock_fifo_N_to_1.v \
$(UUT_DIR)/wb4_dual_clock_fifo.v

TB_MAIN       = $(TB_DIR)/sc_main.cc
UVMSC_SOURCES = $(TB_MAIN)

######################################################################
# TOOL
VERILATOR_COVERAGE = verilator_coverage

# Create annotated source
VERILATOR_COV_FLAGS += --annotate logs/annotated
# A single coverage hit is considered good enough
VERILATOR_COV_FLAGS += --annotate-min 1
# Create LCOV info
VERILATOR_COV_FLAGS += --write-info logs/coverage.info
# Input file from Verilator
VERILATOR_COV_FLAGS += logs/coverage.dat

GENHTML = genhtml
######################################################################

#################     Verilator Arguments     #################


.PHONY: verilate
verilate:
	@echo
	@echo "Verilating..."
	@echo
	verilator $(VERILATOR_ARGS) --top-module $(UUT) $(UVMSC_SOURCES) $(VERILOG_SOURCES)
#	#verilator $(VERILATOR_ARGS) -Wno-SYNCASYNCNET --top-module $(UUT) $(UVMSC_SOURCES) $(VERILOG_SOURCES)

.PHONY: compile
compile:
	@echo
	@echo "Compiling Test Bench..."
	@echo
	$(MAKE) -j$(THREADS) -C obj_dir -f V$(UUT).mk V$(UUT)
##	$(MAKE) -j$(THREADS) -C obj_dir -f V$(UUT).mk V$(UUT)


.PHONY: rum_sim
rum_sim:
	@echo
	@echo "Running Sim..."
	@echo
	obj_dir/V$(UUT)


.PHONY: wave
wave:
	@echo
	@echo "Opening Wave..."
	@echo
	gtkwave wave.gtkw



.PHONY: cov coverage
cov coverage:
	@echo
	@echo "Processing Coverage..."
	@echo
	@rm -rf logs/annotated
	$(VERILATOR_COVERAGE) $(VERILATOR_COV_FLAGS)

.PHONY: genhtml coverage_report cov_rpt
genhtml coverage_report cov_rpt:
	@echo
	@echo "GENHTML... "
	@echo
	$(GENHTML) logs/coverage.info --output-directory logs/html


.PHONY: clean mostlyclean distclean
clean mostlyclean distclean:
	@echo
	@echo "Cleaning TB..."
	@echo
	rm -rf obj_dir logs *.log *.dmp *.vpd core


.PHONY: all
all: clean verilate compile rum_sim coverage cov_rpt
