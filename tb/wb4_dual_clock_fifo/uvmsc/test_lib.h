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
File name      : test_lib.h
Author         : Jose R Garcia (jg-fossh@protonmail.com)
Project Name   : Wishbone B4 FIFO Library
Class(es) Name : test_fifo_default; test_fifo_one_wr_rd
Description    : 
    test_fifo_default   -> test with complete code coverage.
    test_fifo_one_wr_rd - >
Additional Comments:
 
*/

#ifndef TEST_LIB_H_
#define TEST_LIB_H_

#include <systemc>
#include <uvm>
#include "test_base.h"


//--------------------------------------------------------------------------------
// test_fifo_default
//--------------------------------------------------------------------------------
class test_fifo_default : public test_base {
public:

  wb4_wr_request_seq* write_seq;
  wb4_rd_request_seq* read_seq;

  
  UVM_COMPONENT_UTILS(test_fifo_default);

  test_fifo_default( uvm::uvm_component_name name = "test_fifo_default") : test_base(name){
    test_pass = true;
  }
  
  void main_phase(uvm::uvm_phase& phase){
    UVM_INFO(get_name()+"::"+__func__, "** MAIN PHASE**", uvm::UVM_NONE);

    //

    phase.raise_objection(this);
    //set a drain-time for the environment if desired
    sc_core::sc_time drain_time = sc_core::sc_time(333.0, sc_core::SC_NS);
    phase.get_objection()->set_drain_time(this, drain_time);



    UVM_INFO(get_name()+"::"+__func__, "> Filling the Tx Buffer half way", uvm::UVM_LOW);
    // Fill up the Tx Buffer with Data
    for(int iter = 0; iter < 128; ++iter) {
      write_seq = wb4_wr_request_seq::type_id::create("write_seq");
      write_seq->req->delay = 16;
      write_seq->req->dat_o = iter;
      write_seq->start(env->wb4_mst_in_agent->sqr);
    }
    

    UVM_INFO(get_name()+"::"+__func__, "> Allowing the buffer to settle", uvm::UVM_LOW);
    // Allow TX Buffer to empty
    sc_core::wait(sc_core::sc_time(150.0, sc_core::SC_NS));


    UVM_INFO(get_name()+"::"+__func__, ">  Emptying the  Rx Buffer", uvm::UVM_LOW);
    // Read CSRs
    for(int iter = 0; iter < 128; ++iter) {
      read_seq = wb4_rd_request_seq::type_id::create("read_seq");
      read_seq->req->adr      = iter;
      read_seq->req->delay    = 16;
      read_seq->req->rsp_clks = 8;
      read_seq->start(env->wb4_mst_out_agent->sqr);
    }


    UVM_INFO(get_name()+"::"+__func__, "> Allowing the buffer to settle", uvm::UVM_LOW);
    // Allow TX Buffer to empty
    sc_core::wait(sc_core::sc_time(150.0, sc_core::SC_NS));

    UVM_INFO(get_name()+"::"+__func__, "> Filling the Tx buffer half way", uvm::UVM_LOW);
    // Fill up the Tx Buffer with Data
    for(int iter = 0; iter < 8; ++iter) {
      write_seq = wb4_wr_request_seq::type_id::create("write_seq");
      write_seq->req->dat_o = iter;
      write_seq->start(env->wb4_mst_in_agent->sqr);
    }

    UVM_INFO(get_name()+"::"+__func__, "> Allowing some settling time", uvm::UVM_LOW);
    // Allow TX Buffer to empty
    sc_core::wait(sc_core::sc_time(150.0, sc_core::SC_NS));


    UVM_INFO(get_name()+"::"+__func__, ">  Emptying the Rx Buffer", uvm::UVM_LOW);
    // Send read sequences to the 'Out' interface.
    for(int iter = 0; iter < 8; ++iter) {
      read_seq = wb4_rd_request_seq::type_id::create("read_seq");
      read_seq->req->adr      = iter;
      read_seq->req->delay    = 16;
      read_seq->req->rsp_clks = 8;
      read_seq->start(env->wb4_mst_out_agent->sqr);
    }


    UVM_INFO(get_name()+"::"+__func__, "> Allowing the buffer to settle", uvm::UVM_LOW);
    // Allow TX Buffer to empty
    sc_core::wait(sc_core::sc_time(150.0, sc_core::SC_NS));


    UVM_INFO(get_name()+"::"+__func__, "> Filling the Tx Buffer half way", uvm::UVM_LOW);
    // Fill up the Tx Buffer with Data
    for(int iter = 0; iter < 128; ++iter) {
      write_seq = wb4_wr_request_seq::type_id::create("write_seq");
      write_seq->req->delay = 16;
      write_seq->req->dat_o = iter;
      write_seq->start(env->wb4_mst_in_agent->sqr);
    }
    

    UVM_INFO(get_name()+"::"+__func__, "> Allowing the buffer to settle", uvm::UVM_LOW);
    // Allow TX Buffer to empty
    sc_core::wait(sc_core::sc_time(150.0, sc_core::SC_NS));


    UVM_INFO(get_name()+"::"+__func__, ">  Emptying the  Rx Buffer", uvm::UVM_LOW);
    // Read CSRs
    for(int iter = 0; iter < 128; ++iter) {
      read_seq = wb4_rd_request_seq::type_id::create("read_seq");
      read_seq->req->adr      = iter;
      read_seq->req->delay    = 16;
      read_seq->req->rsp_clks = 8;
      read_seq->start(env->wb4_mst_out_agent->sqr);
    }



    // Fill up the Tx Buffer with Data
    for(int iter = 0; iter < 128; ++iter) {
      read_seq = wb4_rd_request_seq::type_id::create("read_seq");
      read_seq->req->adr      = iter;
      read_seq->req->delay    = 2;
      read_seq->req->rsp_clks = 12;

      write_seq = wb4_wr_request_seq::type_id::create("write_seq");
      write_seq->req->delay = 8;
      write_seq->req->dat_o = iter;


      read_seq->start(env->wb4_mst_out_agent->sqr);
      write_seq->start(env->wb4_mst_in_agent->sqr);
    }
    
    phase.drop_objection(this);
  }
}; // test_fifo_default



