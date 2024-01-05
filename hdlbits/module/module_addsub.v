module top_module(
    input [31:0] a,
    input [31:0] b,
    input sub,
    output [31:0] sum
);
    wire cout_1, cout_2;
    wire [31:0] bxor;
    
    assign bxor = b ^ {32{sub}};
    add16 in1 (a[15:0], bxor[15:0], sub, sum[15:0], cout_1);
    add16 in2 (a[31:16], bxor[31:16], cout_1, sum[31:16], cout_2);

endmodule