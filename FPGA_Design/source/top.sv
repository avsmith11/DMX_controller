// FPGA Design Top Level Module
// Henry Heathwood
// hheathwood@hmc.edu
// 11/18/24

module top(input logic sysclk, logic SPILoad, logic SPIData, logic reset,
			output logic RSout, logic RWout, logic [7:0] dataOut, logic enable);
		
		logic [7:0] paramOut;
		logic [7:0] value;
		logic SPIDone;
		logic clk;
		logic dataReady;
		logic RSin;
		logic RWin;
		logic [7:0] dataIn;
		logic dataDone;
		
		SPI_FSM s1(sysclk, SPIData, 
					value [7:0], paramOut [7:0] );
					
		control_FSM s2(value [7:0], paramOut [7:0], SPIDone, dataDone, clk,  reset,
					  dataReady,  RSin,  RWin, dataIn [7:0]);
					  
		SPI_sync_FSM s3(sysclk, SPILoad, reset, 
				 SPIDone);
				
		send_FSM s4(dataReady, RSin, RWin, dataIn [7:0], clk, reset,
				  RSout, RWout, dataOut [7:0], enable, dataDone);
				 
		clk_divider s5(sysclk, clk);

endmodule