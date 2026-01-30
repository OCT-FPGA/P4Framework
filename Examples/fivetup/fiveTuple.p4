#include <core.p4>
#include <xsa.p4>

/*
 * Five Tuple (with Timestamp header insertion):
 * Inserts a custom timestamp header (ts_hdr_t) between L2 (eth/vlan) and IPv4.
 * - t_in  = standard_metadata.ingress_timestamp (built-in free-running counter latched at ingress)
 * - t_out = sampled via UserExtern (you will wire it to the SAME counter in RTL)
 * - delta = t_out - t_in  (cycles)
 */

typedef bit<48>  MacAddr;
typedef bit<32>  IPv4Addr;

const bit<16> QINQ_TYPE = 0x88A8;
const bit<16> VLAN_TYPE = 0x8100;
const bit<16> IPV4_TYPE = 0x0800;

// Custom EtherType for our inserted timestamp shim header
const bit<16> TS_TYPE   = 0x0908;  // choose any unused value in your testbed

const bit<8>  TCP_PROT  = 0x06;
const bit<8>  UDP_PROT  = 0x11;

struct metadata {
    bit<64> dummy;
    bit<64> now;
}


// ****************************************************************************** //
// *************************** H E A D E R S  *********************************** //
// ****************************************************************************** //

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

header ts_hdr_t {
    bit<16> next_type;  // will be IPV4_TYPE
    bit<16> rsvd;
    bit<64> t_in;       // ingress timestamp (cycles)
    bit<64> t_out;      // sampled later (cycles)
    bit<64> delta;      // t_out - t_in (cycles)
}

header ipv4_t {
    bit<4>   version;
    bit<4>   hdr_len;
    bit<8>   tos;
    bit<16>  length;
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

// ****************************************************************************** //
// ************************* S T R U C T U R E S  ******************************* //
// ****************************************************************************** //

struct headers {
    eth_mac_t    eth;
    vlan_t       new_vlan;
    vlan_t       vlan;
    ts_hdr_t     ts;        // <--- inserted between L2 and IPv4
    ipv4_t       ipv4;
    ipv4_opt_t   ipv4opt;
    tcp_t        tcp;
    tcp_opt_t    tcpopt;
    udp_t        udp;
}


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
            TS_TYPE   : parse_ts;
            IPV4_TYPE : parse_ipv4;
            default   : accept;
        }
    }

    state parse_vlan {
        packet.extract(hdr.vlan);
        transition select(hdr.vlan.tpid) {
            TS_TYPE   : parse_ts;
            IPV4_TYPE : parse_ipv4;
            default   : accept;
        }
    }

    state parse_ts {
        packet.extract(hdr.ts);
        transition select(hdr.ts.next_type) {
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

    // UserExtern: input bit<1> dummy, output bit<64> timestamp
    // The "(1)" is declared latency in cycles (adjust to match your RTL)
    UserExtern<bit<64>, bit<64>>(1) ts_now;

    action InsertVLAN(bit<3> pcp, bit<1> cfi, bit<12> vid) {
        hdr.new_vlan.setValid();
        hdr.new_vlan.pcp  = pcp;
        hdr.new_vlan.cfi  = cfi;
        hdr.new_vlan.vid  = vid;
        hdr.new_vlan.tpid = hdr.eth.type; // remember original eth.type as inner TPID
    }

    table FiveTuple {
        key = {
            hdr.ipv4.src      : exact;
            hdr.ipv4.dst      : exact;
            hdr.ipv4.protocol : exact;
            table_key_sport   : exact;
            table_key_dport   : exact;
        }
        actions = {
            InsertVLAN;
            NoAction;
        }
        size = 8192;
        default_action = NoAction;
    }

    apply {

        // 5-tuple classification
        if (hdr.udp.isValid()) {
            table_key_sport = hdr.udp.src_port;
            table_key_dport = hdr.udp.dst_port;
            hit = FiveTuple.apply().hit;
        } else if (hdr.tcp.isValid()) {
            table_key_sport = hdr.tcp.src_port;
            table_key_dport = hdr.tcp.dst_port;
            hit = FiveTuple.apply().hit;
        }

        // Your original VLAN/QinQ behavior
        if (hit) {
            if (hdr.vlan.isValid())
                hdr.eth.type = QINQ_TYPE;
            else
                hdr.eth.type = VLAN_TYPE;
        }

        // ---------------------------------------------------------------------
        // Timestamp header insertion (between L2 tags and IPv4)
        // ---------------------------------------------------------------------
        if (hdr.ipv4.isValid() && !hdr.ts.isValid()) {
            // Make TS header valid and populate
            hdr.ts.setValid();
            hdr.ts.next_type = IPV4_TYPE;
            hdr.ts.rsvd      = 0;

            // ingress timestamp (built-in)
            hdr.ts.t_in = smeta.ingress_timestamp;
	    
	    ts_now.apply(0, meta.now);
	    hdr.ts.t_out = meta.now;
	    hdr.ts.delta = hdr.ts.t_out - hdr.ts.t_in;

            

            // Fix EtherType chain so TS header appears right before IPv4.
            //
            // Priority:
            // 1) If original VLAN exists, make vlan.tpid point to TS_TYPE.
            // 2) else if new_vlan exists, make new_vlan.tpid point to TS_TYPE.
            // 3) else make eth.type point to TS_TYPE.
            //
            // NOTE: If you have both new_vlan and vlan valid and you want TS after both,
            // you should set the "last VLAN tag's tpid" to TS_TYPE.
            if (hdr.vlan.isValid()) {
                hdr.vlan.tpid = TS_TYPE;
            } else if (hdr.new_vlan.isValid()) {
                hdr.new_vlan.tpid = TS_TYPE;
            } else {
                hdr.eth.type = TS_TYPE;
            }
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
        packet.emit(hdr.new_vlan);
        packet.emit(hdr.vlan);
        packet.emit(hdr.ts);     // <--- emit TS header between L2 and IPv4
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
