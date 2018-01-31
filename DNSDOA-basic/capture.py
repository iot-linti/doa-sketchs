#!/usr/bin/python
from scapy.all import *
from time import sleep
import argparse
import blessings
import functools
import os
import sys

DNS_QUERY = 0
DNS_ANSWER = 1

DOA_TYPE_FIRMWARE = 102
DOA_TYPE_FIRMWARE_SIG = 103
DOA_TYPE_FIRMWARE_VERSION = 104

DOA_LOCATION_LOCAL = 1
DOA_LOCATION_URI = 2
DOA_LOCATION_LOCAL = 3

DELAY = 5
term = blessings.Terminal(kind='xterm-256color')
WIDTH = 55 if term.width < 55 else term.width
WIDTH = 55
QUERY_COLOR = term.red
ANSWER_COLOR = term.blue
BRIGHT_QUERY_COLOR = term.bright_yellow
BRIGHT_ANSWER_COLOR = term.bright_cyan
EMPH = term.reverse
EMPH2 = EMPH
NORMAL = term.normal

ESPRESSIF_NAME = None
ESPRESSIF_OUI = '5c:cf:7f'

def emph_rst(text, emph_format, reset_format):
    '''
    emph the given text and then apply reset_format.
    '''
    fmt_text = emph_format(text) + reset_format
    fmt_len = len(fmt_text) - len(text)
    return fmt_text, fmt_len

class DOARecord(object):
    rdata_format = struct.Struct('!IIBB')
    doa_types = {
        0: 'Reserved',
        1: 'Contact e-mail',
        2: 'Contact Website',
        3: 'Contact telephone',
        101: 'Description',
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
        doa_type ='DOA-Type: {} ({})'.format(self.doa_type, self.doa_type_name())
        doa_type_fmt, fmt_len =  emph_rst(doa_type, EMPH, ANSWER_COLOR)
        print('| {:^{}} |'.format(doa_type_fmt, WIDTH - 4 + fmt_len))

        self._separator()
        half_a = WIDTH / 2
        half_b = WIDTH - half_a
        sys.stdout.write('| {:^{}} '.format('Location: {} ({})'.format(self.doa_location, self.doa_location_name()), half_a - 3))
        print('| {:^{}} |'.format('Media-Type-Len: {} bytes'.format(len(self.media_type)), half_b - 4))
        self._separator()
        print('| {:^{}} |'.format('Media-Type: {}'.format(self.media_type), WIDTH - 4))
        self._separator()
        doa_data_fmt, fmt_len = emph_rst('DOA-Data:', EMPH, ANSWER_COLOR)
        print('| {:{}} |'.format(doa_data_fmt, WIDTH - 4 + fmt_len))
        for line in self.wrap_lines(self.doa_data, WIDTH - 4):
            line_fmt, fmt_len = emph_rst(line, EMPH, ANSWER_COLOR)
            print('| {:{}} |'.format(line_fmt, WIDTH - 4 + fmt_len))
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

    type_ = pkt[0]['DNS'].fields['qr']
    srcmac = pkt[0]['Ether'].src
    dstmac = pkt[0]['Ether'].dst
    if type_ == DNS_QUERY:
        color = QUERY_COLOR
    else:
        color = ANSWER_COLOR

    sys.stdout.write(color)

    print('=' * WIDTH)
    src = pkt['IP'].src
    dst = pkt['IP'].dst
    if ESPRESSIF_NAME is not None:
        if srcmac[0:8] == ESPRESSIF_OUI:
            src = ESPRESSIF_NAME
        if dstmac[0:8] == ESPRESSIF_OUI:
            dst = ESPRESSIF_NAME
    print('{:^{}}'.format('{} -> {}'.format(src, dst), WIDTH))
    qname = pkt[0]['DNS'].fields['qd'].fields['qname']
    qtype = pkt[0]['DNS'].fields['qd'].fields['qtype']

    title = 'DNS {} for name: {} '.format(emph_rst(
        'Query' if type_ == DNS_QUERY else 'Answers',
        EMPH2,
        color
    )[0], qname)
    print('{:^{}}'.format(title, WIDTH))
    print('=' * WIDTH)

    if type_ == DNS_QUERY:
        print(emph_rst(qname, EMPH, color)[0] + ' with ' + emph_rst('RRTYPE ' + str(qtype), EMPH, color)[0])
    else:
        ancount = pkt[0]['DNS'].fields['ancount']
        for i in range(ancount):
            ans = pkt[0]['DNS'].fields['an'][i]
            rrname = ans.fields['rrname']
            rdata = ans.fields['rdata']
            type_ = ans.fields['type']
            print(emph_rst(rrname, EMPH, color)[0] + ' with ' + emph_rst('RRTYPE ' + str(type_), EMPH, color)[0])
            if type_ == 259:
                DOARecord.from_wire(rdata).pprint()
                sleep(DELAY)
            else:
                print(rdata)
            print('') # Separator between answers
    print(term.normal)

def get_argparser():
    parser = argparse.ArgumentParser()
    mode = parser.add_mutually_exclusive_group()
    mode.add_argument('-i', '--interface', help='Sniff from given interface')
    mode.add_argument('-f', '--file', help='Read pcap file')
    parser.add_argument('-d', '--dark', action='store_true',
                        help='Colors for dark background terminals')
    parser.add_argument('-n', '--name',
                        help='Use name for Espressif devices')
    return parser



if __name__ == '__main__':
    parser = get_argparser()
    args = parser.parse_args()
    if 'name' in args:
        ESPRESSIF_NAME = args.name
    if args.dark:
        QUERY_COLOR = BRIGHT_QUERY_COLOR
        ANSWER_COLOR = BRIGHT_ANSWER_COLOR
    if args.interface:
        sniff(iface=args.interface, filter="udp and port 53", promisc=1, store=0, prn=callback)
    elif args.file:
        sniff(offline=args.file, filter="udp and port 53", promisc=1, store=0, prn=callback)
    else:
        parser.print_usage()
        exit(1)

