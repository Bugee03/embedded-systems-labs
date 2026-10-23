TB=tb_bi_demo
SRC=bi_demo.vhd tb_bi_demo.vhd
WAVE=wave_bi.vcd

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

