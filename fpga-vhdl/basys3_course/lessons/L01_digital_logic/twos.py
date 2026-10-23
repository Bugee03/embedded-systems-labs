#!/usr/bin/env python3
"""twos.py — kağıt hesaplarını kontrol etmek için two's complement yardımcısı.

Kullanım:
    python3 twos.py -3 4          # -3'ü 4 bit göster
    python3 twos.py 0b1101 4      # 1101 bitlerini unsigned/signed yorumla
    python3 twos.py 0xB6 8        # hex giriş
    python3 twos.py --table 4     # 4-bit tüm değerler tablosu
    python3 twos.py --grow 14 16 64   # bit büyümesi: N-bit x M-bit, K toplam
"""
import math
import sys


def to_bits(value: int, n: int) -> str:
    lo, hi = -(1 << (n - 1)), (1 << n) - 1
    if not lo <= value <= hi:
        raise SystemExit(f"{value} does not fit in {n} bits (range {lo}..{hi})")
    return format(value & ((1 << n) - 1), f"0{n}b")


def interpret(bits: str) -> tuple[int, int]:
    n = len(bits)
    u = int(bits, 2)
    s = u - (1 << n) if bits[0] == "1" else u
    return u, s


def show(value: int, n: int) -> None:
    bits = to_bits(value, n)
    u, s = interpret(bits)
    grouped = "_".join(bits[max(i - 4, 0):i] for i in range(len(bits), 0, -4)[::-1])
    print(f"bits    : {grouped}  ({n}-bit)")
    print(f"hex     : 0x{u:0{(n + 3) // 4}X}")
    print(f"unsigned: {u}")
    print(f"signed  : {s}")
    inv = "".join("1" if b == "0" else "0" for b in bits)
    print(f"negate  : invert {inv} + 1 -> {to_bits((-s) & ((1 << n) - 1), n)}  (= {-s}, "
          f"{'fits' if -s <= (1 << (n - 1)) - 1 else 'OVERFLOW'})")


def table(n: int) -> None:
    print(f"{'bits':>{n}}  unsigned  signed")
    for u in range(1 << n):
        b = format(u, f"0{n}b")
        print(f"{b}  {u:8d}  {interpret(b)[1]:6d}")


def grow(n: int, m: int, k: int) -> None:
    prod = n + m
    acc = prod + math.ceil(math.log2(k)) if k > 1 else prod
    print(f"{n}-bit x {m}-bit product : {prod} bits")
    print(f"sum of {k} products      : {prod} + ceil(log2 {k}) = {acc} bits")


def main() -> None:
    a = sys.argv[1:]
    if not a:
        raise SystemExit(__doc__)
    if a[0] == "--table":
        table(int(a[1]))
    elif a[0] == "--grow":
        grow(int(a[1]), int(a[2]), int(a[3]))
    else:
        n = int(a[1])
        if a[0].startswith(("0b", "0x")):
            u = int(a[0], 0)
            bits = format(u, f"0{n}b")
            show(interpret(bits)[1], n)
        else:
            show(int(a[0]), n)


if __name__ == "__main__":
    main()
