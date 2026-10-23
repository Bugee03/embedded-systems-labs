// half_adder.v — smoke test (araçlar çalışıyor mu?)
// Bu dosyayı KOPYALAMA serbest; L01'den itibaren her şeyi elle yazacaksın.
`timescale 1ns / 1ps
module half_adder (
    input  wire a,
    input  wire b,
    output wire sum,
    output wire carry
);
    assign sum   = a ^ b;   // XOR
    assign carry = a & b;   // AND
endmodule
