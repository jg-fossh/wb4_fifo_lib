/*
 
Copyright (c) 2023, Jose R. Garcia (jg-fossh@protonmail.com)
All rights reserved.

The following hardware description source code is subject to the terms of the
                 Open Hardware Description License, v. 1.0
If a copy of the afromentioned license was not distributed with this file you
can obtain one at http://juliusbaxter.net/ohdl/ohdl.txt

--------------------------------------------------------------------------------
File name    : wb4_dual_clock_fifo.v
Author       : Jose R Garcia (jg-fossh@protonmail.com)
Project Name : Wishbone B4 FIFO Library
Module Name  : wb4_dual_clock_fifo
Description  : Wishbone B4(pipelined) dual clock FIFO Top Wrapper

Additional Comments:
   
*/
module wb4_dual_clock_fifo #(
  parameter integer P_DATA_I_MSB    = 7,            // FIFO Width-1
  parameter integer P_DATA_O_MSB    = P_DATA_I_MSB, // FIFO Width-1
  parameter integer P_DEPTH         = 128,          // FIFO $clog2(Depth)-1
  //parameter integer P_USE_BRAM      = 1,            // BRAM of LUT based
  parameter integer P_MASK_MSB      = P_DATA_O_MSB, // P_DATA_O_MSB=bit, (P_DATA_O_MSB+1)/4=nibble...
  // Write Synchronizers Params
  parameter integer P_WR_SYNC_DEPTH = 2,
  // Read Synchronizers Params
  parameter integer P_RD_SYNC_DEPTH = 2 
)(
  // Write Interface  Signals
  input                   i_wb4_in_sclk,   // Cycle, not abort
  input                   i_wb4_in_srst,   // reset
  input                   i_wb4_in_scyc,   // Enable
  input                   i_wb4_in_sstb,   // Write Strobe
  output                  o_wb4_in_sack,   // Write Strobe
  input  [P_DATA_I_MSB:0] i_wb4_in_sdata,  // Write Data
  output                  o_wb4_in_sstall, // Full?
  // Read Interface Signals
  input                   i_wb4_out_sclk,   // Cycle, not abort
  input                   i_wb4_out_srst,   // reset
  input                   i_wb4_out_scyc,   // Read Strobe
  input                   i_wb4_out_sstb,   // Read Strobe
  output                  o_wb4_out_sack,   // Write Strobe
  output [P_DATA_O_MSB:0] o_wb4_out_sdata,  // Read Data
  output                  o_wb4_out_sstall  // Empty?
);

/*verilator coverage_off*/
  ///////////////////////////////////////////////////////////////////////////////
  // Parameters Check
  //     This is Verilog code and assertions were introduced in SystemVerilog,
  //     therefore we are using an initial statement to catch mis-configurations.
  //     Also Yosys and Verilator can't handle $error() nor $fatal() hence 
  //     defaulted to $display() to provide feedback to the integrator. The 
  //     messsages are in xml to promote automation tools that can parse the log
  //     and create reports.
  ///////////////////////////////////////////////////////////////////////////////
  initial begin
    if(P_DATA_I_MSB > P_DATA_O_MSB) begin
      $display("[COMPILE-ERROR]");
      $display("  source: wb4_dual_clock_fifo");
      $display("  type: Parameter");
      $display("  parameters:");
      $display("    P_DATA_I_MSB: %0d", P_DATA_I_MSB);
      $display("    P_DATA_O_MSB: %0d", P_DATA_O_MSB);
      $display("  description: P_DATA_O_MSB can't be samller than P_DATA_I_MSB.");
    end

    if(P_MASK_MSB > P_DATA_O_MSB) begin
      $display("[COMPILE-ERROR]");
      $display("  source: wb4_dual_clock_fifo");
      $display("  type: Parameter");
      $display("  parameter:");
      $display("    P_MASK_MSB: %0d", P_MASK_MSB);
      $display("    P_DATA_O_MSB: %0d", P_DATA_O_MSB);
      $display("  description: P_DATA_O_MSB can't be samller than P_MASK_MSB.");
    end

    // if(P_USE_BRAM == 0) begin
    //   $display("<compile>");
    //   $display("  <notice>");
    //   $display("    <source>wb4_dual_clock_fifo</source>");
    //   $display("    <type>Infered Logic</type>");
    //   $display("    <parameter>P_USE_BRAM</parameter>");
    //   $display("    <param_value>%0d</param_value>", P_USE_BRAM);
    //   $display("    <description>Inferring LUTs instead of BRAM for FIFO space.</description>");
    //   $display("  </notice>");
    //   $display("</compile>");
    // end

    if(P_DATA_I_MSB < P_DATA_O_MSB) begin
      $display("[COMPILE-NOTICE]");
      $display("  source: wb4_dual_clock_fifo");
      $display("  type: Infered Logic");
      $display("  parameters:");
      $display("    P_DATA_I_MSB: %0d", P_DATA_I_MSB);
      $display("    P_DATA_O_MSB: %0d", P_DATA_O_MSB);
      $display("  description: Generating an %0d to 1 FIFO.", ((P_DATA_O_MSB+1)/(P_DATA_I_MSB+1)));
    end
  end
