from http.server import HTTPServer, BaseHTTPRequestHandler
from socketserver import StreamRequestHandler, BaseRequestHandler
from socketserver import TCPServer, UDPServer
from threading import Thread
import datetime
import ssl
import sys

def gen_ascii_data(bytes_=512, line_width=80):
    lines = bytes_ // line_width
    text = '\n'.join(['A' * (line_width - 1)] * lines)
    text += '\n'
    last_line_bytes = bytes_ - len(text)
    if last_line_bytes > 0:
        text += 'B' * (last_line_bytes - 1) + '\n'
    return text.encode('utf-8')

class TCPHandler(StreamRequestHandler):
    def setup(self):
        super(TCPHandler, self).setup()
        self.log = open(self.__class__.__name__ + '.log', 'a')
        print(self.__class__)

    def handle(self):
        self.log.write(datetime.datetime.now().isoformat() + '\n')
        self.data = self.rfile.read(1)
        self.wfile.write(gen_ascii_data())


class UDPHandler(BaseRequestHandler):
    def setup(self):
        super(UDPHandler, self).setup()
        self.log = open(self.__class__.__name__ + '.log', 'a')
        print(self.__class__)

    def handle(self):
        self.log.write(datetime.datetime.now().isoformat() + '\n')
        self.data = self.request[0]
        socket = self.request[1]
        socket.sendto(gen_ascii_data(), self.client_address)


class HTTPHandler(BaseHTTPRequestHandler):
    def setup(self):
        super(HTTPHandler, self).setup()
        self.log = open(self.__class__.__name__ + '.log', 'a')
        print(self.__class__)

    def do_GET(self):
        self.log.write(datetime.datetime.now().isoformat() + '\n')
        self.send_response(200)
        self.end_headers()
        self.wfile.write(gen_ascii_data())


def http_fun():
    http_server = HTTPServer((host, http_port), HTTPHandler)
    http_server.serve_forever()


def https_fun():
    https_server = HTTPServer((host, https_port), HTTPHandler)
    try:
        https_server.socket = ssl.wrap_socket(  https_server.socket,
                                                server_side=True,
                                                certfile='certificate.pem',
                                                ssl_version=ssl.PROTOCOL_TLSv1)
    except ssl.SSLError as e:
        print(e.library, e.reason)

    https_server.serve_forever()


def tcp_fun():
    tcp_server = TCPServer((host, tcp_port), TCPHandler)
    tcp_server.serve_forever()


def udp_fun():
    udp_server = UDPServer((host, udp_port), UDPHandler)
    udp_server.serve_forever()


if __name__ == '__main__':
    host, http_port, tcp_port, udp_port, https_port = '0.0.0.0', 9000, 9001, 9002, 9003
    print(len(gen_ascii_data()))

    http_t = Thread(target=http_fun)
    https_t = Thread(target=https_fun)
    tcp_t = Thread(target=tcp_fun)
    udp_t = Thread(target=udp_fun)

    threads = (http_t, https_t, tcp_t, udp_t)
    for t in threads:
        t.daemon = True
        t.start()

    while any(map(Thread.is_alive, threads)):
        for each in threads:
            if each.is_alive():
                each.join(timeout=0.1)

