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
 * Five Tuple:
 *
 * This example shows how to implement in the P4 language the standard 5-tuple 
 * network application. A 5-tuple system is commonly used to identify key 
 * requirements for creating a secure and bidirectional TCP/IP or UDP/IP network 
 * connection between two or more machines. This application uses the source IP 
 * address and TCP/UDP port number, destination IP address and TCP/UDP port 
 * number and IP protocol to perform an exact-match based classification.
 *
 */

typedef bit<48>  MacAddr;
typedef bit<32>  IPv4Addr;

const bit<16> TM_TYPE = 0x88A9;
const bit<16> QINQ_TYPE = 0x88A8;
const bit<16> VLAN_TYPE = 0x8100;
const bit<16> IPV4_TYPE = 0x0800;


const bit<8>  TCP_PROT  = 0x06;
const bit<8>  UDP_PROT  = 0x11;

// ****************************************************************************** //
// *************************** H E A D E R S  *********************************** //
// ****************************************************************************** //

header eth_mac_t {
    MacAddr dmac; // Destination MAC address
    MacAddr smac; // Source MAC address
    bit<16> type; // Tag Protocol Identifier
}

header vlan_t {
    bit<3>  pcp;  // Priority code point
    bit<1>  cfi;  // Drop eligible indicator
    bit<12> vid;  // VLAN identifier
    bit<16> tpid; // Tag protocol identifier
}

header ipv4_t {
    bit<4>   version;  // Version (4 for IPv4)
    bit<4>   hdr_len;  // Header length in 32b words
    bit<8>   tos;      // Type of Service
    bit<16>  length;   // Packet length in 32b words
    bit<16>  id;       // Identification
    bit<3>   flags;    // Flags
    bit<13>  offset;   // Fragment offset
    bit<8>   ttl;      // Time to live
    bit<8>   protocol; // Next protocol
    bit<16>  hdr_chk;  // Header checksum
    IPv4Addr src;      // Source address
    IPv4Addr dst;      // Destination address
}

header ipv4_opt_t {
    varbit<320> options; // IPv4 options - length = (ipv4.hdr_len - 5) * 32
}

header tcp_t {
    bit<16> src_port;   // Source port
    bit<16> dst_port;   // Destination port
    bit<32> seqNum;     // Sequence number
    bit<32> ackNum;     // Acknowledgment number
    bit<4>  dataOffset; // Data offset
    bit<6>  resv;       // Offset
    bit<6>  flags;      // Flags
    bit<16> window;     // Window
    bit<16> checksum;   // TCP checksum
    bit<16> urgPtr;     // Urgent pointer
}

header tcp_opt_t {
    varbit<320> options; // TCP options - length = (tcp.dataOffset - 5) * 32
}

header udp_t {
    bit<16> src_port;  // Source port
    bit<16> dst_port;  // Destination port
    bit<16> length;    // UDP length
    bit<16> checksum;  // UDP checksum
}

//custom header to add timestsamp to packets
header timestamp_t {
    bit<64> time_par;  // timestamp at parser
    bit<64> time_depar;  // timestamp at deparser
    bit<16> hdrtype;
}

// ****************************************************************************** //
// ************************* S T R U C T U R E S  ******************************* //
// ****************************************************************************** //

// header structure
struct headers {
    eth_mac_t    eth;
    vlan_t       new_vlan;
    vlan_t       vlan;
    ipv4_t       ipv4;
    ipv4_opt_t   ipv4opt;
    tcp_t        tcp;
    tcp_opt_t    tcpopt;
    udp_t        udp;
    timestamp_t timestamp;
}

// User metadata structure
// struct metadata {
//     // empty
// }

struct metadata {
    
	bit<16> tuser_size;
	bit<16> tuser_src;
	bit<16> tuser_dst;
}

// User-defined errors 
error {
    InvalidIPpacket,
    InvalidTCPpacket
}

// ****************************************************************************** //
// *************************** P A R S E R  ************************************* //
// ****************************************************************************** //

