// FPGA Design Send FSM Module
// Henry Heathwood
// hheathwood@hmc.edu
// 11/18/24

module send_FSM(input logic dataReady, logic RSin, logic RWin, logic [7:0] dataIn, logic int_osc, logic reset, logic en, 
				output logic RSout, logic RWout, logic [7:0] dataOut, logic enable, logic dataDone);
				
	typedef enum logic [1:0] {S0, send1, send2, send3} statetype;
	statetype state, nextstate;
			
	// State register
	always_ff @(posedge int_osc)
		if (~reset) state <= S0;
		else if (en) state <= nextstate; 
			
	// Nextstate logic
	always_comb 
		case (state) 
			S0: if (dataReady) nextstate = send1;
				else nextstate = S0;
			send1: nextstate = send2;
			send2: nextstate = send3;
			send3: nextstate = S0;
			default: nextstate = S0;
		endcase
		
	// Output logic
	always_comb
		case (state)
			S0: begin
				RSout = 0;
				RWout = 0;
				dataOut [7:0] = 0;
				enable = 0;
				dataDone = 0;
				end
			send1: begin
				RSout = RSin;
				RWout = RWin;
				dataOut = dataIn;
				enable = 0;
				dataDone = 0;
				end
			send2: begin
				RSout = RSin;
				RWout = RWin;
				dataOut = dataIn;
				enable = 1;
				dataDone = 0;
				end
			send3: begin
				RSout = RSin;
				RWout = RWin;
				dataOut = dataIn;
				enable = 0;
				dataDone = 1;
				end
			default: begin
				RSout = 0;
				RWout = 0;
				dataOut = 0;
				enable = 0;
				dataDone = 0;
				end
		endcase
endmodule