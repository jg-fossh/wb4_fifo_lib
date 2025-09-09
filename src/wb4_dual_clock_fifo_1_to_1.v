/*
 
Copyright (c) 2023, Jose R. Garcia (jg-fossh@protonmail.com)
All rights reserved.

The following hardware description source code is subject to the terms of the
                 Open Hardware Description License, v. 1.0
If a copy of the afromentioned license was not distributed with this file you
can obtain one at http://juliusbaxter.net/ohdl/ohdl.txt

--------------------------------------------------------------------------------
File name    : wb4_dual_clock_fifo_1_to_1.v
Author       : Jose R Garcia (jg-fossh@protonmail.com)
Project Name : Wishbone B4 FIFO Library
Module Name  : wb4_dual_clock_fifo_1_to_1
Description  : Wishbone B4(pipelined) dual clock FIFO One-to-One data units 

Additional Comments:
   
*/
module wb4_dual_clock_fifo_1_to_1 #(
  parameter integer P_DATA_MSB = 7,   // FIFO Width-1
  parameter integer P_DEPTH    = 128, // FIFO $clog2(Depth)-1
  // Write Synchronizers Params
  parameter integer P_WR_SYNC_DEPTH = 2, //
  // Read Synchronizers Params
  parameter integer P_RD_SYNC_DEPTH = 2  //
)(
  // Write Interface  Signals
  input                 i_wb4_in_sclk,   //
  input                 i_wb4_in_srst,   // 
  input                 i_wb4_in_scyc,   // Enable
  input                 i_wb4_in_sstb,   // Write Strobe
  output                o_wb4_in_sack,   // Write Strobe
  input  [P_DATA_MSB:0] i_wb4_in_sdata,  // Write Data
  output                o_wb4_in_sstall, // Full?
  // Read Interface Signals 
  input                 i_wb4_out_sclk,   //
  input                 i_wb4_out_srst,   //
  input                 i_wb4_out_scyc,   // Read Cycle, Abort
  input                 i_wb4_out_sstb,   // Read Strobe
  output                o_wb4_out_sack,   // Write Strobe
  output [P_DATA_MSB:0] o_wb4_out_sdata,  // Read Data
  output                o_wb4_out_sstall  // Empty?
);

  ///////////////////////////////////////////////////////////////////////////////
  // Internal Parameter Declarations
  ///////////////////////////////////////////////////////////////////////////////
  localparam L_ADDR_MSB = $clog2(P_DEPTH)-1;

  ///////////////////////////////////////////////////////////////////////////////
  // Internal Signals Declarations
  ///////////////////////////////////////////////////////////////////////////////
  // Write Proc
  reg r_write_ack;
  // Read Proc
  reg r_read_ack;
  // Write Controls Asynch Logic
  wire w_full;
  wire w_we = i_wb4_in_scyc & i_wb4_in_sstb & ~w_full;
  // Read Controls Asynch Logic
  wire w_last;
  wire w_re = i_wb4_out_scyc & i_wb4_out_sstb & ~w_last;

  ///////////////////////////////////////////////////////////////////////////////
  //            ********      Architecture Declaration      ********           //
  ///////////////////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////////////////
  // Process     :  Write ACK Process
  // Description : 
  /////////////////////////////////////////////////////////////////////////////
  always @(posedge i_wb4_in_sclk) begin : write_ack_proc
    if (i_wb4_in_srst == 1'b1 || i_wb4_in_scyc == 1'b0) begin
      r_write_ack <= 1'b0;
    end
    else begin
      if(w_full == 1'b0) begin
        // 
        r_write_ack <= i_wb4_in_sstb;
      end
      else begin
        r_write_ack <= 1'b0;
      end
    end
  end // write_ack_proc
  //
  assign o_wb4_in_sack    = r_write_ack;
  assign o_wb4_in_sstall  = w_full;
  assign o_wb4_out_sstall = w_last;

  ///////////////////////////////////////////////////////////////////////////////
  // Instance    : Dual Clock FIFO
  // Description : 
  ///////////////////////////////////////////////////////////////////////////////
  dual_clock_fifo #(
    // FIFO Params
    .P_DATA_MSB(P_DATA_MSB), // FIFO Width-1
    .P_DEPTH   (P_DEPTH),    // FIFO $clog2(Depth)-1
    // Write Synchronizers Params
    .P_WR_SYNC_DEPTH(P_WR_SYNC_DEPTH), //
    // Read Synchronizers Params
    .P_RD_SYNC_DEPTH(P_RD_SYNC_DEPTH) //
  ) dual_clock_fifo_inst (
    //
    .i_wr_clk  (i_wb4_in_sclk),   //
    .i_wr_rst  (i_wb4_in_srst),   //
    .i_wr_inc  (w_we),            //
    .i_wr_data (i_wb4_in_sdata),  // 
    .o_wr_full (w_full),          // 
    //
    .i_rd_clk  (i_wb4_out_sclk),   //
    .i_rd_rst  (i_wb4_out_srst),   //
    .i_rd_inc  (w_re),             //
    .o_rd_data (o_wb4_out_sdata),  //
    .o_rd_empty(w_last)           //
  );

  /////////////////////////////////////////////////////////////////////////////
  // Process     :  Read ACK
  // Description : 
  /////////////////////////////////////////////////////////////////////////////
  always @(posedge i_wb4_out_sclk) begin : read_ack_proc
    if (i_wb4_out_srst == 1'b1 || i_wb4_out_scyc == 1'b0) begin
      r_read_ack <= 1'b0;
    end
    else begin
      if(w_last == 1'b0) begin
        // 
        r_read_ack <= i_wb4_out_sstb & i_wb4_out_scyc;
      end
      else begin
        r_read_ack <= 1'b0;
      end
    end
  end // read_ack_proc
  // 
  assign o_wb4_out_sack = r_read_ack;

endmodule // wb4_dual_clock_fifo_1_to_1
