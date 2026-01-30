#!/usr/bin/env python3
import argparse
import math
import sys

def ceil_div(a: int, b: float) -> int:
    #ceil(calculated latency / (Minimum Packet Size / Packet Bus Width))
    return math.ceil(a/b)

def main():
    ap = argparse.ArgumentParser(description="Estimate worst-case packets in flight from latency + min pkt size in pcap.")
    ap.add_argument("--pcap", required=True, help="Input pcap/pcapng file")
    ap.add_argument("--lat", type=int, required=True, help="Calculated latency (in cycles)")
    ap.add_argument("--bus", type=int, default=512, help="Packet bus width in bits (default: 512)")
    args = ap.parse_args()

    from scapy.all import rdpcap, raw
    

    pkts = rdpcap(args.pcap)
    if len(pkts) == 0:
        print("ERROR: pcap has 0 packets", file=sys.stderr)
        return 2

    # Estimate min packet size from pcap bytes as captured (no FCS/preamble/IFG)
    pcap_min = min(len(raw(p)) for p in pkts)
    min_bytes = min(64, pcap_min) # 64 default of least of the pcap

    min_bits = min_bytes * 8
    cycles_per_pkt = (min_bits/args.bus)
    pkts_in_flight = ceil_div(args.lat, cycles_per_pkt)

    print(f"PCAP packets           : {len(pkts)}")
    print(f"Min packet size        : {min_bytes} bytes ({min_bits} bits)")
    print(f"Bus width              : {args.bus} bits")
    print(f"Calculated latency      : {args.lat} cycles")
    print(f"Packets in flight (wc)  : {pkts_in_flight}")

    return 0

if __name__ == "__main__":
    raise SystemExit(main())
