# Proje klasoru disindan kisa yollar.
#   make monitor            -> seri port (hangi proje yuklu olursa olsun)
#   make probe              -> Nu-Link bagli mi?
#   make flash P=13_i2c_oled  -> o projeyi derle + yukle
NUVOTON := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
ifeq ($(notdir $(NUVOTON)),projects)
NUVOTON := $(abspath $(NUVOTON)/..)
endif

.PHONY: monitor probe flash
monitor probe:
	@$(MAKE) --no-print-directory -C $(NUVOTON)/projects/template $@

flash:
	@test -n "$(P)" || { echo "Kullanim: make flash P=<proje_klasoru>  (ls $(NUVOTON)/projects)"; exit 1; }
	@$(MAKE) --no-print-directory -C $(NUVOTON)/projects/$(P) flash
