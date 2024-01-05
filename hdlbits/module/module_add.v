module top_module(
    input [31:0] a,
    input [31:0] b,
    output [31:0] sum
);
    wire cout1, cout2;
    wire [15:0] lower_out;
    wire [15:0] upper_out;
    add16 instance1(a[15:0], b[15:0], 0, lower_out, cout1);
    add16 instance2(a[31:16], b[31:16], cout1, upper_out, cout2);
    
    assign sum = {upper_out, lower_out};

endmodule