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

ascii_display i_ascii_display(
    .pressed(pressed),
    .code(ps2_keyboard_out),
    .seg_ascii2(seg_ascii2),
    .seg_ascii1(seg_ascii1)
);

seg_counter i_seg_counter(
    .pressed(pressed),
    .seg_count2(seg_count2),
    .seg_count1(seg_count1)
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


module lut_ascii (
    input [7:0] code,
    output reg [3:0] ascii_code_high,
    output reg [3:0] ascii_code_low 
);
always @(code) begin
    case (code)
        8'h1C : {ascii_code_high, ascii_code_low} = {4'h4, 4'h1}; //A
        8'h32 : {ascii_code_high, ascii_code_low} = {4'h4, 4'h2}; //B
        8'h21 : {ascii_code_high, ascii_code_low} = {4'h4, 4'h3}; //C
        8'h23 : {ascii_code_high, ascii_code_low} = {4'h4, 4'h4}; //D
        8'h24 : {ascii_code_high, ascii_code_low} = {4'h4, 4'h5}; //E
        8'h2B : {ascii_code_high, ascii_code_low} = {4'h4, 4'h6}; //F
        8'h34 : {ascii_code_high, ascii_code_low} = {4'h4, 4'h7}; //G
        8'h33 : {ascii_code_high, ascii_code_low} = {4'h4, 4'h8}; //H
        8'h43 : {ascii_code_high, ascii_code_low} = {4'h4, 4'h9}; //I
        8'h3B : {ascii_code_high, ascii_code_low} = {4'h4, 4'hA}; //J
        8'h42 : {ascii_code_high, ascii_code_low} = {4'h4, 4'hB}; //K
        8'h4B : {ascii_code_high, ascii_code_low} = {4'h4, 4'hC}; //L
        8'h3A : {ascii_code_high, ascii_code_low} = {4'h4, 4'hD}; //M
        8'h31 : {ascii_code_high, ascii_code_low} = {4'h4, 4'hE}; //N
        8'h44 : {ascii_code_high, ascii_code_low} = {4'h4, 4'hF}; //O
        8'h4D : {ascii_code_high, ascii_code_low} = {4'h5, 4'h0}; //P
        8'h15 : {ascii_code_high, ascii_code_low} = {4'h5, 4'h1}; //Q
        8'h2D : {ascii_code_high, ascii_code_low} = {4'h5, 4'h2}; //R
        8'h1B : {ascii_code_high, ascii_code_low} = {4'h5, 4'h3}; //S
        8'h2C : {ascii_code_high, ascii_code_low} = {4'h5, 4'h4}; //T
        8'h3C : {ascii_code_high, ascii_code_low} = {4'h5, 4'h5}; //U
        8'h2A : {ascii_code_high, ascii_code_low} = {4'h5, 4'h6}; //V
        8'h1D : {ascii_code_high, ascii_code_low} = {4'h5, 4'h7}; //W
        8'h22 : {ascii_code_high, ascii_code_low} = {4'h5, 4'h8}; //X
        8'h35 : {ascii_code_high, ascii_code_low} = {4'h5, 4'h9}; //Y
        8'h1A : {ascii_code_high, ascii_code_low} = {4'h5, 4'hA}; //Z
        8'h16 : {ascii_code_high, ascii_code_low} = {4'h3, 4'h1}; //不是右边的小键盘 1
        8'h1E : {ascii_code_high, ascii_code_low} = {4'h3, 4'h2}; // 2
        8'h26 : {ascii_code_high, ascii_code_low} = {4'h3, 4'h3}; // 3 
        8'h25 : {ascii_code_high, ascii_code_low} = {4'h3, 4'h4}; // 4
        8'h2E : {ascii_code_high, ascii_code_low} = {4'h3, 4'h5}; // 5
        8'h36 : {ascii_code_high, ascii_code_low} = {4'h3, 4'h6}; // 6
        8'h3D : {ascii_code_high, ascii_code_low} = {4'h3, 4'h7}; // 7
        8'h3E : {ascii_code_high, ascii_code_low} = {4'h3, 4'h8}; // 8
        8'h46 : {ascii_code_high, ascii_code_low} = {4'h3, 4'h9}; // 9
        8'h45 : {ascii_code_high, ascii_code_low} = {4'h3, 4'h0}; // 0
        default: {ascii_code_high, ascii_code_low} = {4'h4, 4'h1}; //A
    endcase
end
endmodule

module ascii_display (
    input pressed,
    input [7:0] code,
    output reg [6:0] seg_ascii2,
    output reg [6:0] seg_ascii1
);
wire [3:0] code_high;
wire [3:0] code_low;

lut_ascii i_lut_ascii(
    .code(code),
    .ascii_code_high(code_high),
    .ascii_code_low(code_low)
);
bcd7seg i2_bcd7seg(
    .value(code_high),
    .enable(pressed),
    .segments(seg_ascii2)
);
bcd7seg i1_bcd7seg(
    .value(code_high),
    .enable(pressed),
    .segments(seg_ascii1)
);
endmodule


module seg_counter (
    input pressed,
    output reg [6:0] seg_count2,
    output reg [6:0] seg_count1
);

reg [3:0] count_high; //十位
reg [3:0] count_low; //个位

always @(negedge pressed) begin
    count_low += 1;
    if (count_low == 4'd10)begin
        count_low = 0;
        count_high += 1;
    end
    if (count_high == 4'd10)begin
        count_high = 0;
    end
end

bcd7seg i2_bcd7seg(
    .value(count_high),
    .enable(1),
    .segments(seg_count2)
);
bcd7seg i1_bcd7seg(
    .value(count_low),
    .enable(1),
    .segments(seg_count1)
);

endmodule