//--------------------------------------------------------------------------------
// test_fifo_one_wr_rd
//--------------------------------------------------------------------------------
class test_fifo_one_wr_rd : public test_base {
public:

  wb4_wr_request_seq* write_seq;
  wb4_rd_request_seq* read_seq;

  
  UVM_COMPONENT_UTILS(test_fifo_one_wr_rd);

  test_fifo_one_wr_rd( uvm::uvm_component_name name = "test_fifo_one_wr_rd") : test_base(name){
    test_pass = true;
  }
  
  void main_phase(uvm::uvm_phase& phase){
    UVM_INFO(get_name()+"::"+__func__, "** MAIN PHASE**", uvm::UVM_NONE);

    //

    phase.raise_objection(this);
    //set a drain-time for the environment if desired
    sc_core::sc_time drain_time = sc_core::sc_time(333.0, sc_core::SC_NS);
    phase.get_objection()->set_drain_time(this, drain_time);



    UVM_INFO(get_name()+"::"+__func__, "> Writing One word", uvm::UVM_LOW);
    // Fill up the Tx Buffer with Data
    write_seq = wb4_wr_request_seq::type_id::create("write_seq");
    write_seq->req->delay = 16;
    write_seq->req->dat_o = 14;
    write_seq->start(env->wb4_mst_in_agent->sqr);
    

    UVM_INFO(get_name()+"::"+__func__, "> Allowing the buffer to settle", uvm::UVM_LOW);
    // Allow TX Buffer to empty
    sc_core::wait(sc_core::sc_time(150.0, sc_core::SC_NS));


    UVM_INFO(get_name()+"::"+__func__, ">  Emptying the  Rx Buffer", uvm::UVM_LOW);
    // Read CSRs
    read_seq = wb4_rd_request_seq::type_id::create("read_seq");
    read_seq->req->adr      = 0;
    read_seq->req->delay    = 16;
    read_seq->req->rsp_clks = 8;
    read_seq->start(env->wb4_mst_out_agent->sqr);
    
    phase.drop_objection(this);
  }
}; // test_fifo_one_wr_rd



//--------------------------------------------------------------------------------
// test_fifo_rd_empty
//--------------------------------------------------------------------------------
class test_fifo_rd_empty : public test_base {
public:

  wb4_wr_request_seq* write_seq;
  wb4_rd_request_seq* read_seq;

  
  UVM_COMPONENT_UTILS(test_fifo_rd_empty);

  test_fifo_rd_empty( uvm::uvm_component_name name = "test_fifo_rd_empty") : test_base(name){
    test_pass = true;
  }
  
  void main_phase(uvm::uvm_phase& phase){
    UVM_INFO(get_name()+"::"+__func__, "** MAIN PHASE**", uvm::UVM_NONE);

    //

    phase.raise_objection(this);
    //set a drain-time for the environment if desired
    sc_core::sc_time drain_time = sc_core::sc_time(333.0, sc_core::SC_NS);
    phase.get_objection()->set_drain_time(this, drain_time);


    UVM_INFO(get_name()+"::"+__func__, ">  Reading Empty FIFO Buffer", uvm::UVM_LOW);
    // Read CSRs
    read_seq = wb4_rd_request_seq::type_id::create("read_seq");
    read_seq->req->adr      = 0;
    read_seq->req->delay    = 1;
    read_seq->req->rsp_clks = 100;
    read_seq->start(env->wb4_mst_out_agent->sqr);


    UVM_INFO(get_name()+"::"+__func__, "> Writing One word", uvm::UVM_LOW);
    // Fill up the Tx Buffer with Data
    write_seq = wb4_wr_request_seq::type_id::create("write_seq");
    write_seq->req->delay = 16;
    write_seq->req->dat_o = 8;
    write_seq->start(env->wb4_mst_in_agent->sqr);
    

    UVM_INFO(get_name()+"::"+__func__, "> Allowing TB to settle", uvm::UVM_LOW);
    // Allow TX Buffer to empty
    sc_core::wait(sc_core::sc_time(100.0, sc_core::SC_NS));
    
    phase.drop_objection(this);
  }
}; // test_fifo_rd_empty

#endif /* TEST_LIB_H_ */
