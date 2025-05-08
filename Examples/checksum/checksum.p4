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
 * Forward Switch:
 *
 * The forward design exemplifies the implementation the core of an IPv4/IPv6
 * network switch. IP destination address is used to perform an LPM search to
 * determine the port where the packet needs to be redirected to. The IPv6
 * table is setup to be implemented with an ternary CAM and the IPv4 table
 * with a semi-ternary CAM.
 *
 */

typedef bit<48>  MacAddr;
typedef bit<32>  IPv4Addr;
typedef bit<128> IPv6Addr;

const bit<16> VLAN_TYPE  = 0x8100;
const bit<16> IPV4_TYPE  = 0x0800;
const bit<16> IPV6_TYPE  = 0x86DD;

const bit<8> TCP_PROT  = 0x06;
const bit<8> UDP_PROT  = 0x11;

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

header ipv4_option_word_h {                    //THIS IS THE HEADER FOR THE IPV4 OPTIONS FIELD
    bit<32>      data;
}

header ipv6_t {
    bit<4>   version;    // Version = 6
    bit<8>   priority;   // Traffic class
    bit<20>  flow_label; // Flow label
    bit<16>  length;     // Payload length
    bit<8>   protocol;   // Next protocol
    bit<8>   hop_limit;  // Hop limit
    IPv6Addr src;        // Source address
    IPv6Addr dst;        // Destination address
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

// ****************************************************************************** //
// ************************* S T R U C T U R E S  ******************************* //
// ****************************************************************************** //

// header structure
struct headers {
    eth_mac_t    eth;
    vlan_t[2]    vlan;
    ipv4_t       ipv4;

//==================================================== 10 IPV4 OPTIONS HEADERS FOR 10x32 = 320 BITS (MAXIMUM OPTIONS) ======================================================

    ipv4_option_word_h  ipv4_options_0;
    ipv4_option_word_h  ipv4_options_1;
    ipv4_option_word_h  ipv4_options_2;
    ipv4_option_word_h  ipv4_options_3;
    ipv4_option_word_h  ipv4_options_4;
    ipv4_option_word_h  ipv4_options_5;
    ipv4_option_word_h  ipv4_options_6;
    ipv4_option_word_h  ipv4_options_7;
    ipv4_option_word_h  ipv4_options_8;
    ipv4_option_word_h  ipv4_options_9;

//========================================================================================================================================================


    ipv6_t       ipv6;
    tcp_t        tcp;
    tcp_opt_t    tcpopt;
    udp_t        udp;
}

// User metadata structure
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

    state parse_eth {
        packet.extract(hdr.eth);
        transition select(hdr.eth.type) {
            VLAN_TYPE : parse_vlan;
            IPV4_TYPE : parse_ipv4;
            IPV6_TYPE : parse_ipv6;
            default   : accept;
        }
    }

    state parse_vlan {
        packet.extract(hdr.vlan.next);
        transition select(hdr.vlan.last.tpid) {
            VLAN_TYPE : parse_vlan;
            IPV4_TYPE : parse_ipv4;
            IPV6_TYPE : parse_ipv6;
            default   : accept;
        }
    }


//========================== PARSING THE IPV4 HEADER ALONG WITH OPTIONS ===============================================================

    state parse_ipv4 {
        packet.extract(hdr.ipv4);
        transition select(hdr.ipv4.version, hdr.ipv4.hdr_len) {
                   (4, 5) : parse_ipv4_no_options;
                   (4, 6) : parse_ipv4_options_0_0;
                   (4, 7) : parse_ipv4_options_0_1;
                   (4, 8) : parse_ipv4_options_0_2;
                   (4, 9) : parse_ipv4_options_0_3;
                   (4, 10) : parse_ipv4_options_0_4;
                   (4, 11) : parse_ipv4_options_0_5;
                   (4, 12) : parse_ipv4_options_0_6;
                   (4, 13) : parse_ipv4_options_0_7;
                   (4, 14) : parse_ipv4_options_0_8;
                   (4, 15) : parse_ipv4_options_0_9;
                   default: accept;
        }
    }

    state parse_ipv4_options_0_0 {                           //NOTE: In this Parser over here, the first 16 bit data chunk from IPv4_Options field goes into the hdr.ipv4_options_n, where ( n = hdr.hdr_len - 1 ).
                                                             //Hence, during emitting the headers in the deparser, they have to be emitted in the correct order. 
        packet.extract(hdr.ipv4_options_0);
        transition parse_ipv4_no_options;

    }

    state parse_ipv4_options_0_1 {

        packet.extract(hdr.ipv4_options_1);
        transition parse_ipv4_options_0_0;
    }

    state parse_ipv4_options_0_2 {

        packet.extract(hdr.ipv4_options_2);
        transition parse_ipv4_options_0_1;

    }

    state parse_ipv4_options_0_3 {

        packet.extract(hdr.ipv4_options_3);
        transition parse_ipv4_options_0_2;
    }

    state parse_ipv4_options_0_4 {

        packet.extract(hdr.ipv4_options_4);
        transition parse_ipv4_options_0_3;
    }

    state parse_ipv4_options_0_5 {

        packet.extract(hdr.ipv4_options_5);
        transition parse_ipv4_options_0_4;
    }

    state parse_ipv4_options_0_6 {

        packet.extract(hdr.ipv4_options_6);
        transition parse_ipv4_options_0_5;
    }


    state parse_ipv4_options_0_7 {

        packet.extract(hdr.ipv4_options_7);
        transition parse_ipv4_options_0_6;
    }

    state parse_ipv4_options_0_8 {

        packet.extract(hdr.ipv4_options_8);
        transition parse_ipv4_options_0_7;
    }

    state parse_ipv4_options_0_9 {

        packet.extract(hdr.ipv4_options_9);
        transition parse_ipv4_options_0_8;
    }

    state parse_ipv4_no_options {
        transition select(hdr.ipv4.protocol){

            TCP_PROT  : parse_tcp;
            UDP_PROT  : parse_udp;
            default   : accept;

        }
    }

//============================================================================================================================

    state parse_ipv6 {
        packet.extract(hdr.ipv6);
        verify(hdr.ipv6.version == 6, error.InvalidIPpacket);
        transition select(hdr.ipv6.protocol) {
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


//============================================================================== CHECKSUM LOGIC ==============================================================

         bit<16> ones_complement_result;           //Globally declared variable to contain the final checksum so that it can be accessed in APPLY block
         action ipv4_checksum_computation() {

                     //**GROUPING OF FIXED LENGTH DATA**

                     bit<16> c1 = hdr.ipv4.version ++ hdr.ipv4.hdr_len ++ hdr.ipv4.tos;
                     bit<16> c2 = hdr.ipv4.length;
                     bit<16> c3 = hdr.ipv4.id;
                     bit<16> c4  = hdr.ipv4.flags ++ hdr.ipv4.offset;
                     bit<16> c5  = hdr.ipv4.ttl ++ hdr.ipv4.protocol;
                     bit<16> c6  = hdr.ipv4.src [31:16];
                     bit<16> c7  = hdr.ipv4.src [15:0];
                     bit<16> c8  = hdr.ipv4.dst [31:16];
                     bit<16> c9 = hdr.ipv4.dst[15:0];

                     bit<16> c10 = hdr.ipv4_options_0.data[31:16];
                     bit<16> c11 = hdr.ipv4_options_0.data[15:0];

                     bit<16> c12 = hdr.ipv4_options_1.data[31:16];
                     bit<16> c13 = hdr.ipv4_options_1.data[15:0];

                     bit<16> c14 = hdr.ipv4_options_2.data[31:16];
                     bit<16> c15 = hdr.ipv4_options_2.data[15:0];

                     bit<16> c16 = hdr.ipv4_options_3.data[31:16];
                     bit<16> c17 = hdr.ipv4_options_3.data[15:0];

                     bit<16> c18 = hdr.ipv4_options_4.data[31:16];
                     bit<16> c19 = hdr.ipv4_options_4.data[15:0];

                     bit<16> c20 = hdr.ipv4_options_5.data[31:16];
                     bit<16> c21 = hdr.ipv4_options_5.data[15:0];

                     bit<16> c22 = hdr.ipv4_options_6.data[31:16];
                     bit<16> c23 = hdr.ipv4_options_6.data[15:0];

                     bit<16> c24 = hdr.ipv4_options_7.data[31:16];
                     bit<16> c25 = hdr.ipv4_options_7.data[15:0];

                     bit<16> c26 = hdr.ipv4_options_8.data[31:16];
                     bit<16> c27 = hdr.ipv4_options_8.data[15:0];

                     bit<16> c28 = hdr.ipv4_options_9.data[31:16];
                     bit<16> c29 = hdr.ipv4_options_9.data[15:0];


                    //**ADDING THE CHUNKS OF 16-BIT DATA**

                    bit<32> sum = (bit<32>)c1 + (bit<32>)c2 + (bit<32>)c3 + (bit<32>)c4 + (bit<32>)c5 + (bit<32>)c6 + (bit<32>)c7 + (bit<32>)c8 + (bit<32>)c9 + (bit<32>)c10 + (bit<32>)c11 + (bit<32>)c12 + (bit<32>)c13 + (bit<32>)c14 + (bit<32>)c15 + (bit<32>)c16 + (bit<32>)c17 + (bit<32>)c18 + (bit<32>)c19 + (bit<32>)c20 + (bit<32>)c21 + (bit<32>)c22 + (bit<32>)c23 + (bit<32>)c24 + (bit<32>)c25 + (bit<32>)c26 + (bit<32>)c27 + (bit<32>)c28 + (bit<32>)c29;

                    //**ADDING THE 16 LSBs OF SUM TO THE OVERFLOW TO MAKE PRIMARY CHECKSUM 16-BITS**

                    bit<16> internet_checksum = sum[15:0] + sum[31:16];

                    //**ONE's COMPLEMENT OF THE PRIMARY CHECKSUM TO COMPUTE FINAL CHECKSUM**

                    ones_complement_result = ~internet_checksum;

    }

//============================================================================================================================================================

    action forwardPacket() {
    }

    action dropPacket() {
                smeta.drop = 1;
    }


    table forwardIPv4 {
        key             = { hdr.ipv4.dst : lpm; }
        actions         = { forwardPacket;
                            dropPacket; }
        size            = 1024;
                num_masks       = 64;
        default_action  = forwardPacket;
    }

    table forwardIPv6 {
        key             = { hdr.ipv6.dst : lpm; }
        actions         = { forwardPacket;
                            dropPacket; }
        size            = 1024;
        default_action  = forwardPacket;
    }

    apply {

        if (smeta.parser_error != error.NoError) {
            dropPacket();
            return;
        }

//==========================FOR CHECKING THE VALIDITY OF THE IPV4 OPTIONS FIELDS===============================================================

        if(!(hdr.ipv4_options_0.isValid())) {

                hdr.ipv4_options_0.data= 0x00000000;

        }

        if(!(hdr.ipv4_options_1.isValid())) {

                hdr.ipv4_options_1.data= 0x00000000;

        }

        if(!(hdr.ipv4_options_2.isValid())) {

                hdr.ipv4_options_2.data= 0x00000000;

        }

        if(!(hdr.ipv4_options_3.isValid())) {

                hdr.ipv4_options_3.data= 0x00000000;

        }

        if(!(hdr.ipv4_options_4.isValid())) {

                hdr.ipv4_options_4.data= 0x00000000;

        }

        if(!(hdr.ipv4_options_5.isValid())) {

                hdr.ipv4_options_5.data= 0x00000000;

        }

        if(!(hdr.ipv4_options_6.isValid())) {

                hdr.ipv4_options_6.data= 0x00000000;

        }

        if(!(hdr.ipv4_options_7.isValid())) {

                hdr.ipv4_options_7.data= 0x00000000;

        }

        if(!(hdr.ipv4_options_8.isValid())) {

                hdr.ipv4_options_8.data= 0x00000000;

        }

        if(!(hdr.ipv4_options_9.isValid())) {

                hdr.ipv4_options_9.data= 0x00000000;

        }

//=============================================================================================================================================

        ipv4_checksum_computation();                               //Called the checksum action

        if (hdr.ipv4.hdr_chk != ones_complement_result) {          //Checking if the checksum header field is correct
            dropPacket();
            return;
        }


        if (hdr.ipv4.isValid())
            forwardIPv4.apply();

        else if (hdr.ipv6.isValid())
            forwardIPv6.apply();
        else
                        forwardPacket();

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
        packet.emit(hdr.vlan);
        packet.emit(hdr.ipv4);

        packet.emit(hdr.ipv4_options_9);
        packet.emit(hdr.ipv4_options_8);
        packet.emit(hdr.ipv4_options_7);
        packet.emit(hdr.ipv4_options_6);
        packet.emit(hdr.ipv4_options_5);
        packet.emit(hdr.ipv4_options_4);
        packet.emit(hdr.ipv4_options_3);
        packet.emit(hdr.ipv4_options_2);
        packet.emit(hdr.ipv4_options_1);
        packet.emit(hdr.ipv4_options_0);

        packet.emit(hdr.ipv6);
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
