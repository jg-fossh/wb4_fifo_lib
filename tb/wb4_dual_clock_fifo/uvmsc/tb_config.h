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
File name      : tb_config.h
Author         : Jose R Garcia (jg-fossh@protonmail.com)
Project Name   : Wishbone B4 FIFO Library
Class(es) Name : tb_config
Description    : Master configuration of the test benches, containing the
                 configuration of all agents, tests and test enviroments.

Additional Comments:
 
*/
#ifndef TB_CONFIG_H_
#define TB_CONFIG_H_

#include "../../../sub/uvmsc_wb4_uvc/src/wb4.h"
#include "../../../sub/uvmsc_reset_generator/src/reset_generator.h"

//--------------------------------------------------------------------------------
// Test bench configuration object.
//--------------------------------------------------------------------------------
class tb_config : public uvm::uvm_object {
public:

   // Agent's Configs ------------------------------------
   reset_generator_config* rst_cfg;
   wb4_config*             wb4_mst_in_cfg;
   wb4_config*             wb4_mst_out_cfg;


   UVM_OBJECT_UTILS(tb_config);
   
   
   //
   tb_config(const std::string& name = "tb_config") {
      // Reset Config -----------------------------------------
      rst_cfg = reset_generator_config::type_id::create("rst_cfg");
      rst_cfg->PWR_ON_RST      = 50;
      rst_cfg->SHORT_PULSE_RST = 33;
      rst_cfg->LONG_PULSE_RST  = 200;
      rst_cfg->is_negated      = false;
      // WB4 IN Config -----------------------------------------
      wb4_mst_in_cfg = wb4_config::type_id::create("wb4_mst_in_cfg");
      wb4_mst_in_cfg->has_checks        = false;
      wb4_mst_in_cfg->has_coverage      = false;
      wb4_mst_in_cfg->is_async          = false;
      wb4_mst_in_cfg->role              = wb4_role_e::WB4_MASTER;
      wb4_mst_in_cfg->driving_edge      = wb4_clocking_edge_e::RISING;
      wb4_mst_in_cfg->sampling_edge     = wb4_clocking_edge_e::RISING;
      wb4_mst_in_cfg->data_unit         = wb4_data_attribute_e::BYTE;
      wb4_mst_in_cfg->data_granularity  = wb4_data_attribute_e::BIT;
      wb4_mst_in_cfg->data_operand_size = wb4_data_attribute_e::BYTE;
      // WB4 OUT Config -----------------------------------------
      wb4_mst_out_cfg = wb4_config::type_id::create("wb4_mst_out_cfg");
      wb4_mst_out_cfg->has_checks        = false;
      wb4_mst_out_cfg->has_coverage      = false;
      wb4_mst_out_cfg->is_async          = false;
      wb4_mst_out_cfg->role              = wb4_role_e::WB4_MASTER;
      wb4_mst_out_cfg->driving_edge      = wb4_clocking_edge_e::RISING;
      wb4_mst_out_cfg->sampling_edge     = wb4_clocking_edge_e::RISING;
      wb4_mst_out_cfg->data_unit         = wb4_data_attribute_e::BYTE;
      wb4_mst_out_cfg->data_granularity  = wb4_data_attribute_e::BIT;
      wb4_mst_out_cfg->data_operand_size = wb4_data_attribute_e::BYTE;
   }


   // Print function
   virtual void do_print(const uvm::uvm_printer& printer) const {
      printer.print_object("rst_cfg",         *rst_cfg);
      printer.print_object("wb4_mst_in_cfg",  *wb4_mst_in_cfg);
      printer.print_object("wb4_mst_out_cfg", *wb4_mst_out_cfg);
   }
   
};

#endif // TB_CONFIG_H_


