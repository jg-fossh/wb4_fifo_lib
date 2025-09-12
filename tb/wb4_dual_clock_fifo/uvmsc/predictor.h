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
Class(es) Name : predictor
Description    : Creates a non-time consuming FIFO as a 'golden' reference.

Additional Comments:
 
*/

#ifndef PREDICTOR_H_
#define PREDICTOR_H_

#include <systemc>
#include <uvm>

//--------------------------------------------------------------------------
// Class : predictor
// Description :
//--------------------------------------------------------------------------
class predictor : public uvm::uvm_component {
public:
  int depth;
  uvm::uvm_analysis_export<wb4_seq_item> in_ap;
  uvm::uvm_analysis_export<wb4_seq_item> out_ap;

  uvm::uvm_analysis_port<wb4_seq_item> in_to_sb_ap;
  uvm::uvm_analysis_port<wb4_seq_item> out_to_sb_ap;

    class in_subscriber : public uvm::uvm_subscriber<wb4_seq_item> {
    public:
        predictor* prd;

        in_subscriber(const uvm::uvm_component_name& name, predictor* prd)
            : uvm_subscriber<wb4_seq_item>(name), prd(prd) {}

        void write(const wb4_seq_item& t) override {
            wb4_seq_item* t_clone = new wb4_seq_item();
            t_clone->do_copy(t);
            prd->write_in(t_clone);
        }
    };

    class out_subscriber : public uvm::uvm_subscriber<wb4_seq_item> {
    public:
        predictor* prd;
        
        out_subscriber(const uvm::uvm_component_name& name, predictor* prd)
            : uvm_subscriber<wb4_seq_item>(name), prd(prd) {}
            
        void write(const wb4_seq_item& t) override {
            wb4_seq_item* t_clone = new wb4_seq_item();
            t_clone->do_copy(t);
            prd->write_out(t_clone);
        }
    };

    in_subscriber  in_writer;
    out_subscriber out_writer;

  // Provide implementations of virtual methods such as get_type_name and create
  UVM_COMPONENT_UTILS(predictor);


    predictor(uvm::uvm_component_name name="predictor", int depth=127)
        : uvm::uvm_component(name), depth(depth),
          in_ap("in_ap"),
          out_ap("out_ap"),
          in_writer("in_writer", this),
          out_writer("out_writer", this) {}


    virtual void connect_phase(uvm::uvm_phase& phase) {
        in_ap.connect(in_writer.analysis_export);
        out_ap.connect(out_writer.analysis_export);
    }

    void write_in(wb4_seq_item* trans) {
      wb4_seq_item* t_clone = new wb4_seq_item();
      t_clone->do_copy(*static_cast<const wb4_seq_item*>(trans));
      if(m_fifo.size() < depth) { 
        m_fifo.push_front(t_clone->dat_o); // Data in to the DUT in data out of the UVC
        this->in_to_sb_ap.write(*static_cast<const wb4_seq_item*>(t_clone)); // Dereference the pointer
      };
    }

    void write_out(wb4_seq_item* trans) {
      wb4_seq_item* t_clone = new wb4_seq_item();
      t_clone->do_copy(*static_cast<const wb4_seq_item*>(trans));
      if(!m_fifo.empty()) { 
        t_clone->dat_i = m_fifo.back(); // Data out of the DUT in data into the UVC
        m_fifo.pop_back(); 
        this->out_to_sb_ap.write(*static_cast<const wb4_seq_item*>(t_clone)); // Dereference the pointer
      };
    }


protected:
  //std::queue<wb4_seq_item*> m_fifo[127];
  std::deque<uint64_t> m_fifo;
};

#endif

