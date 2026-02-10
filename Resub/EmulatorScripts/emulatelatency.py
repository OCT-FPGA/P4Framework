#!/usr/bin/env python3
import argparse
import math
import re
import subprocess
import sys
from pathlib import Path


def run_cmd(cmd, cwd):
    proc = subprocess.run(cmd, cwd=cwd, text=True, capture_output=True)
    if proc.returncode != 0:
        sys.stderr.write(proc.stdout)
        sys.stderr.write(proc.stderr)
        raise SystemExit(f"command failed: {' '.join(cmd)}")
    return proc.stdout


def parse_pkt_rate(text):
    # "Suggested VitisNetP4 PKT_RATE (rounded up): 59"
    m = re.search(r"Suggested VitisNetP4 PKT_RATE .*?:\s*(\d+)", text)
    if not m:
        raise ValueError("PKT_RATE not found in mpps output")
    return int(m.group(1))


def parse_total_latency(text):
    # Vivado output prints the value as a bare number on a line
    for line in reversed(text.splitlines()):
        line = line.strip()
        if re.fullmatch(r"\d+", line):
            return int(line)
    raise ValueError("TOTAL_LATENCY not found in vivado output")


def estimate_a(p4_text, bus_bits):
    txt = re.sub(r"/\*.*?\*/", "", p4_text, flags=re.S)
    txt = "\n".join(line.split("//", 1)[0] for line in txt.splitlines())

    ops = {
        "assign": set(re.findall(r"\bhdr\.(\w+)\.\w+\s*=\s*(?![=])", txt)),
    }
    keep = lambda n: not (n.startswith(("eth", "ipv", "tcp", "udp")))
    names = sorted({n for s in ops.values() for n in s if keep(n)})

    hb_match = re.search(r"struct\s+headers\s*{(.*?)}", txt, flags=re.S)
    if not hb_match:
        raise ValueError("headers struct not found in P4 file")
    hb = hb_match.group(1)

    inst2type = {name: typ for typ, name in re.findall(r"(\w+)\s+(\w+)\s*;", hb)}
    type2body = dict(re.findall(r"header\s+(\w+)\s*{(.*?)}", txt, flags=re.S))

    def sum_bits(body):
        return sum(int(b) for b in re.findall(r"\bbit<\s*(\d+)\s*>\s+\w+\s*;", body))

    total = 0
    for n in names:
        bits = sum_bits(type2body.get(inst2type.get(n, ""), ""))
        total += math.ceil(bits / bus_bits)
    return total


def estimate_b(pcap_path, latency_cycles, bus_bits):
    try:
        from scapy.all import rdpcap, raw
    except Exception as exc:
        raise RuntimeError("scapy is required for estimate B") from exc

    pkts = rdpcap(str(pcap_path))
    if len(pkts) == 0:
        raise ValueError("pcap has 0 packets")

    pcap_min = min(len(raw(p)) for p in pkts)
    min_bytes = min(64, pcap_min)
    min_bits = min_bytes * 8
    cycles_per_pkt = (min_bits / bus_bits)
    return math.ceil(latency_cycles / cycles_per_pkt)


def main():
    parser = argparse.ArgumentParser(description="Compute latency components for VitisNetP4")
    parser.add_argument("pcap", help="Input PCAP path")
    parser.add_argument("p4", help="P4 file path")
    parser.add_argument("--line-gbps", type=float, default=100.0, help="Line rate in Gbps")
    parser.add_argument("--bus", type=int, default=512, help="Bus width in bits")
    parser.add_argument("--vivado", default="vivado", help="Vivado executable")
    args = parser.parse_args()

    base = Path(__file__).resolve().parent
    pcap = Path(args.pcap).resolve()
    p4 = Path(args.p4).resolve()

    mpps_out = run_cmd(
        [sys.executable, str(base / "mpps.py"), str(pcap), "--line-gbps", str(args.line_gbps)],
        cwd=base,
    )
    pkt_rate = parse_pkt_rate(mpps_out)

    vivado_cmd = (
        "source /tools/Xilinx/Vivado/2021.2/settings64.sh && "
        f"{args.vivado} -mode batch -source {base / 'calc_latency.tcl'} "
        f"-tclargs {p4} {pkt_rate} {args.bus}"
    )
    vivado_out = run_cmd(["bash", "-lc", vivado_cmd], cwd=base)
    total_latency = parse_total_latency(vivado_out)

    comp_a = estimate_a(p4.read_text(), args.bus)
    comp_b = estimate_b(pcap, total_latency, args.bus)

    comp_c = comp_b * comp_a
    total_d = comp_c + total_latency

    print(f"PKT_RATE: {pkt_rate}")
    print(f"Calculated base latency from VitisNetP4: {total_latency}")
    print(f"Latency Component A(due to header inserts): {comp_a}")
    print(f"Latency Component B(number of packets (worst case) in the pipeline): {comp_b}")
    print(f"Latency Component C (B*A)(total extensions due to header inserts): {comp_c}")
    print(f"Total Latency D (C + VitisNetP4 base latency): {total_d}")


if __name__ == "__main__":
    main()

