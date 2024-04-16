from scapy.all import IP, UDP, Ether
from scapy.utils import rdpcap 

# UDP packet with 128B payload
class pktsUtils:
    def get_packets(pcapFile):
        pkts = rdpcap(pcapFile)
        print("Successfully read {} pkts from {}".format(len(pkts), pcapFile))
        return pkts

pktsUtils.get_packets('test1.pcap')
