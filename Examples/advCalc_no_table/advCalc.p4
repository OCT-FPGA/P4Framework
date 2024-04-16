// ----------------------------------------------------------------------- //
//  This file is owned and controlled by Xilinx and must be used solely    //
//  for design, simulation, implementation and creation of design files    //
//  limited to Xilinx devices or technologies. Use with non-Xilinx         //
//  devices or technologies is expressly prohibited and immediately        //
//  terminates your license.                                               //
//                                                                         //
//  XILINX IS PROVIDING THIS DESIGN, CODE, OR INFORMATION "AS IS" SOLELY   //
//  FOR USE IN DEVELOPING PROGRAMS AND SOLUTIONS FOR XILINX DEVICES.  BY   //
//  PROVIDING THIS DESIGN, CODE, OR INFORMATION AS ONE POSSIBLE            //
//  IMPLEMENTATION OF THIS FEATURE, APPLICATION OR STANDARD, XILINX IS     //
//  MAKING NO REPRESENTATION THAT THIS IMPLEMENTATION IS FREE FROM ANY     //
//  CLAIMS OF INFRINGEMENT, AND YOU ARE RESPONSIBLE FOR OBTAINING ANY      //
//  RIGHTS YOU MAY REQUIRE FOR YOUR IMPLEMENTATION.  XILINX EXPRESSLY      //
//  DISCLAIMS ANY WARRANTY WHATSOEVER WITH RESPECT TO THE ADEQUACY OF THE  //
//  IMPLEMENTATION, INCLUDING BUT NOT LIMITED TO ANY WARRANTIES OR         //
//  REPRESENTATIONS THAT THIS IMPLEMENTATION IS FREE FROM CLAIMS OF        //
//  INFRINGEMENT, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A  //
//  PARTICULAR PURPOSE.                                                    //
//                                                                         //
//  Xilinx products are not intended for use in life support appliances,   //
//  devices, or systems.  Use in such applications are expressly           //
//  prohibited.                                                            //
//                                                                         //
//  (c) Copyright 1995-2019 Xilinx, Inc.                                   //
//  All rights reserved.                                                   //
// ----------------------------------------------------------------------- //

#include <core.p4>
#include <xsa.p4>

/*
 * P4 Advanced Calculator:
 *
 * Same behavior as the calculator example but with two additional 
 * operations implemented with user externs: arithmetic division and square-root. 
 *
 */

const bit<16> P4CALC_ETYPE = 0x1234; // custom value
const bit<8>  P4CALC_P     = 0x50;   // 'P'
const bit<8>  P4CALC_4     = 0x34;   // '4'
const bit<8>  P4CALC_VER   = 0x01;   // v0.1
const bit<8> op_add  = 0x6;
const bit<8> op_sub  = 0x1;
const bit<8> op_mult = 0x5;
const bit<8> op_div  = 0x8;
const bit<8> op_sqrt = 0xf;
const bit<8> op_and  = 0x0;
const bit<8> op_or   = 0x4;
const bit<8> op_xor  = 0x3;

// ****************************************************************************** //
// *************************** H E A D E R S  *********************************** //
// ****************************************************************************** //

header eth_mac_t {
    bit<48> dstAddr;     // Destination MAC address
    bit<48> srcAddr;     // Source MAC address
    bit<16> etherType;   // Tag Protocol Identifier
}

header p4calc_t {
    bit<8>  p;           // P is an ASCII Letter 'P'
    bit<8>  four;        // 4 is an ASCII Letter '4'
    bit<8>  version;     // Version is currently 0.1
    bit<8>  operation;   // Op is an operation to Perform
    bit<32> operand_a;   // A operand (left)
    bit<32> operand_b;   // B operand (right)
    bit<32> result;      // Result of the operation
}

// ****************************************************************************** //
// ************************* S T R U C T U R E S  ******************************* //
// ****************************************************************************** //

struct divider_input {
    bit<32> divisor;
    bit<32> dividend;
}

struct divider_output {
    bit<32> remainder;
    bit<32> quotient;
}

// header structure
struct headers {
    eth_mac_t    eth;
    p4calc_t     p4calc;
}

// User metadata structure
struct metadata {
    // empty
	bit<16> tuser_size;
	bit<16> tuser_src;
	bit<16> tuser_dst;
}

// ****************************************************************************** //
// *************************** P A R S E R  ************************************* //
// ****************************************************************************** //

