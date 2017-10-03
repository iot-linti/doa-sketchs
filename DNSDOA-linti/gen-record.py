from scapy.all import *
from codification import *

def dnsRes(pkt):
    print pkt[IP].src
    ip = pkt.getlayer(IP)
    dns = pkt.getlayer(DNS)
    return IP(dst=ip.src, src=ip.dst)/UDP(dport=ip.sport,sport=ip.dport)/DNS(id=dns.id,qd=dns.qd,qr=1,an=DNSRR(rrname=dns.qd.qname, type=259, ttl=3560L, rdata=record1))



def main():
    while 1:
        # sniff(filter="udp and port 53", prn=dnsRes)
        a=sniff(iface='eth3', filter="port 53",count=1,promisc=1)
        if not a[0].haslayer(DNS) or a[0].qr:
            continue
        print(a.nsummary())
        send(dnsRes(a[0]))

if __name__ == '__main__':
    main()

