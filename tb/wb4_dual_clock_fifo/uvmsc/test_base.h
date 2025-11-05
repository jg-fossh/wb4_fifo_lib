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
File name      : test_base.h
Author         : Jose R Garcia (jg-fossh@protonmail.com)
Project Name   : Wishbone B4 FIFO Library
Class(es) Name : test_base
Description    : 

Additional Comments:
 
*/
#ifndef TEST_BASE_H_
#define TEST_BASE_H_

#include <systemc>
#include <uvm>

#include <verilated.h>

#include "tb_env.h"
//
#include "../../../sub/uvmsc_reset_generator/src/reset_generator_seq_lib.h"
#include "../../../sub/uvmsc_wb4_uvc/src/wb4_seq_lib.h"


//--------------------------------------------------------------------------------
// Base Test
//--------------------------------------------------------------------------------
class test_base : public uvm::uvm_test {
public:

  //
  tb_env* env {nullptr};
  //
  uvm::uvm_table_printer* topo_printer {nullptr};
  bool test_pass;
  std::string uvmtest_name;


  UVM_COMPONENT_UTILS(test_base);


  test_base( uvm::uvm_component_name name = "test_base") : uvm::uvm_test(name){
    test_pass = true;
  }


  virtual void build_phase(uvm::uvm_phase& phase){
    uvm::uvm_test::build_phase(phase);

    // Enable transaction recording for everything
    uvm::uvm_config_db<int>::set(this, "*", "recording_detail", uvm::UVM_FULL);

    // Create TB Enviroment
    env = tb_env::type_id::create("env", this);
    assert(env);

    // Create a specific depth printer for printing the created topology
    topo_printer = new uvm::uvm_table_printer();
  }


  void end_of_elaboration_phase(uvm::uvm_phase& phase){
    UVM_INFO(get_name()+"::"+__func__, "Test Topology :\n" +
      this->sprint(topo_printer), uvm::UVM_NONE);
  }


  virtual void start_of_simulation_phase(uvm::uvm_phase& phase) {
      uvm::uvm_component::start_of_simulation_phase(phase);
#if VM_COVERAGE
    //  Clear Coverage analysis at the start to ensure no false coverage
    VerilatedCov::zero();
#endif
  }


  virtual void reset_phase(uvm::uvm_phase& phase){
    UVM_INFO(get_name()+"::"+__func__, "** RESET PHASE**", uvm::UVM_NONE);
    // Drive 'Power On' Reset
    reset_generator_base_seq* rst_seq;
    rst_seq = reset_generator_base_seq::type_id::create("rst_seq");

    UVM_INFO(get_name()+"::"+__func__, "> Executing Power On Reset", uvm::UVM_NONE);
    phase.raise_objection(this);
    sc_core::sc_time drain_time = sc_core::sc_time(150.0, sc_core::SC_NS);
    phase.get_objection()->set_drain_time(this, drain_time);
    
    rst_seq->start(env->rst_agent->sqr);
    phase.drop_objection(this);
  }


  void report_phase(uvm::uvm_phase& phase) {
    if(env->in_sb->error || env->out_sb->error)
      test_pass = false;

    if(test_pass)
    {
      UVM_INFO(get_name()+"::"+__func__, "\n***** TEST SCORE: PASSED *****", uvm::UVM_NONE);
    }
    else
    {
      UVM_INFO(get_name()+"::"+__func__, "\n***** TEST SCORE: FAILED *****", uvm::UVM_NONE);
    }

#if VM_COVERAGE
    if(!uvm::uvm_config_db<std::string>::get(this, "*", "uvmtest_name", uvmtest_name))
     UVM_FATAL("NOTST", "Test name not in the config_db: " + get_full_name() + ".uvmtest_name");

    UVM_INFO(get_name()+"::"+__func__, "Coverage Log saved at: "+uvmtest_name+"/logs/coverage.dat", uvm::UVM_NONE);
    Verilated::mkdir((uvmtest_name+"/").c_str());
    Verilated::mkdir((uvmtest_name+"/logs").c_str());
    VerilatedCov::write(uvmtest_name+"/logs/coverage.dat");
#endif
  }


  void final_phase(uvm::uvm_phase& phase)
  {
    UVM_INFO(get_type_name(), "** Final PHASE **", uvm::UVM_NONE);
    delete topo_printer;
  }

}; // class test_base

#endif /* TEST_BASE_H_ */
