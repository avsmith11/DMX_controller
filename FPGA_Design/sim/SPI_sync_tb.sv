`timescale 1ns/1ns
// Testench for SPI sync module

module SPI_sync_tb #(parameter CLKDiv = 1000)();

	logic sysclk, SPILoad, reset, SPIDone;

	

	typedef enum logic [1:0] {waiting, pulseOn, pulseOff} statetype;
	statetype state, nextstate;
	logic [31:0] count, nextcount;
	
	// Instantiate DUT
	SPI_sync_FSM SPI_sync_FSM_tb(sysclk, SPILoad, reset,
				 SPIDone);
			
	// State register
	always_ff @(posedge sysclk)
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

		//Generate clock
		always begin
			sysclk = 1; #25; sysclk = 0; #25;
		end


		// generate reset
		initial begin 
			reset = 0; #70; reset = 1;
		end

		// apply inputs
		initial begin
		SPILoad = 0; #130 SPILoad = 1; #130 SPILoad = 0;
		end


endmodule