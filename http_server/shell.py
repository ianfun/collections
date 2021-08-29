from http.server import *
from code import InteractiveConsole
import sys
from contextlib import redirect_stdout, redirect_stderr
from io import StringIO

sys.ps1 = ">>> "
sys.ps2 = "... "
python = InteractiveConsole()

class handler(BaseHTTPRequestHandler):
    def do_GET(self):
        self.send_response(200)
        self.send_header("Content-type", "text/plain")
        self.send_header('Access-Control-Allow-Origin', '*')
        self.end_headers()
        self.wfile.write(("Python %s on %s\nType \"help\", \"copyright\", \"credits\" or \"license\" for more information." %(sys.version, sys.platform)).encode('utf-8'))

    def do_POST(self):
        self.send_response(200)
        self.send_header("Content-type", "text/plain")
        self.send_header('Access-Control-Allow-Origin', '*')
        self.end_headers()
        s=StringIO()
        with redirect_stderr(s), redirect_stdout(s):
            s.write(sys.ps2 if python.push(self.rfile.read(int(self.headers['Content-Length'])).decode('utf-8')) else sys.ps1)
        self.wfile.write(s.getvalue().encode('utf-8'))

with HTTPServer(("", 7999), handler) as s:
    print("shell server started")
    s.serve_forever()
