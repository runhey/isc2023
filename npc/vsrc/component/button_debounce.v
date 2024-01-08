module debounce # (parameter DELAY_CYCLES = 10)(
  input wire button,
  output reg debounced_button
);

  reg [DELAY_CYCLES-1:0] history;
  reg state;

  always @(posedge button) begin
    case (state)
      0: begin
        if (button == 1'b1) begin
          history <= {history[DELAY_CYCLES-2:0], 1'b1};
          state <= 1;
        end
      end
      1: begin
        if (button == 1'b0) begin
          history <= {history[DELAY_CYCLES-2:0], 1'b0};
          state <= 0;
        end
      end
      default: state <= 0;
    endcase
  end

  assign debounced_button = (history == {DELAY_CYCLES{1'b1}});

endmodule