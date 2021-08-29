
#!/usr/bin/env python
# test in windows

'''
a simple server for run [C, python, haskell] in borwser
'''

from http.server import *
import sys
import subprocess as sub
from webbrowser import open as _open
path = sys.executable
from os import getcwd, sep
end = 'py'
def execute(t):
    if len(t)==0:
        return b'no input(from server)'
    if t[0] == ord('!'):
        if t[1]==ord('p'):
                globals()['path'] = sys.executable
                globals()['end']='py'
        elif t[1]==ord('c'):
                globals()['path']='gcc'
                globals()['end'] = 'c'
        elif t[1]==ord('h'):
                globals()['path']='runghc'
                globals()['end'] = 'hs'
        return b'change excute path to '+path.encode('ascii')

    with open("temp.%s" % end, 'wb') as w:
        w.write(t)
    x = sub.Popen(('gcc -Wall -o temp.exe temp.c' if path=='gcc' else [path, "temp.%s" % end]), stderr=-1, stdout=-1)
    r = b''
    while x.poll() is None:
        r += x.stdout.read() + x.stderr.read()
    if end=='c' and not r:
        X = sub.Popen("temp.exe", stdout=-1, stderr=-1) # temp.out
        while X.poll() is None:
            r += X.stdout.read() + X.stderr.read()
        r += X.stdout.read() + X.stderr.read()
    return r + x.stdout.read() + x.stderr.read()

class server(BaseHTTPRequestHandler):
    '''
    POST: run code server
    GET:  python shell server
    '''
    def do_GET(self):
        self.send_response(200)
        self.send_header("Content-type", "text/plain")
        self.send_header('Access-Control-Allow-Origin', '*')
        self.end_headers()
        if sub.Popen([sys.executable, "shell.py"]):
            print("start python shell server(subprocess)")
            _open(getcwd() + sep + "shell.html")
            self.wfile.write(b"ok")
        else:
            self.wfile.write(b"can't create subprocess")
    def do_POST(self):
        self.send_response(200)
        self.send_header("Content-type", "text/plain")
        self.send_header('Access-Control-Allow-Origin', '*')
        self.end_headers()
        self.wfile.write(execute(self.rfile.read(int(self.headers['Content-Length']))))

with HTTPServer(("localhost", 8000), server) as s:
    print("server at ", s.socket.getsockname())
    _open(getcwd() + sep + 'client.html')
    s.serve_forever()
