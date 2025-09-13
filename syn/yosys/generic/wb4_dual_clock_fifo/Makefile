
# PIN_DEF = up5k
# DEVICE = up5k
# PACKAGE = sg48
PROJ     =wb4_uart_top
PIN_DEF  =hx8k
DEVICE   =hx8k
PACKAGE  =ct256
PNR      =nextpnr-ice40
SEED_0   =369
MAP_OPT ?=SIZE

all: clean rpt assertions

assertions: 
	egrep -w 'Error-Src|Error-Type|Error-Msg' $(PROJ)_$(DEVICE)_syn.log

syn: 
	yosys -ql $(PROJ)_$(DEVICE)_syn.log syn_ice40.ys $^

map: 
	@echo "Running Technology Mapping Tool"
ifeq ( $(MAP_OPT) , SPEED)
	@echo "Mapping optimized for SPEED"
	yosys -ql $(PROJ)_$(DEVICE)_syn.log map_ice40_speed.ys $^
else
	@echo "Mapping optimizating for SIZE"
	yosys -ql $(PROJ)_$(DEVICE)_syn.log map_ice40_size.ys $^
endif

pnr: map 
	$(PNR) --$(DEVICE) --package $(PACKAGE) --json $(PROJ)_syn.json --pcf $(PIN_DEF).pcf --pcf-allow-unconstrained --timing-allow-fail --ignore-loops --pre-pack pre_pack.py --opt-timing --seed $(SEED_0) --asc $(PROJ)_$(SEED_0).asc --report critical_path_rpt$(SEED_0).json

rpt: pnr
	icetime -d $(DEVICE) -p $(PIN_DEF).pcf -P $(PACKAGE) -c 12 -mtr Timming_Report_$(SEED_0).rpt $(PROJ)_$(SEED_0).asc

build: rpt
	icepack $(PROJ)_$(SEED_0).asc $(PROJ).bin

prog: $(PROJ).bin
	iceprog $<

sudo-prog: $(PROJ).bin
	@echo 'Executing prog as root!!!'
	sudo iceprog $<

clean:
	rm -f $(PROJ)_syn.blif $(PROJ)_syn.edif $(PROJ).asc $(PROJ).rpt $(PROJ).bin $(PROJ)_syn.json $(PROJ)_$(DEVICE)_syn.log $(PROJ)_*.asc Timming_Report_* critical_path_rpt*.json

.SECONDARY:
.PHONY: all syn map pnr rpt build prog sudo-prog assertions clean

