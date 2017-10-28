#!/usr/bin/python
from scapy.all import *
import sys
from time import sleep

DNS_QUERY = 0
DNS_ANSWER = 1

WIDTH = 62
DOA_TYPE_FIRMWARE = 102
DOA_TYPE_FIRMWARE_SIG = 103
DOA_TYPE_FIRMWARE_VERSION = 104

DOA_LOCATION_LOCAL = 1
DOA_LOCATION_URI = 2
DOA_LOCATION_LOCAL = 3

DELAY = 0

class DOARecord(object):
    rdata_format = struct.Struct('!IIBB')
    doa_types = {
        0: 'Reserved',
        1: 'Contact e-mail',
        2: 'Contact Website',
        3: 'Contact telephone',
        102: 'Firmware',
        103: 'Firmware signature',
        104: 'Firmware version',
    }
    doa_locations = {
        0: 'Reserved',
        1: 'Local',
        2: 'URI',
        3: 'HDL',
    }
    def __init__(self, enterprise, doa_type, doa_location, media_type, doa_data):
        self.enterprise = enterprise
        self.doa_type = doa_type
        self.doa_location = doa_location
        self.media_type = media_type
        self.doa_data = doa_data

    def _separator(self):
        print('+{}+'.format('-' * (WIDTH - 2)))

    def wrap_lines(self, text, width):
        return (text[i:i+width] for i in range(0, len(text), width))

    def doa_type_name(self):
        return self.doa_types.get(self.doa_type, 'Unknown')

    def doa_location_name(self):
        return self.doa_locations.get(self.doa_location, 'Unknown')

    def pprint(self):
        self._separator()
        print('| {:^{}} |'.format('Enterprise: {}'.format(self.enterprise), WIDTH - 4))
        self._separator()
        print('| {:^{}} |'.format('DOA-Type: {} ({})'.format(self.doa_type, self.doa_type_name()), WIDTH - 4))
        self._separator()
        half_a = WIDTH / 2
        half_b = WIDTH - half_a
        sys.stdout.write('| {:^{}} '.format('Location: {} ({})'.format(self.doa_location, self.doa_location_name()), half_a - 3))
        print('| {:^{}} |'.format('Media-Type-Len: {} bytes'.format(len(self.media_type)), half_b - 4))
        self._separator()
        print('| {:^{}} |'.format('Media-Type: {}'.format(self.media_type), WIDTH - 4))
        self._separator()
        print('| {:{}} |'.format('DOA-Data:', WIDTH - 4))
        for line in self.wrap_lines(self.doa_data, WIDTH - 4):
            print('| {:{}} |'.format('{}'.format(line), WIDTH - 4))
        self._separator()


    @classmethod
    def from_wire(cls, raw):
        hdr_size = cls.rdata_format.size
        unpacked = cls.rdata_format.unpack(raw[:hdr_size])
        enterprise = unpacked[0]
        type_ = unpacked[1]
        location = unpacked[2]
        media_type_len = unpacked[3]
        media_type = raw[hdr_size:hdr_size + media_type_len]
        data = raw[hdr_size + media_type_len:]
        return cls(enterprise, type_, location, media_type, data)


def callback(pkt):
    if not pkt[0].haslayer(DNS):
        return ''
    print('=' * WIDTH)
    print('{:^{}}'.format('{} -> {}'.format(pkt['IP'].src, pkt['IP'].dst), WIDTH))
    qname = pkt[0]['DNS'].fields['qd'].fields['qname']
    qtype = pkt[0]['DNS'].fields['qd'].fields['qtype']
    type_ = pkt[0]['DNS'].fields['qr']
    
    title = 'DNS {} for name: {} '.format('Query' if type_ == DNS_QUERY else 'Answer', qname)
    print('{:^{}}'.format(title, WIDTH))
    print('=' * WIDTH)
    
    if type_ == DNS_QUERY:
        print(qname + ' with RRTYPE ' + str(qtype))
    else:
        ancount = pkt[0]['DNS'].fields['ancount']
        for i in range(ancount):
            ans = pkt[0]['DNS'].fields['an'][i]
            rrname = ans.fields['rrname']
            rdata = ans.fields['rdata']
            type_ = ans.fields['type']
            print(rrname + ' with RRTYPE ' + str(type_))
            if type_ == 259:
                DOARecord.from_wire(rdata).pprint()
                sleep(DELAY)
            else:
                print(rdata)
            print('')
    print('')


if __name__ == '__main__':
    sniff(iface='wlan2', filter="udp and port 53", promisc=1, store=0, prn=callback)

