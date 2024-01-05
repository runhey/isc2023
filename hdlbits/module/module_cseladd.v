module top_module(
    input [31:0] a,
    input [31:0] b,
    output [31:0] sum
);
    wire cout, cout1, cout2;
    wire [15:0] lower_sum;
    wire [15:0] upper_sum_1;
    wire [15:0] upper_sum_2;
    
    add16 instance1 (a[15:0], b[15:0], 0, lower_sum, cout);
    add16 instance2 (a[31:16], b[31:16], 0, upper_sum_1, cout1);
    add16 instance3 (a[31:16], b[31:16], 1, upper_sum_2, cout2);
    always @* begin
    if(cout == 1)
         sum = {upper_sum_2, lower_sum};
    else 
         sum = {upper_sum_1, lower_sum};
    end
        

endmodule