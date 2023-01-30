#!/usr/bin/env python
from http.server import HTTPServer, SimpleHTTPRequestHandler
 
port=8001
print("Running on port %d" % port)
 
SimpleHTTPRequestHandler.extensions_map['.wasm'] = 'application/wasm'
 
httpd = HTTPServer(('localhost', port), SimpleHTTPRequestHandler)
 
print("Mapping \".wasm\" to \"%s\"" % SimpleHTTPRequestHandler.extensions_map['.wasm'])
httpd.serve_forever()

