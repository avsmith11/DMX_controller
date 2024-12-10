// FPGA Design SPI FSM Module
// Henry Heathwood
// hheathwood@hmc.edu
// 11/18/24

module SPI_FSM (input logic sclk, logic SPIData, logic SPILoad, 
				output logic [7:0] value, logic [7:0] param);
				
		always_ff @(posedge sclk) begin
			if (SPILoad) {value, param} = {value[6:0], param, SPIData};
		end
		
				
				
endmodule