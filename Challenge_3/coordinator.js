//var SerialPort = require("serialport");
var app = require('express')();
var http = require('http').Server(app);
var io = require('socket.io')(http);

// var portName = process.argv[2],
// portConfig = {
// 	baudRate: 9600,
// 	parser: SerialPort.parsers.readline("\n")
// };
// var sp;
// sp = new SerialPort.SerialPort(portName, portConfig);

app.get('/', function(req, res){
  res.sendfile('webview/test.html');
});
app.get('/1k.js', function(req, res){
  res.sendfile('webview/1k.js');
});

io.on('connection', function(socket){
	// Initial Connection sends server data
	var bulbData = {'bulb-1': '255,122,12', 'bulb-2': '123,221,45', 'bulb-3': '101,201,110'};
	io.emit('bulbFromServer', bulbData);
  console.log('a user connected');

  socket.on('disconnect', function(){
		console.log('a user disconnected');
  });

  socket.on('bulb-init', function(msg){
		var bulbData = {'bulb-1': '255,122,12', 'bulb-2': '123,221,45', 'bulb-3': '101,201,110'};
    io.emit('bulbFromServer', bulbData);
  });

	socket.on('bulb-change', function(colorChangeKV){
		console.log(colorChangeKV);
		//msg.parse()
    io.emit('bulbFromServer', colorChangeKV);
  });
});

http.listen(3000, function(){
  console.log('listening on *:3000');
});

// sp.on("open", function () {
//   console.log('open');
//   sp.on('data', function(data) {
//     console.log('data received: ' + data);
//     io.emit("chat message", "An XBee says: " + data);
//   });
// });
