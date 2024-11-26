// FPGA Design Clock Divider Module
// Henry Heathwood
// hheathwood@hmc.edu
// 11/18/24

module clk_divider(input logic sysclk, reset, output logic clk);
	
	logic [17:0] counter;
	
	always_ff @(posedge sysclk, posedge reset)
		if (~reset) begin
			counter <= 0;
			clk <= 1;
		    end
		else if (counter[17]) begin
			counter <= 0;
			clk <= ~clk;
			end 		else
			counter <= counter + 1;
		
	
endmodule