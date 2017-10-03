import struct
import io
import base64

DOA_TYPE_FIRMWARE = 102
DOA_TYPE_FIRMWARE_SIG = 103
DOA_TYPE_FIRMWARE_VERSION = 104

DOA_LOCATION_LOCAL = 1
DOA_LOCATION_URI = 2
DOA_LOCATION_LOCAL = 3

class DOA:
    def __init__(self, enterprise, type_, location, media_type, data):
        self.rdata = struct.Struct('!IIBB')
        self.enterprise = enterprise
        self.type = type_
        self.location = location
        self.media_type = media_type
        self.data = data

    def towire(self):
        return '{}{}{}'.format(
            self.rdata.pack(
                self.enterprise,
                self.type,
                self.location,
                len(self.media_type),
            ),
            self.media_type,
            base64.encodestring(self.data),
        )

record1 = DOA(0, DOA_TYPE_FIRMWARE, DOA_LOCATION_URI, 'application/x-binary', 'http://lihuen.linti.unlp.edu.ar/bajar/firm.bin').towire()
record2 = DOA(0, DOA_TYPE_FIRMWARE_VERSION, DOA_LOCATION_LOCAL, 'text/plain', '1.0').towire()
record3 = DOA(0, DOA_TYPE_FIRMWARE_SIG, DOA_LOCATION_LOCAL, 'text/plain', 'something').towire()

