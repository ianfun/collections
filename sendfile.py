# use this to send file to your compute, smart-phone, and other !
# test in iPhone 5s
from http.server import *
from tkinter.filedialog import askopenfilename

class Server(BaseHTTPRequestHandler):
	def do_GET(self):
		self.send_response(200)
		self.send_header('Accept-Ranges', 'bytes')
		self.end_headers()
		for i in open(path, 'rb'):
			self.wfile.write(i)


path = askopenfilename()
if not path:
	exit(0)
# replace with your IP (use ipconfig to get it)
with HTTPServer(('192.168.0.108', 80), Server) as s:
	print(path)
	print("wait for iphone")
	s.serve_forever()