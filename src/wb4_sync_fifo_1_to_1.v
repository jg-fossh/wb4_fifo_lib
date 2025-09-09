/*
 
Copyright (c) 2023, Jose R. Garcia (jg-fossh@protonmail.com)
All rights reserved.

The following hardware description source code is subject to the terms of the
                 Open Hardware Description License, v. 1.0
If a copy of the afromentioned license was not distributed with this file you
can obtain one at http://juliusbaxter.net/ohdl/ohdl.txt

--------------------------------------------------------------------------------
File name    : wb4_sync_fifo_1_to_1.v
Author       : Jose R Garcia (jg-fossh@protonmail.com)
Project Name : Wishbone B4 FIFO Library
Module Name  : wb4_sync_fifo_1_to_1
Description  : Wishbone B4(pipelined) Sync FIFO

Additional Comments:
   
*/
module wb4_sync_fifo_1_to_1 #(
  parameter integer P_DATA_MSB = 7,   // FIFO Width-1
  parameter integer P_DEPTH    = 128, // FIFO $clog2(Depth)-1
  parameter integer P_USE_BRAM = 1    // BRAM of LUT based
)(
  // Component's clocks and resets
  input i_clk, // clock
  input i_rst, // reset
  // Write Interface  Signals
  input                 i_wb4_in_scyc,   // Enable
  input                 i_wb4_in_sstb,   // Write Strobe
  output                o_wb4_in_sack,   // Write Strobe
  input  [P_DATA_MSB:0] i_wb4_in_sdata,  // Write Data
  output                o_wb4_in_stgd,   // Empty?
  output                o_wb4_in_sstall, // Full?
  // Read Interface Signals
  input                 i_wb4_out_scyc,   // Read Strobe
  input                 i_wb4_out_sstb,   // Read Strobe
  output                o_wb4_out_sack,   // Write Strobe
  output [P_DATA_MSB:0] o_wb4_out_sdata,  // Read Data
  output                o_wb4_out_stgd,   // Full?
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
  reg [L_ADDR_MSB:0] r_write_ptr;
  reg                r_full;
  reg                r_write_ack;
  // Read Proc
  reg [L_ADDR_MSB:0] r_read_ptr;
  reg                r_empty_d;
  reg                r_empty;
  reg                r_read_ack;
  //
  wire w_ce = i_wb4_in_scyc | i_wb4_out_scyc;
  // Write Controls Asynch Logic
  wire w_wr_hazzard = ($signed(r_write_ptr) == $signed(r_read_ptr)-1) ? 1'b1 : 1'b0;
  wire w_full       = w_wr_hazzard & i_wb4_in_sstb;
  wire w_we         = i_wb4_in_scyc & i_wb4_in_sstb & ~r_full;
  // Read Controls Asynch Logic
  wire w_rd_hazzard = ($signed(r_read_ptr) == $signed(r_write_ptr)-1) ? 1'b1 : 1'b0;
  wire w_empty      = w_rd_hazzard & i_wb4_out_sstb;
  wire w_re         = i_wb4_out_scyc & i_wb4_out_sstb & ~r_empty;

  ///////////////////////////////////////////////////////////////////////////////
  //            ********      Architecture Declaration      ********           //
  ///////////////////////////////////////////////////////////////////////////////
  generate // TBD --- IN PROGRESS
    if (P_USE_BRAM == 0) begin: Gen_LUT_MEM
      // Mem Block
      reg [P_DATA_MSB:0] mem [0:P_DEPTH-1] /*verilator public*/;

      /////////////////////////////////////////////////////////////////////////////
      // Process     : Mem Process
      // Description : .
      /////////////////////////////////////////////////////////////////////////////
      always @(posedge i_clk) begin : Lut_Write_Proc
        if (i_wb4_in_scyc == 1'b1 && w_we == 1'b1) begin
          mem[r_write_ptr] <= i_wb4_in_sdata;
        end
      end // Lut_Write_Proc
      // Read
      assign o_wb4_out_sdata = mem[r_read_ptr];
      //assign o_wb4_out_sack  = 
    end // Gen_LUT_MEM
  endgenerate

  generate
    if (P_USE_BRAM == 1) begin: bram_mem_gen
      ///////////////////////////////////////////////////////////////////////////////
      // Instance    : Single Port BRAM
      // Description : Inferrable Single Port Block RAM.
      ///////////////////////////////////////////////////////////////////////////////
      generic_sbram #(
        // Compile time configurable parameters
        .P_SBRAM_DATA_MSB(P_DATA_MSB),
        .P_SBRAM_ADDR_MSB(L_ADDR_MSB),
        .P_SBRAM_MASK_MSB(0),
        .P_SBRAM_HAS_FILE(0),
        .P_SBRAM_INIT_FILE(0)
      ) fifo_mem_block_i (
        .i_ce(w_ce),
        .i_wclk(i_clk),
        .i_rclk(i_clk),
        .i_waddr(r_write_ptr),
        .i_raddr(r_read_ptr),
        .i_we(w_we),
        .i_mask(), // 0=writes, 1=masks
        .i_wdata(i_wb4_in_sdata),
        .o_rdata(o_wb4_out_sdata)
      );
    end // bram_mem_gen
  endgenerate
  
  /////////////////////////////////////////////////////////////////////////////
  // Process     :  Read Pointer Process
  // Description : .
  /////////////////////////////////////////////////////////////////////////////
  always @(posedge i_clk) begin: read_ptr_proc
    if (i_rst == 1'b1) begin
      r_read_ptr <= 'h0;
      r_empty    <= 1'b1;
      r_empty_d  <= 1'b1;
    end
    else begin
    //else if (i_wb4_in_scyc == 1'b1) begin
      if(r_empty == 1'b1) begin
        //
        r_empty_d <= ~(i_wb4_in_sstb & i_wb4_in_scyc);
        r_empty   <= r_empty_d;
      end
      else begin
        if(w_re == 1'b1) begin
          // After it is warmed up
          r_read_ptr <= r_read_ptr + 1;
        end
        r_empty <= w_empty;
      end
    end
  end // read_ptr_proc

  /////////////////////////////////////////////////////////////////////////////
  // Process     :  Read Pointer Process
  // Description : .
  /////////////////////////////////////////////////////////////////////////////
  always @(posedge i_clk) begin: read_ack_proc
    if (i_rst == 1'b1 || i_wb4_out_scyc == 1'b0) begin
      r_read_ack <= 1'b0;
    end
    else begin
      if(i_wb4_out_sstb == 1'b1 && r_empty == 1'b0) begin
        // 
        r_read_ack <= 1'b1;
      end
      else begin
        r_read_ack <= 1'b0;
      end
    end
  end // read_ack_proc
  // 
  assign o_wb4_out_sack = r_read_ack;
  // Memory read-address pointer (okay to use binary to address memory)
  assign o_wb4_in_stgd    = r_empty;
  assign o_wb4_out_sstall = r_empty;

  /////////////////////////////////////////////////////////////////////////////
  // Process     :  Write Pointer Process
  // Description : .
  /////////////////////////////////////////////////////////////////////////////
  always @(posedge i_clk) begin: Write_Proc
    if (i_rst == 1'b1) begin
      r_write_ptr <= 'h0;
      r_full      <= 1'b0;
      r_write_ack <= 1'b0;
    end
    else begin
      if (w_we == 1'b1) begin
        r_write_ptr <= r_write_ptr + 1;
        r_full      <= w_full;
        r_write_ack <= ~w_full;
      end
      else begin
        //
        r_write_ack <= 1'b0;
      end
    end
  end // Write_Proc
  //
  assign o_wb4_in_sack = r_write_ack;
  // Memory read-address pointer (okay to use binary to address memory)
  assign o_wb4_in_sstall = r_full;
  assign o_wb4_out_stgd  = r_full;

endmodule // wb4_sync_fifo_1_to_1
