from http.server import HTTPServer, BaseHTTPRequestHandler
from socketserver import StreamRequestHandler, BaseRequestHandler
from socketserver import TCPServer, UDPServer
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
        print(self.__class__)

    def handle(self):
        self.data = self.rfile.read(1)
        self.wfile.write(gen_ascii_data())


class UDPHandler(BaseRequestHandler):
    def setup(self):
        super(UDPHandler, self).setup()
        print(self.__class__)

    def handle(self):
        self.data = self.request[0]
        socket = self.request[1]
        socket.sendto(gen_ascii_data(), self.client_address)


class HTTPHandler(BaseHTTPRequestHandler):
    def setup(self):
        super(HTTPHandler, self).setup()
        print(self.__class__)

    def do_GET(self):
        self.send_response(200)
        self.end_headers()
        self.wfile.write(gen_ascii_data())


if __name__ == '__main__':
    host, http_port, tcp_port, udp_port = '0.0.0.0', 9000, 9001, 9002
    print(len(gen_ascii_data()))

    if 'http' in sys.argv:
        http_server = HTTPServer((host, http_port), HTTPHandler)
        http_server.serve_forever()
    elif 'tcp' in sys.argv:
        tcp_server = TCPServer((host, tcp_port), TCPHandler)
        tcp_server.serve_forever()
    elif 'udp' in sys.argv:
        udp_server = UDPServer((host, udp_port), UDPHandler)
        udp_server.serve_forever()
    else:
        print("Uso: scrpt [http|tcp|udp]")
        exit(1)
