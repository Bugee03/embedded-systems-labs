# Verilog kopya kâğıdı (derslerle birlikte doldurulacak)

> Bu dosyayı ben başlattım; her dersin sonunda **sen** öğrendiğin yapıyı kendi cümlenle ekleyeceksin.

## Altın kurallar (şimdiden ezberle)
1. Kombinasyonel: `always @(*)` + **blocking `=`** + her dalda her çıktıya değer ata (yoksa **latch**).
2. Sıralı: `always @(posedge clk)` + **non-blocking `<=`**.
3. Tek saat, **clock enable** ile yavaşlat — sayaç çıktısını saat olarak KULLANMA.
4. Dış dünyadan gelen asenkron sinyal (buton, UART RX) → **2-FF synchronizer**.
5. Her modül için testbench; her testbench self-checking.

## İskelet
```verilog
`timescale 1ns / 1ps
module name #(
    parameter W = 8
)(
    input  wire         clk,
    input  wire         rst,
    input  wire [W-1:0] din,
    output reg  [W-1:0] dout
);
    always @(posedge clk) begin
        if (rst) dout <= {W{1'b0}};
        else     dout <= din;
    end
endmodule
```
