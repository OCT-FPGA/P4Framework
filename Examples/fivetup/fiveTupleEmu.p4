// ----------------------------------------------------------------------- //
//  FiveTuple + latency shim (L programmable from control-plane)
//  - Inserts custom EtherType 0x88B5 + lat_shim_t after Ethernet
//  - L (GUI latency cycles) comes from table CfgLatency (size 1)
//  - No ternary operators (?:) used (your behav architecture forbids it)
// ----------------------------------------------------------------------- //

#include <core.p4>
#include <xsa.p4>

typedef bit<48>  MacAddr;
typedef bit<32>  IPv4Addr;

const bit<16> QINQ_TYPE = 0x88A8;
const bit<16> VLAN_TYPE = 0x8100;
const bit<16> IPV4_TYPE = 0x0800;

const bit<8>  TCP_PROT  = 0x06;
const bit<8>  UDP_PROT  = 0x11;

// ----------------------------
// Latency constants
// ----------------------------
const bit<16> GUI_L_DEFAULT  = 40;      // used if control-plane doesn't program table
const bit<16> BUS_BYTES      = 64;      // 512-bit bus => 64 bytes per cycle
const bit<16> LAT_SHIM_ETYPE = 0x88B5;  // custom EtherType

// ----------------------------
// Headers
// ----------------------------
header eth_mac_t {
    MacAddr dmac;
    MacAddr smac;
    bit<16> type;
}

header vlan_t {
    bit<3>  pcp;
    bit<1>  cfi;
    bit<12> vid;
    bit<16> tpid;
}

header ipv4_t {
    bit<4>   version;
    bit<4>   hdr_len;
    bit<8>   tos;
    bit<16>  length;   // IPv4 total length in bytes
    bit<16>  id;
    bit<3>   flags;
    bit<13>  offset;
    bit<8>   ttl;
    bit<8>   protocol;
    bit<16>  hdr_chk;
    IPv4Addr src;
    IPv4Addr dst;
}

header ipv4_opt_t {
    varbit<320> options;
}

header tcp_t {
    bit<16> src_port;
    bit<16> dst_port;
    bit<32> seqNum;
    bit<32> ackNum;
    bit<4>  dataOffset;
    bit<6>  resv;
    bit<6>  flags;
    bit<16> window;
    bit<16> checksum;
    bit<16> urgPtr;
}

header tcp_opt_t {
    varbit<320> options;
}

header udp_t {
    bit<16> src_port;
    bit<16> dst_port;
    bit<16> length;
    bit<16> checksum;
}

// Latency shim header inserted after Ethernet
header lat_shim_t {
    bit<16> inner_type;

    bit<16> D_cycles;
    bit<16> B_pkts_in_flight;
    bit<8>  A_cycles_per_pkt;
    bit<8>  flags;           // bit0 hit, bit1 new_vlan_valid, bit2 ipv4_valid, bit3 vlan_in_valid

    bit<16> cur_len_bytes;   // current frame length in bytes (no FCS)
}

// Divider structs (same as advanced_calculator)
struct divider_input {
    bit<32> divisor;
    bit<32> dividend;
}

struct divider_output {
    bit<32> remainder;
    bit<32> quotient;
}

struct headers {
    eth_mac_t    eth;
    lat_shim_t   lat;     // NEW
    vlan_t       new_vlan;
    vlan_t       vlan;
    ipv4_t       ipv4;
    ipv4_opt_t   ipv4opt;
    tcp_t        tcp;
    tcp_opt_t    tcpopt;
    udp_t        udp;
}

// Metadata now carries programmable L
struct metadata {
    bit<16> gui_L_cycles;
}

// User-defined errors
error {
    InvalidIPpacket,
    InvalidTCPpacket
}