parser MyParser(packet_in packet, 
                out headers hdr, 
                inout metadata meta, 
                inout standard_metadata_t smeta) {
    
    state start {
        transition parse_eth;
    }

    //parse custom timestamp header after ethernet
    state parse_eth {
        packet.extract(hdr.eth);
        transition select(hdr.eth.type) {
            TM_TYPE: parse_timestamp;
            VLAN_TYPE : parse_vlan;
            IPV4_TYPE : parse_ipv4;
            default   : accept; 
        }
    }

    //parse custom timestamp header
    state parse_timestamp {
        packet.extract(hdr.timestamp);
        transition select(hdr.timestamp.hdrtype) {
            VLAN_TYPE : parse_vlan;
            IPV4_TYPE : parse_ipv4;
            default   : accept; 
        }
    }
    
    state parse_vlan {
        packet.extract(hdr.vlan);
        transition select(hdr.vlan.tpid) {
            IPV4_TYPE : parse_ipv4;
            default   : accept; 
        }
    }
    
    state parse_ipv4 {
        packet.extract(hdr.ipv4);
        verify(hdr.ipv4.version == 4 && hdr.ipv4.hdr_len >= 5, error.InvalidIPpacket);
        packet.extract(hdr.ipv4opt, (((bit<32>)hdr.ipv4.hdr_len - 5) * 32));
        transition select(hdr.ipv4.protocol) {
            TCP_PROT  : parse_tcp;
            UDP_PROT  : parse_udp;
            default   : accept; 
        }
    }

    state parse_tcp {
        packet.extract(hdr.tcp);
        verify(hdr.tcp.dataOffset >= 5, error.InvalidTCPpacket);
        packet.extract(hdr.tcpopt, (((bit<32>)hdr.tcp.dataOffset - 5) * 32));
        transition accept;
    }
    
    state parse_udp {
        packet.extract(hdr.udp);
        transition accept;
    }
}

// ****************************************************************************** //
// **************************  P R O C E S S I N G   **************************** //
// ****************************************************************************** //

control MyProcessing(inout headers hdr, 
                     inout metadata meta, 
                     inout standard_metadata_t smeta) {
                      
    bit<16> table_key_sport;
    bit<16> table_key_dport;
    bool hit = false;

    action InsertTimestamp() {
        hdr.timestamp.setValid();
        meta.tuser_size = meta.tuser_size + 0x0012;  
        hdr.timestamp.time_par = smeta.ingress_timestamp;
        hdr.timestamp.time_depar = 0;
        hdr.eth.type = 0x88A9;
        hdr.timestamp.hdrtype = 0x0800;
    }

    action InsertVLAN(bit<3> pcp, bit<1> cfi, bit<12> vid) {
        hdr.new_vlan.setValid();
        meta.tuser_size = meta.tuser_size + 0x0004;  
        hdr.new_vlan.pcp  = pcp;
        hdr.new_vlan.cfi  = cfi;
        hdr.new_vlan.vid  = vid;
        hdr.eth.type = 0x88A9;
        hdr.timestamp.hdrtype = 0x8100;
	    hdr.new_vlan.tpid = 0x0800;
    }

    action InsertTimestamplatest() {
        hdr.timestamp.time_depar = smeta.ingress_timestamp;
    }


    table FiveTuple {
        key            = { hdr.ipv4.src      : exact;
                           hdr.ipv4.dst      : exact;
	                       hdr.ipv4.protocol : exact;
	                       table_key_sport   : exact;
	                       table_key_dport   : exact; }
	    actions        = { InsertVLAN; 
                           NoAction; }
	    size           = 8192;
	    default_action = NoAction;
    }

    apply {
        InsertTimestamp();
	    if (hdr.udp.isValid()) {
            table_key_sport = hdr.udp.src_port;
            table_key_dport = hdr.udp.dst_port;
            hit = FiveTuple.apply().hit;
        } else if (hdr.tcp.isValid()) {
            table_key_sport = hdr.tcp.src_port;
            table_key_dport = hdr.tcp.dst_port;
            hit = FiveTuple.apply().hit;
        } 
        InsertTimestamplatest();
        //hdr.timestamp.time_depar = smeta.ingress_timestamp;
        // if (hit) {
        //     if (hdr.vlan.isValid()) 
        //         hdr.eth.type = QINQ_TYPE;
        //     else 
        //         hdr.eth.type = VLAN_TYPE;
        // }
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
        //InsertTimestamplatest();
        packet.emit(hdr.eth);
        packet.emit(hdr.timestamp);
        packet.emit(hdr.new_vlan);
        packet.emit(hdr.vlan);
        packet.emit(hdr.ipv4);
        packet.emit(hdr.ipv4opt);
        packet.emit(hdr.tcp);
        packet.emit(hdr.tcpopt);
        packet.emit(hdr.udp);
        
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
