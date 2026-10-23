#!/bin/sh
# Yeni proje olustur.
#   ./yeni.sh benim_proje               -> Template'ten bos proje
#   ./yeni.sh adc_deneme ADC_SingleMode -> BSP ornegini kopyala
# Ornek listesi: ls M031BSP/SampleCode/StdDriver
set -e
cd "$(dirname "$0")"
NAME="$1"; SAMPLE="$2"
[ -z "$NAME" ] && { echo "kullanim: ./yeni.sh <proje_adi> [BSP_ornek_adi]"; exit 1; }
DST="projects/$NAME"
[ -e "$DST" ] && { echo "$DST zaten var"; exit 1; }
if [ -n "$SAMPLE" ]; then
  SRC="M031BSP/SampleCode/StdDriver/$SAMPLE"
  [ -d "$SRC" ] || SRC="M031BSP/SampleCode/RegBased/$SAMPLE"
  [ -d "$SRC" ] || { echo "Ornek bulunamadi: $SAMPLE"; exit 1; }
else
  SRC="projects/template"
fi
mkdir -p "$DST"
cp "$SRC"/*.c "$DST"/ 2>/dev/null || true
cp "$SRC"/*.h "$DST"/ 2>/dev/null || true
printf 'include ../../mk/m031.mk\n' > "$DST/Makefile"
echo "Olusturuldu: $DST"
echo "  cd $DST && make flash"