parser MyParser(packet_in packet, 
                out headers hdr, 
                inout metadata meta, 
                inout standard_metadata_t smeta) {
    
    state start {
        packet.extract(hdr.eth);
        transition select(hdr.eth.etherType) {
            P4CALC_ETYPE : parse_p4calc;
            default      : drop;
        }
    }

    state parse_p4calc {
        packet.extract(hdr.p4calc);
        transition select(hdr.p4calc.p, hdr.p4calc.four, hdr.p4calc.version) {
            (P4CALC_P, P4CALC_4, P4CALC_VER) : accept;
            default                          : drop;
        }
    }
    
    state drop {
        smeta.drop = 1;
        transition accept;
    }
}

// ****************************************************************************** //
// **************************  P R O C E S S I N G   **************************** //
// ****************************************************************************** //

control MyProcessing(inout headers hdr, 
                     inout metadata meta, 
                     inout standard_metadata_t smeta) {
                        
    UserExtern<divider_input, divider_output>(34) calc_divide;
    UserExtern<bit<32>, bit<24>>(17) calc_square_root;
    
    divider_input div_in;
    divider_output div_out;
    
    bit<24> square_root_result;
    bit<8> op;
      
    action send_back(bit<32> result) {
        bit<48> tmp       = hdr.eth.dstAddr;
        hdr.eth.dstAddr   = hdr.eth.srcAddr;
        hdr.eth.srcAddr   = tmp;
        hdr.p4calc.result = result;
    }
    
    action operation_add() {
        send_back(hdr.p4calc.operand_a + hdr.p4calc.operand_b);
    }
    
    action operation_sub() {
        send_back(hdr.p4calc.operand_a - hdr.p4calc.operand_b);
    }
    
    action operation_mult() {
        send_back(hdr.p4calc.operand_a * hdr.p4calc.operand_b);
    }
    
    action operation_div() {
        // Prepare input structure to User Extern
        div_in.dividend = hdr.p4calc.operand_a;
        div_in.divisor = hdr.p4calc.operand_b;
        // Apply the User Extern
        calc_divide.apply(div_in, div_out);
        send_back(div_out.quotient);
    }
    
    action operation_sqrt() {
        calc_square_root.apply(hdr.p4calc.operand_a, square_root_result);
        send_back((bit<32>)square_root_result);
    }
    
    action operation_and() {
        send_back(hdr.p4calc.operand_a & hdr.p4calc.operand_b);
    }
    
    action operation_or() {
        send_back(hdr.p4calc.operand_a | hdr.p4calc.operand_b);
    }

    action operation_xor() {
        send_back(hdr.p4calc.operand_a ^ hdr.p4calc.operand_b);
    }

    action opr_or() {
        send_back(hdr.p4calc.operand_a | hdr.p4calc.operand_b);
    }
    action opr_xor() {
        send_back(hdr.p4calc.operand_a ^ hdr.p4calc.operand_b);
    }

    action operation_drop() {
        smeta.drop = 1;
    }
    
    action operation_forward() {
        smeta.drop = 1;
    }
    
    table calculate {
        key = {
            hdr.p4calc.operation : exact;
        }
        actions = {
            operation_forward;
            opr_xor;
            opr_or;
			operation_drop;
        }
        direct_match = true;
        default_action = operation_drop();
    }

    apply {
        if (hdr.p4calc.isValid()) {
            op=hdr.p4calc.operation;
            if (op == op_add) {
                operation_add();
            } else if (op == op_sub){
                operation_sub();
            } else if (op == op_mult){
                operation_mult();
            } else if (op == op_and){
                operation_and();
            } else if (op == op_or){
                operation_or();
            } else if (op == op_xor){
                operation_xor();
            } else if (op == op_div){
                operation_div();
            } else if (op == op_sqrt){
                operation_sqrt();
            } else {
                calculate.apply();
                operation_drop();
            }
        } else {
            operation_drop();
        }
    }
} 

// ****************************************************************************** //
// ***************************  D E P A R S E R  ******************************** //
// ****************************************************************************** //

control MyDeparser(packet_out packet, 
                   in headers hdr,
                   inout metadata meta, 
                   inout standard_metadata_t smeta) {
    apply {
        packet.emit(hdr.eth);
        packet.emit(hdr.p4calc);
    }
}

// ****************************************************************************** //
// *******************************  M A I N  ************************************ //
// ****************************************************************************** //

XilinxPipeline(
    MyParser(), 
    MyProcessing(), 
    MyDeparser()
) main;