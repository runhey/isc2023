module top_module ( input clk, input d, output q );
    wire [1:0] qq;
    my_dff instance1 (clk, d, qq[0]);
    my_dff instance2 (clk, qq[0], qq[1]);
    my_dff instance3 (clk, qq[1], q);

endmodule