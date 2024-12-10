// FPGA Design Top Level Module
// Henry Heathwood
// hheathwood@hmc.edu
// 11/18/24

module top(input logic sclk, logic SPILoad, logic SPIData, logic reset,
				output logic [7:0] param, logic [7:0] value, logic SPIDone);
			//output logic RSout, logic RWout, logic [7:0] dataOut, logic enable);
		
		logic [7:0] param;
		logic [7:0] value;
		logic SPIDone;
		logic en;
		logic dataReady;
		logic RSin;
		logic RWin;
		logic [7:0] dataIn;
		logic dataDone;
		logic int_osc;
		
		
		// Internal high-speed oscillator
		HSOSC #(.CLKHF_DIV(2'b01)) 
			hf_osc (.CLKHFPU(1'b1), .CLKHFEN(1'b1), .CLKHF(int_osc));
		
		SPI_FSM s1(sclk, SPIData, SPILoad, 
					value [7:0], param [7:0] );
					
		//control_FSM s2(value [7:0], param [7:0], SPIDone, dataDone, int_osc,  reset, en,
		//			  dataReady,  RSin,  RWin, dataIn [7:0]);
					  
		SPI_sync_FSM s3(int_osc, SPILoad, reset, 
				 SPIDone);
				
		//send_FSM s4(dataReady, RSin, RWin, dataIn [7:0], int_osc, reset, en,
			//	  RSout, RWout, dataOut [7:0], enable, dataDone);
				 
		clk_divider s5(int_osc, reset, en);

endmodule