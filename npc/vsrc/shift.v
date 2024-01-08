module top (
    input clk,
    input rst,
    input button,
    output reg [7:0] led
);
always @(posedge button) begin
    if(led == 8'd0) led = 8'b00011000;
    else begin
        led = { led[4]^led[3]^led[2]^led[0], led[7:1]};
    end
end

endmodule