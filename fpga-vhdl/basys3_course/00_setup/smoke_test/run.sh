#!/usr/bin/env bash
# Kullanım: ./run.sh      (derle + simüle et + lint)
set -e
iverilog -g2005 -Wall -o sim.out half_adder.v tb_half_adder.v   # derle
vvp sim.out                                                      # çalıştır
if command -v verilator >/dev/null; then
    verilator --lint-only -Wall half_adder.v && echo "LINT OK"
fi
echo "Dalga formu için: gtkwave half_adder.vcd"
