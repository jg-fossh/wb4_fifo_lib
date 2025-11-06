#################################################################################
# File name    : Makefile
# Author       : Jose R. Garcia (jg-fossh@protonmail.com)
# Project Name : WB4 UART
# Description  : 
#    wb4_dual_clock_fifo; wb4_sync_fifo
# Additional Comments:
#   
#################################################################################
PROJECT        = wb4_fifo_lib
UUT            = wb4_sync_fifo
FPGA_TECH      = lattice
TB_FRAMEWORK   = uvmsc
UVMSC_TESTNAME = test_fifo_default
UVMSC_TESTLIST = test_fifo_default \
                 test_fifo_one_wr_rd \
                 test_fifo_rd_empty
SIM_CMD        = all # test
##################################################################################################
#Capture user arguments
ifdef fpga_tech
FPGA_TECH = $(fpga_tech)
endif

ifdef framework
TB_FRAMEWORK = $(framework)
endif

ifdef sim_cmd
SIM_CMD = $(sim_cmd)
endif

ifdef uut
UUT = $(uut)
endif

ifdef uvmsc_testname
UVMSC_TESTNAME = $(uvmsc_testname)
endif

TOP = $(UUT)
##################################################################################################
RTL_DIR    = ./src
TB_DIR     = ./tb
SUBMODULES = ./sub 
SIM_DIR    = ./sim
UVMSC_DIR  = uvmsc
SYN_DIR    = ./syn
PNR_DIR    = ./build
COV_DIR    = logs
##################################################################################################
SIM_TOOL  = verilator
SYN_TOOL  = yosys
PNR_TOOL  = nextpnr
WAVE_TOOL = surfer    # gtkwave
COV_TOOL  = surf      # netsurf librewolf firefox chromiun-stable
##################################################################################################


#clear && make && gtkwave sim_wave.gtkw
.PHONY: help h
help h: 
	@echo "Usage : $(MAKE) <target> <arg>=value"
	@echo " "
	@echo "List of available targets:"
	@echo "   | Target                      | Description                                                         |"
	@echo "   | :-------------------------- | :------------------------------------------------------------------ |"
	@echo "   | help, h                     | Prints this quick help.                                             |"
	@echo "   | sim, verilator              | Runs simulation.                                                    |"
	@echo "   | wave, gtkwave, surfer       | Opens the waverform.                                                |"
	@echo "   | sim_gui                     | Runs the simulation and opens the waveform.                         |"
	@echo "   | cov, coverage               | Opens the code coverage report.                                     |"
	@echo "   | syn, yosys                  | Runs Yosys synthesis analisys. Not technology specific, no mapping. |"
	@echo "   | pnr, nextpnr, nextpnr-ice40 | Runs yosys mapping and nextpnr for Place and Route                  |"
	@echo "   | sta, timming, icetime       | Runs icetools Static Timing Analysis Tools.                         |"
	@echo "   | build, bitstream, icepack   | Generates the FPGA Bitstream file.                                  |"
	@echo " "
	@echo "List of available arguments:"
	@echo "   | Argument         | Description                              |"
	@echo "   | :--------------- | :--------------------------------------- |"
	@echo "   |                  | Visible DesignTop Level Parameters       |"


# Runs Simulation
.PHONY: verilator uvmsc sim
# Conditional logic (Make directives, not shell commands)
ifneq ($(filter-out all,$(UVMSC_TESTNAME)),)
verilator uvmsc sim: run_single_test
else
verilator uvmsc sim: run_all_tests
endif

run_single_test:
	clear
	@echo
	@echo "Running Simulation"
	@echo
	$(MAKE) -C $(SIM_DIR)/$(SIM_TOOL)/$(TOP) all uvmsc_testname=$(UVMSC_TESTNAME)

run_all_tests:
	clear
	@echo
	@echo "Running Simulation"
	@echo
	$(foreach test,$(UVMSC_TESTLIST), \
		$(MAKE) -C $(SIM_DIR)/$(SIM_TOOL)/$(TOP) all uvmsc_testname=$(test);)


# Opens the wave viewer
.PHONY: gtkwave wave surfer
gtkwave surfer wave:
	@echo
	@echo "Opening Waveform"
	@echo
	$(WAVE_TOOL) $(SIM_DIR)/$(SIM_TOOL)/$(TOP)/$(UVMSC_TESTNAME)/wave.fst -s $(SIM_DIR)/$(SIM_TOOL)/$(TOP)/wave.surf.ron


# Opens the wave viewer
.PHONY: cov_viewer coverage cov
ifneq ($(filter-out all,$(UVMSC_TESTNAME)),)
cov_viewer coverage cov: cov_single_test
else
cov_viewer coverage cov: cov_all_tests
endif

cov_single_test:
	clear
	@echo
	@echo "Opening Coverage Report"
	@echo
	$(COV_TOOL) $(SIM_DIR)/$(SIM_TOOL)/$(TOP)/$(UVMSC_TESTNAME)/$(COV_DIR)/html/index.html

cov_all_tests:
	clear
	$(MAKE) -C $(SIM_DIR)/$(SIM_TOOL)/$(TOP) merge_cov
	@echo
	@echo "Opening Coverage Report"
	@echo
	$(COV_TOOL) $(SIM_DIR)/$(SIM_TOOL)/$(TOP)/merged/html/index.html


# Runs simulations and opens the wave viewer after.
.PHONY: sim_gui
sim_gui: sim wave


# Runs Synthesis
.PHONY: syn yosys
syn yosys:
	clear
	@echo
	@echo "Running Synthesis Tool"
	@echo
	$(MAKE) all -C $(SYN_DIR)/$(SYN_TOOL)/$(FPGA_TECH)/$(TOP) 
	xdot -n $(SYN_DIR)/$(SYN_TOOL)/$(FPGA_TECH)/$(TOP)/$(TOP).dot


# Runs Place & Route Tool
.PHONY: pnr nextpnr nextpnr-ice40
pnr nextpnr nextpnr-ice40:
	clear
	@echo
	@echo "Running PnR Tool"
	@echo
	$(MAKE) map -C $(SYN_DIR)/$(SYN_TOOL)/$(FPGA_TECH)/$(TOP) 
	$(MAKE) all -C $(PNR_DIR)/$(PNR_TOOL)/$(FPGA_TECH)/$(TOP) 


# Generates Timming Reports
.PHONY: sta timming icetime
sta timming icetime:
	clear
	@echo
	@echo "Running Static Timing Analysis Tool"
	@echo
	$(MAKE) rpt -C $(SYN_DIR)/$(SYN_TOOL)/$(FPGA_TECH)/$(TOP) 

# Runs Synthesis
.PHONY: build bitstream
build bitstream:
	clear
	@echo
	@echo "Running Synthesis Tool"
	@echo
	$(MAKE) syn -C $(SYN_DIR)/$(SYN_TOOL)/$(FPGA_TECH)/$(TOP) 
	xdot $(SYN_DIR)/$(SYN_TOOL)/$(FPGA_TECH)/$(TOP)/$(TOP).dot

	

