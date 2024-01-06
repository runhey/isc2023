module top_module (
    input [4:0] a, b, c, d, e, f,
    output [7:0] w, x, y, z );//

    // assign { ... } = { ... };
    assign {w[7:0], x[7:0], y[7:0], z[7:0]} = {a[4:0], b, c, d, e, f, 2'b11};

endmodule

