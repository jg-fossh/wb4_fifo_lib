/* 
 
 Copyright (c) 2025, Jose R. Garcia (jg-fossh@protonmail.com)
 All rights reserved.

 
    Licensed under the Apache License, Version 2.0 (the
    "License"); you may not use this file except in
    compliance with the License.  You may obtain a copy of
    the License at
 
        http://www.apache.org/licenses/LICENSE-2.0
 
    Unless required by applicable law or agreed to in
    writing, software distributed under the License is
    distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
    CONDITIONS OF ANY KIND, either express or implied.  See
    the License for the specific language governing
    permissions and limitations under the License.

--------------------------------------------------------------------------------
File name      : sc_main.h
Author         : Jose R Garcia (jg-fossh@protonmail.com)
Project Name   : Wishbone B4 FIFO Library
Class(es) Name : sc_main
Description    : Top of the SystemC simulation

Additional Comments:
 
*/

#include <iostream>
#include <string>
#include <boost/program_options.hpp>
//using namespace boost::program_options;
#include <sys/stat.h>  // mkdir
//
#include <systemc>
#include <uvm>
//
#include <verilated.h>
#include <verilated_fst_sc.h>
//
#include "Vwb4_sync_fifo.h"

#include "../../../sub/uvmsc_reset_generator/src/reset_generator_if.h"
#include "../../../sub/uvmsc_wb4_uvc/src/wb4_bfm.h"
#include "../../../sub/uvmsc_wb4_uvc/src/wb4_if.h"
#include "../../../sub/uvmsc_wave_trace/src/wave_trace.h"
#include "test_base.h"
#include "test_lib.h"

using namespace sc_core;
using namespace sc_dt;


//--------------------------------------------------------------------------------
// Main function for the SystemC simulation. (The Top of the simulation)
//--------------------------------------------------------------------------------
int sc_main(int argc, char* argv[]) {
  namespace po = boost::program_options;
  std::string uvmtest_name;

  // Define the supported command-line options
  po::options_description desc("Allowed options");
  desc.add_options()
    ("+uvmtest_name", po::value<std::string>(), "UVM Test Name");

  // Parse the command line
  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);
  
  // Print SystemC version
  std::cout << "SYSTEMC_VERSION: " << SYSTEMC_VERSION << "\n" << std::endl;

  // Print SystemC version
  std::cout << "SYSTEMC_VERSION: " << SYSTEMC_VERSION << std::endl;

  // Use the string variable
  if (vm.count("+uvmtest_name")) {
      uvmtest_name = vm["+uvmtest_name"].as<std::string>();
      std::cout << "Running UVM Test: " << uvmtest_name << std::endl;
  } else {
      uvmtest_name = "test_fifo_default";
      std::cout << "Running UVM Test: " << uvmtest_name << std::endl;
  }

  // Parse command line arguments
  Verilated::commandArgs(argc, argv);

  // Set debug level, 0 is off, 9 is highest presently used
  // May be overridden by commandArgs argument parsing
  Verilated::debug(0);
  Verilated::traceEverOn(true);

  // Unit Under Test
  // Create an instance of the wb4_sync_fifo module.
  Vwb4_sync_fifo* uut = new Vwb4_sync_fifo("uut");

  // Agents Interface
  // Create an instance of the reset generator interface.
  reset_generator_if* rst_vif = new reset_generator_if("rst_vif");
  // Create an instance of the WB4 master interface.
  wb4_if<uint32_t, uint32_t, bool, bool, bool, bool, bool>* wb4_mst_in_if = 
    new wb4_if<uint32_t,uint32_t,bool,bool,bool,bool,bool>("wb4_mst_in_if");
    
  wb4_if<uint32_t, uint32_t, bool, bool, bool, bool, bool>* wb4_mst_out_if = 
    new wb4_if<uint32_t,uint32_t,bool,bool,bool,bool,bool>("wb4_mst_out_if");


  // Create a clock signal for the simulation.
  sc_clock sim_clk("sim_clk", 5, SC_NS, 0.5, 3, SC_NS, true);
  
  //Connect the Agents to the clock and reset signals.
  rst_vif->clk_i(sim_clk);
  //
  wb4_mst_in_if->clk_i(sim_clk       );
  wb4_mst_in_if->rst_i(rst_vif->rst_o );
  wb4_mst_out_if->clk_i(sim_clk      );
  wb4_mst_out_if->rst_i(rst_vif->rst_o);
  
  // Connect the Agents to the UUT
  // UART Wishbone Slave Interface
  uut->i_clk  (sim_clk       );   // WB cycle
  uut->i_rst  (rst_vif->rst_o);   // WB strobe
  //
  uut->i_wb4_in_scyc  (wb4_mst_in_if->cyc  );   // WB write enable
  uut->i_wb4_in_sstb  (wb4_mst_in_if->stb  );   // WB acknowledge
  uut->o_wb4_in_sack  (wb4_mst_in_if->ack  );   // WB acknowledge
  uut->i_wb4_in_sdata (wb4_mst_in_if->dat_o);   // WB acknowledge
  uut->o_wb4_in_sstall(wb4_mst_in_if->stall);   // WB acknowledge
  // Control & Status Wishbone 4 Slave Interface
  uut->i_wb4_out_scyc  (wb4_mst_out_if->cyc  );   // WB write enable
  uut->i_wb4_out_sstb  (wb4_mst_out_if->stb  );   // WB acknowledge
  uut->o_wb4_out_sack  (wb4_mst_out_if->ack  );   // WB acknowledge
  uut->o_wb4_out_sdata (wb4_mst_out_if->dat_i);   // WB acknowledge
  uut->o_wb4_out_sstall(wb4_mst_out_if->stall);   // WB acknowledge


  
  // Add interface to configuration database.
  uvm::uvm_config_db<reset_generator_if*>::set(uvm::uvm_root::get(), "*", "rst_vif", rst_vif);
  uvm::uvm_config_db<wb4_bfm*>::set(uvm::uvm_root::get(), "*", "wb4_mst_in_if", wb4_mst_in_if);
  uvm::uvm_config_db<wb4_bfm*>::set(uvm::uvm_root::get(), "*", "wb4_mst_out_if", wb4_mst_out_if);
  uvm::uvm_config_db<std::string>::set(uvm::uvm_root::get(), "*", "uvmtest_name", uvmtest_name);
#if VM_TRACE
  uvm::uvm_config_db<Vwb4_sync_fifo*>::set(uvm::uvm_root::get(), "*", "uut", uut);
#endif

  // Run the test.
  uvm::run_test(uvmtest_name);

  return 0;
}


