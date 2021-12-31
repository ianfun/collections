const websocket = require("websocket");
const {spawn} = require("child_process");
const http = require("http");
const fs = require("fs");
let server = http.createServer((req, res)=>{fs.createReadStream("terminal.html").pipe(res);});
let ws = new websocket.server({httpServer: server});
ws.on("request", (req)=>{
	req.accept(null, req.origin);
})
ws.on("connect", (req)=>{
		let proc=null;
		req.on("message", (m)=>{proc.stdin.write(m.utf8Data);});
		proc = spawn("cmd", []);
		proc.on("error", (e)=>{req.send(e);});
		proc.on("close", (code)=>{req.send(code.toString())});
		proc.stdout.on("data", (sd)=>{req.send(sd);});
		proc.stderr.on("data", (sd)=>{req.send(sd);});
	}
)

server.listen(80);
process.stdout.write("server ready at http://127.0.0.1")