// ----------------------------
// Parser (handles optional shim on looped packets)
// ----------------------------
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
            LAT_SHIM_ETYPE : parse_lat;
            VLAN_TYPE      : parse_vlan;
            IPV4_TYPE      : parse_ipv4;
            default        : accept;
        }
    }

    state parse_lat {
        packet.extract(hdr.lat);
        transition select(hdr.lat.inner_type) {
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
            TCP_PROT : parse_tcp;
            UDP_PROT : parse_udp;
            default  : accept;
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

// ----------------------------
// Processing
// ----------------------------
control MyProcessing(inout headers hdr,
                     inout metadata meta,
                     inout standard_metadata_t smeta) {

    // Divider UserExtern available via .so (as in advanced_calculator)
    UserExtern<divider_input, divider_output>(34) calc_divide;

    divider_input  div_in;
    divider_output div_out;

    bit<16> table_key_sport;
    bit<16> table_key_dport;
    bool hit = false;

    // ----------------------------
    // Control-plane programmable GUI latency L (cycles)
    // ----------------------------
    action set_gui_L(bit<16> L) {
        meta.gui_L_cycles = L;
    }

    action set_gui_L_default() {
        meta.gui_L_cycles = GUI_L_DEFAULT;
    }

    table CfgLatency {
        key = {
            (bit<16>)0 : exact;   // constant key => single entry table
        }
        actions = {
            set_gui_L;
            set_gui_L_default;
        }
        size = 256;
        default_action = set_gui_L_default();
    }

    // ----------------------------
    // FiveTuple program logic
    // ----------------------------
    action InsertVLAN(bit<3> pcp, bit<1> cfi, bit<12> vid) {
        hdr.new_vlan.setValid();
        hdr.new_vlan.pcp  = pcp;
        hdr.new_vlan.cfi  = cfi;
        hdr.new_vlan.vid  = vid;
        hdr.new_vlan.tpid = hdr.eth.type; // captures eth.type at insertion time
    }

    table FiveTuple {
        key = {
            hdr.ipv4.src      : exact;
            hdr.ipv4.dst      : exact;
            hdr.ipv4.protocol : exact;
            table_key_sport   : exact;
            table_key_dport   : exact;
        }
        actions = { InsertVLAN; NoAction; }
        size = 8192;
        default_action = NoAction;
    }

    apply {
        // Apply config table first (sets meta.gui_L_cycles)
        CfgLatency.apply();

        // FiveTuple hit logic (same)
        if (hdr.udp.isValid()) {
            table_key_sport = hdr.udp.src_port;
            table_key_dport = hdr.udp.dst_port;
            hit = FiveTuple.apply().hit;
        } else if (hdr.tcp.isValid()) {
            table_key_sport = hdr.tcp.src_port;
            table_key_dport = hdr.tcp.dst_port;
            hit = FiveTuple.apply().hit;
        }

        // Ethertype rewrite on hit (same)
        if (hit) {
            if (hdr.vlan.isValid())
                hdr.eth.type = QINQ_TYPE;
            else
                hdr.eth.type = VLAN_TYPE;
        }

        // Only emit telemetry for IPv4 packets (same behavior as before)
        if (!hdr.ipv4.isValid()) {
            return;
        }

        // Compute current frame length (bytes): 14 + (vlan?4:0) + ipv4.total_len
        bit<16> l2 = (bit<16>)14;
        if (hdr.vlan.isValid()) {
            l2 = l2 + (bit<16>)4;
        }
        bit<16> cur_len = l2 + (bit<16>)hdr.ipv4.length;

        // A cycles:
        // - inserting hdr.lat => 1 cycle on 512-bit bus
        // - inserting hdr.new_vlan only if valid => +1
        bit<8> A = (bit<8>)1;
        if (hdr.new_vlan.isValid()) {
            A = A + (bit<8>)1;
        }

        // B = ceil(L*BUS_BYTES / cur_len)
        bit<32> num = (bit<32>)meta.gui_L_cycles * (bit<32>)BUS_BYTES;

        bit<32> den = (bit<32>)cur_len;
        if (cur_len == (bit<16>)0) {
            den = (bit<32>)1;
        }

        div_in.dividend = num;
        div_in.divisor  = den;
        calc_divide.apply(div_in, div_out);

        bit<32> q = div_out.quotient;
        bit<32> r = div_out.remainder;

        // B32 = q + (r != 0 ? 1 : 0)  // NO ternary allowed
        bit<32> B32 = q;
        if (r != (bit<32>)0) {
            B32 = B32 + (bit<32>)1;
        }

        // C32 = A * B32 (A is 1 or 2)  // NO ternary allowed
        bit<32> C32 = B32;
        if (A != (bit<8>)1) {
            C32 = B32 << 1;
        }

        bit<32> D32 = (bit<32>)meta.gui_L_cycles + C32;

        // Insert shim header and populate
        hdr.lat.setValid();
        hdr.lat.inner_type = hdr.eth.type;
        hdr.eth.type = LAT_SHIM_ETYPE;

        hdr.lat.D_cycles         = (bit<16>)D32;
        hdr.lat.B_pkts_in_flight = (bit<16>)B32;
        hdr.lat.A_cycles_per_pkt = A;
        hdr.lat.cur_len_bytes    = cur_len;

        hdr.lat.flags = (bit<8>)0;
        if (hit)                    hdr.lat.flags[0:0] = 1;
        if (hdr.new_vlan.isValid()) hdr.lat.flags[1:1] = 1;
        hdr.lat.flags[2:2] = 1; // ipv4_valid
        if (hdr.vlan.isValid())     hdr.lat.flags[3:3] = 1;
    }
}

// ----------------------------
// Deparser
// ----------------------------
control MyDeparser(packet_out packet,
                   in headers hdr,
                   inout metadata meta,
                   inout standard_metadata_t smeta) {
    apply {
        packet.emit(hdr.eth);
        packet.emit(hdr.lat);
        packet.emit(hdr.new_vlan);
        packet.emit(hdr.vlan);
        packet.emit(hdr.ipv4);
        packet.emit(hdr.ipv4opt);
        packet.emit(hdr.tcp);
        packet.emit(hdr.tcpopt);
        packet.emit(hdr.udp);
    }
}

XilinxPipeline(
    MyParser(),
    MyProcessing(),
    MyDeparser()
) main;
