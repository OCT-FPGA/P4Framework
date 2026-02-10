import re, sys, math

p4, bus = sys.argv[1], int(sys.argv[2])  # usage: python3 script.py prog.p4 512
txt = open(p4).read()
txt = re.sub(r"/\*.*?\*/", "", txt, flags=re.S)
txt = "\n".join(line.split("//", 1)[0] for line in txt.splitlines())

ops = {
    "setValid":   set(re.findall(r"\bhdr\.(\w+)\.setValid\(\)",   txt)),
    "setInvalid": set(re.findall(r"\bhdr\.(\w+)\.setInvalid\(\)", txt)),
   # "isValid":    set(re.findall(r"\bhdr\.(\w+)\.isValid\(\)",    txt)),
}
keep = lambda n: not (n.startswith(("eth", "ipv", "tcp", "udp")))
names = sorted({n for s in ops.values() for n in s if keep(n)})

hb = re.search(r"struct\s+headers\s*{(.*?)}", txt, flags=re.S).group(1)
inst2type = {name: typ for typ, name in re.findall(r"(\w+)\s+(\w+)\s*;", hb)}
type2body = dict(re.findall(r"header\s+(\w+)\s*{(.*?)}", txt, flags=re.S))
sum_bits = lambda body: sum(int(b) for b in re.findall(r"\bbit<\s*(\d+)\s*>\s+\w+\s*;", body))

for n in names:
    bits = sum_bits(type2body.get(inst2type.get(n, ""), ""))
    used = [k for k, s in ops.items() if n in s]
    print(f"hdr.{n} [{','.join(used)}]: bits={bits}, ceil(bits/bus)={math.ceil(bits/bus)}")

print("TOTAL ceil(bits/bus) =", sum(math.ceil(sum_bits(type2body.get(inst2type.get(n, ""), ""))/bus) for n in names))
