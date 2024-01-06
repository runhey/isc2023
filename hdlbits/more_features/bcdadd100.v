module top_module( 
    input [399:0] a, b,
    input cin,
    output cout,
    output [399:0] sum );
    
    wire [399:0] temp;
    
    
    bcd_fadd bcd_adder(
        .a(a[3:0]),
            .b(b[3:0]),
            .cin(cin),
        .cout(temp[0]),
            .sum(sum[3:0])
        );
// Instantiate 100 copies of bcd_fadd
genvar i;
generate
    for (i = 1; i < 100; i = i + 1) begin: xx
        bcd_fadd bcd_adder(
            .a(a[i*4 + 3:i*4]),
            .b(b[i*4 + 3:i*4]),
            .cin(temp[i-1]),
            .cout(temp[i]),
            .sum(sum[i*4 + 3:i*4])
        );
    end
endgenerate
    assign cout = temp[399];

endmodule