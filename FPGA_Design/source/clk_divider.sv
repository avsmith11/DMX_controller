// FPGA Design Clock Divider Module
// Henry Heathwood
// hheathwood@hmc.edu
// 11/18/24

module clk_divider #(parameter CLKDiv = 1000)(input logic int_osc, reset, output logic en);
	
	logic [15:0] counter;
	
	always_ff @(posedge int_osc)
		if (~reset) begin
			counter <= 0;
			en <= 0;
		    end
		else if (counter == CLKDiv) begin // this turns the 40MHz fast clock into a 200 kHz slow clock
			counter <= 0;
			en <= 1;
			end 		else begin
			counter <= counter + 1;
			en <= 0;
			end
	
endmodule