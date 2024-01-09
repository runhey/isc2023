module top(
    input clk,
    input rst,
    input ps2_clk,
    input ps2_data,
    output reg [6:0] seg_count2,
    output reg [6:0] seg_count1,
    output reg [6:0] seg_ascii2,
    output reg [6:0] seg_ascii1,
    output reg [6:0] seg_code2,
    output reg [6:0] seg_code1
);

reg [7:0] ps2_keyboard_in;
ps2_keyboard i_ps2_keyboard(
    .clk(clk),
    .resetn(1),
    .ps2_clk(ps2_clk),
    .ps2_data(ps2_data),
    .current_code(ps2_keyboard_in)
);

reg [7:0] ps2_keyboard_out;
reg pressed;
detector i_detector(
    .code_in(ps2_keyboard_in),
    .code_out(ps2_keyboard_out),
    .pressed(pressed)
);

seg_code i_seg_code(
    .pressed(pressed),
    .code(ps2_keyboard_out),
    .segments1(seg_code1),
    .segments2(seg_code2)
);

endmodule


module ps2_keyboard (
    input clk,
    input resetn,
    input ps2_clk,
    input ps2_data,
    output reg [7:0]current_code
);
reg [9:0] buffer;        // ps2_data bits
reg [3:0] count;  // count ps2_data bits
reg [2:0] ps2_clk_sync;

always @(posedge clk) begin
    ps2_clk_sync <=  {ps2_clk_sync[1:0],ps2_clk};
end
wire sampling = ps2_clk_sync[2] & ~ps2_clk_sync[1];

always @(posedge clk) begin
    if (resetn == 0) begin // reset
        count <= 0;
    end
    else begin
        if (sampling) begin
            if (count == 4'd10) begin
            if ((buffer[0] == 0) &&  // start bit
                (ps2_data)       &&  // stop bit
                (^buffer[9:1])) begin      // odd  parity
                $display("receive %x", buffer[8:1]);
                current_code <= buffer[8:1];
            end
            count <= 0;     // for next
            end else begin
            buffer[count] <= ps2_data;  // store ps2_data
            count <= count + 3'b1;
            end
        end
    end
end
endmodule

module detector (
    input [7:0] code_in,
    output reg [7:0] code_out,
    output reg pressed
);
reg [7:0] last_code;
// 去它鸭的三段式状态更新，怎么舒服怎么来
always @(code_in) begin
    if(last_code == 8'hf0) begin
        pressed <= 1'b0;
    end
    else if (last_code != code_in)begin
        pressed <= 1'b1;
    end
    last_code = code_in;
    code_out = code_in;
end
endmodule



module bcd7seg (
    input [3:0] value,
    input enable,
    output reg [6:0] segments
);
always @(*) begin
    if(enable) begin
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
    else begin
        segments = 7'b1111111;
    end
end
    
endmodule

module seg_code (
    input pressed,
    input [7:0] code,
    output reg [6:0] segments1,
    output reg [6:0] segments2
);
bcd7seg i0 (.value(code[3:0])
            ,.enable(pressed)
            ,.segments(segments1));
bcd7seg i1 (.value(code[7:4])
            ,.enable(pressed)
            ,.segments(segments2));
endmodule


