// 为什么说要求输入的都是补码

module top (
    input clk,
    input rst,
    input control_up,
    input control_down,
    input [3:0] a,
    input [3:0] b,
    output reg [2:0] control_led, // 显示控制的模式
    output [3:0] result,
    output carry,
    output zero,
    output overflow
);
controller i0 (clk, control_up, control_down, control_led);
alu i1 (control_led, a, b, result, carry, zero, overflow);
endmodule

module controller (
    input clk,
    input control_up,
    input control_down,
    output reg [2:0] control_led
);

always @(posedge control_up or posedge control_down) begin
    if(control_up && (control_led < 3'b111)) begin 
        control_led += 1 ;
        $display("Press button up: %x", control_led[2:0]);
    end
    else if(control_down && (control_led > 3'b000)) begin 
        control_led -= 1;
        $display("Press button down: %x", control_led[2:0]);
    end
end
    
endmodule

module alu (
    input [2:0] control,
    input [3:0] a,
    input [3:0] b,
    output [3:0] result,
    output carry,
    output zero,
    output overflow
);
adder i0(a, b, result, carry, zero, overflow); 

// always @(control) begin
//     case (control) 
//     3'b000:  adder i0(a, b, result, carry, zero, overflow); 
//     // default: i0(a, b, result, carry, zero, overflow);
//     endcase
// end

    
endmodule

// 加法 输入是补码，输出是原码
module adder (
    input [3:0] a,
    input [3:0] b,
    output [3:0] result,
    output carry,
    output zero,
    output overflow
);
assign {carry, result} = a + b;
assign zero = ~(| result);
assign  overflow = a[3] == b[3] && a[3] != result[3]; 
endmodule



