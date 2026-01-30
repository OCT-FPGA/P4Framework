#!/usr/bin/env python3
import argparse
import shutil
import subprocess
import sys
from statistics import mean

def run_tshark_lengths(pcap_path: str):
    """Return list of frame lengths in bytes using tshark."""
    if not shutil.which("tshark"):
        print("ERROR: tshark not found. Install Wireshark/tshark first.", file=sys.stderr)
        sys.exit(2)

    cmd = ["tshark", "-r", pcap_path, "-T", "fields", "-e", "frame.len"]
    try:
        out = subprocess.check_output(cmd, stderr=subprocess.DEVNULL, text=True)
    except subprocess.CalledProcessError as e:
        print("ERROR: failed to run tshark. Is the pcap readable?", file=sys.stderr)
        sys.exit(2)

    lens = []
    for line in out.splitlines():
        line = line.strip()
        if not line:
            continue
        try:
            lens.append(int(line))
        except ValueError:
            continue
    return lens

def main():
    ap = argparse.ArgumentParser(
        description="Estimate packets-per-second at a target line rate from a pcap file."
    )
    ap.add_argument("pcap", help="Input .pcap or .pcapng")
    ap.add_argument("--line-gbps", type=float, default=100.0,
                    help="Line rate in Gbps (default: 100)")
    ap.add_argument("--overhead-bytes", type=int, default=20,
                    help="On-wire overhead bytes per frame (default: 20 = preamble+IFG)")
    ap.add_argument("--filter", default=None,
                    help="Optional display filter (Wireshark/tshark syntax), e.g. 'eth.type==0x0908'")
    args = ap.parse_args()

    if args.filter:
        # Re-run tshark with a display filter; still extract frame.len
        if not shutil.which("tshark"):
            print("ERROR: tshark not found. Install Wireshark/tshark first.", file=sys.stderr)
            sys.exit(2)
        cmd = ["tshark", "-r", args.pcap, "-Y", args.filter, "-T", "fields", "-e", "frame.len"]
        try:
            out = subprocess.check_output(cmd, stderr=subprocess.DEVNULL, text=True)
        except subprocess.CalledProcessError:
            print("ERROR: failed to run tshark with filter. Check filter syntax.", file=sys.stderr)
            sys.exit(2)
        lens = []
        for line in out.splitlines():
            line = line.strip()
            if not line:
                continue
            try:
                lens.append(int(line))
            except ValueError:
                continue
    else:
        lens = run_tshark_lengths(args.pcap)

    if not lens:
        print("No packets found (or filter removed all packets).", file=sys.stderr)
        sys.exit(1)

    avg_len = mean(lens)  # bytes, as captured (L2 frame bytes)
    avg_onwire_bytes = avg_len + args.overhead_bytes

    line_bps = args.line_gbps * 1e9
    pps = line_bps / (avg_onwire_bytes * 8.0)
    mpps = pps / 1e6

    print(f"Packets: {len(lens)}")
    print(f"Avg frame.len: {avg_len:.2f} bytes")
    print(f"Assumed on-wire overhead: +{args.overhead_bytes} bytes (preamble+IFG)")
    print(f"Avg on-wire size: {avg_onwire_bytes:.2f} bytes")
    print(f"Line rate: {args.line_gbps:.3f} Gbps")
    print(f"Estimated PPS at line rate: {pps:,.2f} pps")
    print(f"Estimated MPPS at line rate: {mpps:,.3f} Mpps")
    print(f"Suggested VitisNetP4 PKT_RATE (rounded up): {int(mpps + 0.999)}")

if __name__ == "__main__":
    main()
