// FPGA Design SPI FSM Module
// Henry Heathwood
// hheathwood@hmc.edu
// 11/18/24

module SPI_FSM (input logic sysclk, logic SPIData, 
				output logic [7:0] value, logic [7:0] paramOut);
				
		always_ff @(posedge sysclk)
			{value, paramOut} = {value[6:0], paramOut, SPIData};
			
		
				
				
endmodule