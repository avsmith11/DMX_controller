`timescale 1ns/1ns

module control_tb();
	logic [7:0] value, param, dataIn;
	logic SPIDone, dataDone, int_osc, reset, en, dataReady, RSin, RWin;

	// instantiate DUT
	control_FSM c_FSM(value [7:0], param [7:0], SPIDone, dataDone, int_osc, reset, en,
					  dataReady,  RSin,  RWin, dataIn [7:0]);

	typedef enum logic [3:0] {init, config1, config2, waiting, 
                            paramPos, paramGen, paramSend, screenOn,
                            valuePos, valueGen, valueSend} statetype;

    	 statetype state, nextState;  
    logic [31:0] countInit, nextCountInit;
    logic [4:0] countValue, nextCountValue;
	logic [7:0] nextDataIn;
    logic nextDataReady, nextRSin, nextRWin;
	

    // next state register
    always_ff @(posedge int_osc) begin
        if (~reset) begin
            state <= init;
            countInit <= 32'd0;
            countValue <= 4'd0;
        end else if (en) begin
            state <= nextState;
            countInit <= nextCountInit;
            countValue <= nextCountValue;
        end
    end

    // next output registers
    always_ff @(posedge int_osc) begin // all the outputs are included in the register to prevent a latch when we want to keep an output the same
        if (~reset) begin
            dataIn <= 8'b0;
            dataReady <= 0;
            RSin <= 0;
            RWin <= 0;
        end else if (en) begin
            dataIn <= nextDataIn;
            dataReady <= nextDataReady;
            RSin <= nextRSin;
            RWin <= nextRWin;
        end
    end

    // nextState logic
    always_comb begin
        case (state)
            init: nextState <= (countInit > 32'd800) ? config1 : init; // a 2 MHz clock gives 80kcycles but I'm adding just to be sure
            config1: nextState <= dataDone ? config2 : config1;  
            config2: nextState <= dataDone ? waiting : config2; 
            waiting: nextState <= SPIDone ? paramPos : waiting;
            
            
            paramPos: nextState <= dataDone ? paramGen : paramPos; // select cursor location 
            paramGen: nextState <= paramSend; // generate digit from MCU data
            paramSend: nextState <= dataDone ? screenOn : paramSend; // send the digit to the LCD
            screenOn: nextState <= dataDone ? valuePos : screenOn; // turn on the display

            valuePos: nextState <= dataDone ? valueGen : valuePos;
            valueGen: nextState <= (countValue == 16) ? waiting : valueSend; 
            valueSend: nextState <= dataDone ? valueGen : valueSend; 

            default: nextState <= init;
        endcase
    end

    // count next state logic: decide when to count up  or reset the various counters that are part of the state for this FSM
    always_comb begin
        nextCountInit <= (state == init) ? countInit + 1 : 0;
        

        case (state)
            valueGen: nextCountValue <= countValue + 1'b1;
            valueSend: nextCountValue <= countValue;
            default: nextCountValue <= 0;
        endcase
    end

    // next output logic
    always_comb begin

        case (nextState)
            config1: begin // send setup signal to the screen
                nextDataIn <= 8'b00111000; // set up for 2 lines, 8 bit data line, 5x8 font.
                nextRSin <= 0;
                nextRWin <= 0;
                nextDataReady <= 1;
            end

            config2: begin // send setup signal to the screen
                nextDataIn <= 8'b00000110; // set up to incriment each time a character is written
                nextRSin <= 0;
                nextRWin <= 0;
                nextDataReady <= 1;
            end
            
            // send title data
            paramPos: begin // send title cursor position to the screen
                nextDataIn <= 8'b10000000; // first position of line 1
                nextRSin <= 0;
                nextRWin <= 0;
                nextDataReady <= 1;
            end
            paramGen: begin
                nextDataIn <= param[7:0];   // get the correct value of the param to send out
                nextRSin <= 1;
                nextRWin <= 0;
                nextDataReady <= 0; // get correct outputs, then send them.
            end
            paramSend: begin
               nextDataIn <= dataIn;
               nextRSin <= RSin;
               nextRWin <= RWin;
               nextDataReady <= 1; // keep the same outputs and send the data to the screen. 
            end

            screenOn: begin // turn the display on after putting the title in
                nextDataIn <= 8'b00001111;
                nextRSin <= 0;
                nextRWin <= 0;
                nextDataReady <= 1;
            end

            // send note data
            valuePos: begin // send note cursor position to the screen
                nextDataIn <= 8'b11000000; // first position of line 2
                nextRSin <= 0;
                nextRWin <= 0;
                nextDataReady <= 1;
            end
            valueGen: begin
                case (countValue)
                    15: nextDataIn <= (value > 240) ? 8'b11111111 : 0;
                    14: nextDataIn <= (value > 224) ? 8'b11111111 : 0;
					13: nextDataIn <= (value > 208) ? 8'b11111111 : 0;
                    12: nextDataIn <= (value > 192) ? 8'b11111111 : 0;
                    11: nextDataIn <= (value > 176) ? 8'b11111111 : 0;
					10: nextDataIn <= (value > 160) ? 8'b11111111 : 0;
                     9: nextDataIn <= (value > 144) ? 8'b11111111 : 0;
                     8: nextDataIn <= (value > 128) ? 8'b11111111 : 0;
					 7: nextDataIn <= (value > 112) ? 8'b11111111 : 0;
                     6: nextDataIn <= (value > 96) ? 8'b11111111 : 0;
                     5: nextDataIn <= (value > 80) ? 8'b11111111 : 0;
					 4: nextDataIn <= (value > 64) ? 8'b11111111 : 0;
					 3: nextDataIn <= (value > 48) ? 8'b11111111 : 0;
                     2: nextDataIn <= (value > 32) ? 8'b11111111 : 0;
                     1: nextDataIn <= (value > 16) ? 8'b11111111 : 0;
					 0: nextDataIn <= (value > 0) ? 8'b11111111 : 0;
                    default: nextDataIn <= 8'b0;
                endcase
                nextRSin <= 1;
                nextRWin <= 0;
                nextDataReady <= 0;
            end
            valueSend: begin
               nextDataIn <= dataIn;
               nextRSin <= RSin;
               nextRWin <= RWin;
               nextDataReady <= 1; // keep the same outputs and send the data to the screen. 
            end

            default: begin
                nextDataIn  <= 8'b0;
                nextRSin <= 0;
                nextRWin <= 0;
                nextDataReady <= 0;
            end
        endcase    
    end


	//Generate clock signal
	always begin 
		int_osc = 1; #3; int_osc = 0; #3;
	end

	//Generate reset
	initial begin
		reset = 0; #10; reset = 1;
	end

	// apply inputs
	initial begin
		param = 8'b00110101; value = 8'b00110110; en = 1;

		dataDone = 0; SPIDone = 0; #10000; dataDone = 1; #6; dataDone = 0; #30; dataDone = 1; #6; dataDone = 0; #50; // configuration

		SPIDone = 1; #6; SPIDone = 0; #30; dataDone = 1; #6; dataDone = 0; #30;	// setting parameter starting position

		dataDone = 1; #6; dataDone = 0; #30; dataDone = 1; #6; dataDone = 0; #30; // sending parameter to LCD and turning on the display

		dataDone = 1; #6; dataDone = 0; #30; // setting value starting position

		dataDone = 1; #6; dataDone = 0; #30;
		dataDone = 1; #6; dataDone = 0; #30;
		dataDone = 1; #6; dataDone = 0; #30;
		dataDone = 1; #6; dataDone = 0; #30;
		dataDone = 1; #6; dataDone = 0; #30;
		dataDone = 1; #6; dataDone = 0; #30;
		dataDone = 1; #6; dataDone = 0; #30;
		dataDone = 1; #6; dataDone = 0; #30;
		dataDone = 1; #6; dataDone = 0; #30;
		dataDone = 1; #6; dataDone = 0; #30;
		dataDone = 1; #6; dataDone = 0; #30;
		dataDone = 1; #6; dataDone = 0; #30;
		dataDone = 1; #6; dataDone = 0; #30;
		dataDone = 1; #6; dataDone = 0; #30;
		dataDone = 1; #6; dataDone = 0; #30;
		dataDone = 1; #6; dataDone = 0; #30;

		
	end



endmodule






