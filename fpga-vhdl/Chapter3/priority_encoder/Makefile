# Makefile for GHDL Simulation
TB     := tb_prio_encoder
SRC    := priority_encoder.vhd tb_prio_encoder.vhd
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
