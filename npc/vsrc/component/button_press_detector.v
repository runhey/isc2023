module button_press_detector (
  input wire button,
  output reg button_pressed
);

reg button_previous_state;


always @(posedge button) begin
button_previous_state <= button; // 记录上一个状态的按钮值
end

always @(*) begin
button_pressed = (button == 1'b1) && (button_previous_state == 1'b0);
end
endmodule