module top(
    input [1:0] a0,
    input [1:0] a1,
    input [1:0] a2,
    input [1:0] a3,
    input [1:0] y,
    output [1:0] f
);

MuxKeyInternal #(4, 2, 2, 0) i0 (f, y, 2'b00, {
    2'b00, a0,
    2'b01, a1,
    2'b10, a2,
    2'b11, a3
  });

endmodule