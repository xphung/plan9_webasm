#!/usr/bin/env python
from http.server import HTTPServer, SimpleHTTPRequestHandler

port=8001
print("Running on port %d" % port)

class MyHTTPRequestHandler(SimpleHTTPRequestHandler):
    def end_headers(self):
        self.send_header('Cross-Origin-Opener-Policy', 'same-origin')
        self.send_header('Cross-Origin-Embedder-Policy', 'require-corp')
        SimpleHTTPRequestHandler.end_headers(self)

MyHTTPRequestHandler.extensions_map['.wasm'] = 'application/wasm'

httpd = HTTPServer(('localhost', port), MyHTTPRequestHandler)

print("Mapping \".wasm\" to \"%s\"" % MyHTTPRequestHandler.extensions_map['.wasm'])
httpd.serve_forever()

