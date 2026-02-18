TB=tb_decoder
SRC=decoder.vhd tb_decoder.vhd
WAVE=wave_decoder.vcd

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
