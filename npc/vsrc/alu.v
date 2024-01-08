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
endmodule

module controller (
    input clk,
    input control_up,
    input control_down,
    output reg [2:0] control_led
);
reg button_up;
reg button_down;
button_press_detector i0 (control_up, button_up);
button_press_detector i1 (control_down, button_down);

always @(button_up or button_down) begin
    if(button_up && (control_led < 3'b111)) begin 
        control_led += 1 ;
        $display("Press button up: %x", control_led[2:0]);
    end
    else if(button_down && (control_led > 3'b000)) begin 
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
    
endmodule
