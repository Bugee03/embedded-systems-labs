# Makefile for GHDL Simulation
TB     := testbench
SRC    := signal_demo.vhd testbench.vhd
WAVE   := wave_conv.vcd

.PHONY: all analyze build run clean wave

all: run

analyze:
	ghdl -a $(SRC)

build: analyze
	ghdl -e $(TB)

run: build
	ghdl -r $(TB) --vcd=$(WAVE)
	@echo "✅ Simulation complete. View with: make wave"

wave:
	gtkwave $(WAVE)

clean:
	rm -f *.o *.cf $(TB) $(WAVE)
