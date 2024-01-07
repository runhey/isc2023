module top(
    input clk,
    input rst,
    input [7:0] sw,
    input enable,
    output indicator,
    output reg [2:0] led,
    output reg [6:0] seg
);
wire [7:0] in;


enabler i0 (enable, sw, in);
indicating i1 (in,indicator);
high_encoder i2 (in, led);
bcd7seg i3 ({1'b0, led}, seg);


endmodule

// 使能开关
module enabler (
    input en,
    input [7:0] in,
    output [7:0] out
);
assign out = {8{en}} & in;
endmodule

// 指示器
module indicating (
    input [7:0] in,
    output out
);
assign out = ~(8'h00 == in);
endmodule

// 高优先编码器
module high_encoder (
    input [7:0] in,
    output reg [2:0] out
);
integer i;
always @(in) begin
    casez (in)
        8'b1???????: out = 3'b111;
        8'b01??????: out = 3'b110;
        8'b001?????: out = 3'b101;
        8'b0001????: out = 3'b100;
        8'b00001???: out = 3'b011;
        8'b000001??: out = 3'b010;
        8'b0000001?: out = 3'b001;
        8'b00000001: out = 3'b000;
        default: out = 3'b000;
    endcase
end
endmodule

module bcd7seg (
    input [3:0] value,
    output reg [6:0] segments,
);
always @(*) begin
    case (value)
        4'h0: segments = 7'b1000000; // 数字 0
        4'h1: segments = 7'b1111001; // 数字 1
        4'h2: segments = 7'b0100100; // 数字 2
        4'h3: segments = 7'b0110000; // 数字 3
        4'h4: segments = 7'b0011001; // 数字 4
        4'h5: segments = 7'b0010010; // 数字 5
        4'h6: segments = 7'b0000010; // 数字 6
        4'h7: segments = 7'b1111000; // 数字 7
        4'h8: segments = 7'b0000000; // 数字 8
        4'h9: segments = 7'b0010000; // 数字 9
        4'hA: segments = 7'b0001000; // 字母 A
        4'hB: segments = 7'b0000011; // 字母 B
        4'hC: segments = 7'b1000110; // 字母 C
        4'hD: segments = 7'b0100001; // 字母 D
        4'hE: segments = 7'b0000110; // 字母 E
        4'hF: segments = 7'b0001110; // 字母 F
        default: segments = 7'b1111111;  // 显示空白
    endcase
end
    
endmodule
