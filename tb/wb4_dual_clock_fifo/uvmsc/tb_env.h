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
File name      : tb_env.h
Author         : Jose R Garcia (jg-fossh@protonmail.com)
Project Name   : Wishbone B4 FIFO Library
Class(es) Name : tb_env
Description    : Test bench enviroment. Creates and connects the verifiaction
                 components.

Additional Comments:
 
*/
#ifndef TB_ENV_H_
#define TB_ENV_H_

#include <systemc>
#include <uvm>

#include "tb_config.h"
#include "predictor.h"

//--------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------
class tb_env : public uvm::uvm_env {
public:

  //
  uvm::uvm_table_printer*    cfg_printer {nullptr};
  // TB Env Components
  tb_config*                 cfg;
  reset_generator_agent*     rst_agent;
  wb4_agent*                 wb4_mst_agent;
  wb4_agent*                 wb4_csr_agent;
  predictor*                 prd;
  wb4_inorder_scoreboard*    in_sb;
  wb4_inorder_scoreboard*    out_sb;

#if VM_TRACE
  // UUT object for tracing
  Vwb4_dual_clock_fifo*      uut;
#endif


  // Provide implementations of virtual methods such as get_type_name and create
  UVM_COMPONENT_UTILS(tb_env);

  
  tb_env(const uvm::uvm_component_name& name) : uvm::uvm_env(name) {

    is_negated = false;
  }
  

  virtual void build_phase(uvm::uvm_phase& phase) {
    uvm::uvm_env::build_phase(phase);
    UVM_INFO(get_name()+"::"+__func__, "tb_env build phase", uvm::UVM_NONE);

    // Create a specific depth printer for printing the created topology
    cfg_printer = new uvm::uvm_table_printer();
    cfg_printer->knobs.depth = 4;

    // TB ENV Components
    cfg = tb_config::type_id::create("cfg", this);
    // Agents
    rst_agent     = reset_generator_agent::type_id::create("rst_agent", this);
    wb4_mst_agent = wb4_agent::type_id::create("wb4_mst_agent", this);
    wb4_csr_agent = wb4_agent::type_id::create("wb4_csr_agent", this);
    in_sb         = wb4_inorder_scoreboard::type_id::create("in_sb", this);
    out_sb        = wb4_inorder_scoreboard::type_id::create("out_sb", this);
    prd           = predictor::type_id::create("prd");

#if VM_TRACE
    if(! uvm::uvm_config_db<Vwb4_dual_clock_fifo*>::get(this, "*", "uut", uut))
      UVM_FATAL("NOUUT", "UUT for tracing not found: " + get_full_name() + ".uut");

    uvm::uvm_component_name trace_name("wave");
    wave_trace<Vwb4_dual_clock_fifo>* wave = new wave_trace<Vwb4_dual_clock_fifo>(trace_name, uut);
#endif

    //
    if(! uvm::uvm_config_db<reset_generator_if*>::get(this, "*", "rst_vif", cfg->rst_cfg->vif))
      UVM_FATAL("NOVIF", "Virtual interface must be set for: " + get_full_name() + ".rst_vif");

    if(! uvm::uvm_config_db<wb4_bfm*>::get(this, "*", "wb4_mst_if", cfg->wb4_mst_cfg->vif))
     UVM_FATAL("NOVIF", "Virtual interface must be set for: " + get_full_name() + ".wb4_mst_if");

    if(! uvm::uvm_config_db<wb4_bfm*>::get(this, "*", "wb4_csr_if", cfg->wb4_csr_cfg->vif))
     UVM_FATAL("NOVIF", "Virtual interface must be set for: " + get_full_name() + ".wb4_csr_if");

    rst_agent->cfg     = cfg->rst_cfg;
    wb4_mst_agent->cfg = cfg->wb4_mst_cfg;
    wb4_csr_agent->cfg = cfg->wb4_csr_cfg;
  }

  
  virtual void connect_phase(uvm::uvm_phase& phase) {
    UVM_INFO(get_name()+"::"+__func__, "in tb_env connect phase", uvm::UVM_NONE);
    // Predictor Connections
    wb4_mst_agent->mon->ap.connect(prd->in_ap);
    wb4_csr_agent->mon->ap.connect(prd->out_ap);

    // In Scoreboard Connections
    wb4_mst_agent->mon->ap.connect(in_sb->observed_ap);
    prd->in_to_sb_ap.connect(in_sb->expected_ap);

    // Out Scoreboard Connections
    wb4_csr_agent->mon->ap.connect(out_sb->observed_ap);
    prd->out_to_sb_ap.connect(out_sb->expected_ap);
  }


  void end_of_elaboration_phase(uvm::uvm_phase& phase){
    UVM_INFO(get_name()+"::"+__func__, "Enviroment Configuration :\n" +
      this->cfg->sprint(cfg_printer), uvm::UVM_LOW);

  }

protected:
 
  bool is_negated;

};

#endif