/*verilator coverage_on*/

  ///////////////////////////////////////////////////////////////////////////////
  //            ********      Architecture Declaration      ********           //
  ///////////////////////////////////////////////////////////////////////////////
  
  generate
  if (P_DATA_I_MSB == P_DATA_O_MSB) begin: one_to_one_fifo_gen
  ///////////////////////////////////////////////////////////////////////////////
  // Instance    : WB4 Sync 1 to 1 FIFO
  // Description : 
  ///////////////////////////////////////////////////////////////////////////////
  wb4_dual_clock_fifo_1_to_1 #(
    .P_DATA_MSB(P_DATA_I_MSB), // FIFO Width-1
    .P_DEPTH   (P_DEPTH     ), // FIFO $clog2(Depth)-1
    //.P_USE_BRAM(P_USE_BRAM  ), // BRAM of LUT based
    // CDC Params
    .P_WR_SYNC_DEPTH(P_WR_SYNC_DEPTH),
    .P_RD_SYNC_DEPTH(P_RD_SYNC_DEPTH)
  ) wb4_dual_clock_fifo_1_to_1_inst (
    // Write Interface  Signals
    .i_wb4_in_sclk  (i_wb4_in_sclk  ), //
    .i_wb4_in_srst  (i_wb4_in_srst  ), //
    .i_wb4_in_scyc  (i_wb4_in_scyc  ), // Enable
    .i_wb4_in_sstb  (i_wb4_in_sstb  ), // Write Strobe
    .o_wb4_in_sack  (o_wb4_in_sack  ), // Write Acknowledge
    .i_wb4_in_sdata (i_wb4_in_sdata ), // Write Data
    .o_wb4_in_sstall(o_wb4_in_sstall), // Full?
    // Read Interface Signals
    .i_wb4_out_sclk  (i_wb4_out_sclk  ), // 
    .i_wb4_out_srst  (i_wb4_out_srst  ), //
    .i_wb4_out_scyc  (i_wb4_out_scyc  ), // Not Abort / Enable
    .i_wb4_out_sstb  (i_wb4_out_sstb  ), // Read Strobe
    .o_wb4_out_sack  (o_wb4_out_sack  ), // Read Acknowledge
    .o_wb4_out_sdata (o_wb4_out_sdata ), // Read Data
    .o_wb4_out_sstall(o_wb4_out_sstall)  // Empty?
  );
  end  // one_to_one_fifo_gen
  endgenerate

  generate
  if (P_DATA_I_MSB < P_DATA_O_MSB) begin: N_to_one_fifo_gen
  ///////////////////////////////////////////////////////////////////////////////
  // Instance    : WB4 Sync 1 to 1 FIFO
  // Description : 
  ///////////////////////////////////////////////////////////////////////////////
  wb4_dual_clock_fifo_N_to_1 #(
    .P_DATA_I_MSB(P_DATA_I_MSB), // FIFO Width-1
    .P_DATA_O_MSB(P_DATA_O_MSB), // FIFO Width-1
    .P_DEPTH     (P_DEPTH     ), // FIFO $clog2(Depth)-1
    //.P_USE_BRAM  (1           ), // BRAM of LUT based
    .P_MASK_MSB  (P_MASK_MSB  ), // 
    // CDC Params
    .P_WR_SYNC_DEPTH(P_WR_SYNC_DEPTH),
    .P_RD_SYNC_DEPTH(P_RD_SYNC_DEPTH)
  ) wwb4_dual_clock_fifo_N_to_1_inst (
    // Write Interface  Signals
    .i_wb4_in_sclk  (i_wb4_in_sclk  ), // 
    .i_wb4_in_srst  (i_wb4_in_srst  ),
    .i_wb4_in_scyc  (i_wb4_in_scyc  ), // Enable
    .i_wb4_in_sstb  (i_wb4_in_sstb  ), // Write Strobe
    .o_wb4_in_sack  (o_wb4_in_sack  ), // Write Acknowledge
    .i_wb4_in_sdata (i_wb4_in_sdata ), // Write Data
    .o_wb4_in_sstall(o_wb4_in_sstall), // Full?
    // Read Interface Signals
    .i_wb4_out_sclk  (i_wb4_out_sclk  ), //
    .i_wb4_out_srst  (i_wb4_out_srst  ), //
    .i_wb4_out_scyc  (i_wb4_out_scyc  ), // Not Abort / Enable
    .i_wb4_out_sstb  (i_wb4_out_sstb  ), // Read Strobe
    .o_wb4_out_sack  (o_wb4_out_sack  ), // Read Acknowledge
    .o_wb4_out_sdata (o_wb4_out_sdata ), // Read Data
    .o_wb4_out_sstall(o_wb4_out_sstall)  // Empty?
  );
  end // N_to_one_fifo_gen
  endgenerate

endmodule // wb4_dual_clock_fifo
