// FPGA Design SPI Synchronizer Module
// Henry Heathwood
// hheathwood@hmc.edu
// 11/18/24

module SPI_sync_FSM #(parameter CLKDiv = 1000)(input logic int_osc, logic SPILoad, reset,
				output logic SPIDone);
				
	typedef enum logic [1:0] {waiting, pulseOn, pulseOff} statetype;
	statetype state, nextstate;
	logic [31:0] count, nextcount;
	
			
	// State register
	always_ff @(posedge int_osc)
		if (~reset) begin
			state <= waiting;
			count <= 0;
			end
		else begin
			state <= nextstate; 
			count <= nextcount;
			end
			
	// Nextstate logic
	always_comb 
		case (state) 
			waiting: if (SPILoad) begin
						nextstate = pulseOn;
						nextcount = 0;
						end
					 else begin
						nextstate = waiting;
						nextcount = 0;
						end
			pulseOn: if (SPILoad) begin
						nextstate = pulseOn;
						nextcount = 0;
						end
					 else begin
						nextstate = pulseOff;
						nextcount = 0;
						end
			pulseOff: if (count < CLKDiv) begin
						nextstate = pulseOff;
						nextcount = count + 1;
						end
					 else begin
						nextstate = waiting;
						nextcount = 0;
						end
			
			default: begin
					nextstate = waiting;
					nextcount = 0;
					end
		endcase
		
		// Output logic
		always_comb 
			case (state)
				waiting: SPIDone = 0;
				pulseOn: SPIDone = 0;
				pulseOff: SPIDone = 1;
				default: SPIDone = 0;
			endcase
	
		
endmodule