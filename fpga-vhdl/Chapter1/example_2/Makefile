TB=tb_eq2

SRC=eq1.vhd eq2.vhd tb_eq2.vhd

WAVE=wave.vcd

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
