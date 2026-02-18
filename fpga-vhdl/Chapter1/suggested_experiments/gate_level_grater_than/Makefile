TB=tb_gate_level_grater_than
SRC=gate_level_grater_than.vhd tb_gate_level_grater_than.vhd
WAVE=wave_gt.vcd

all: run

analyze:
	ghdl -a $(SRC)

build: analyze
	ghdl -e $(TB)

run: build
	ghdl -r $(TB) --vcd=$(WAVE)
	@echo "Simulation complete. View with: gtkwave $(WAVE)"

clean:
	rm -f *.o *.cf $(TB) $(WAVE)
