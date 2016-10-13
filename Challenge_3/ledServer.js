var app     =     require("express")();
var SerialPort = require("serialport");
var http    =     require('http').Server(app);
var io      =     require("socket.io")(http);

app.get('/', function(req, res){
  res.sendFile(__dirname + '/webview/test.html');
});

app.get('/1k.js', function(req, res){
  res.sendFile(__dirname + '/webview/1k.js');
});

var portName = process.argv[2],
portConfig = {
	baudRate: 9600,
	parser: SerialPort.parsers.byteLength(4)
};
var sp = new SerialPort(portName, portConfig);
var ledNum = 3;
var ledMap = {}; // ledID : [R,G,B]
//Create Buffer object to pass to pass to sp.write
var buffer = Buffer.allocUnsafe(4);

for( var i = 1; i <= ledNum; i++) {
	var led = [0,0,0]; // [R, G, B]
	ledMap['bulb-' + i] = led;
}

io.on('connection', function(socket){

  console.log('a user connected');
  buffer.writeInt32BE(-1, 0);
  sp.write(buffer);
  // when website connects, send status of LEDs
  console.log("Sending LED status to website: ");
  console.log(ledMap);

  socket.on('disconnect', function(){
  });

  // received command from website to toggle led and color
  socket.on('toggle led', function(id,R,G,B){
  	console.log("Website command: " + id + " " + R + " " + G + " " + B);
  	// send led num and RGB to arduino
	var ledCommand = (id << 8) | R;
	ledCommand = (ledCommand << 8) | G;
	ledCommand = (ledCommand << 8) | B;
	buffer.writeInt32BE(ledCommand,0);
	console.log("Sending command ID,R,G,B: " + buffer.toString('hex'));
	sp.write(buffer);
  });

  socket.on('load arduino', function(){
    console.log("Sending arduino led request");
    buffer.writeInt32BE(-1, 0);
    sp.write(buffer);
  });

});



http.listen(3000, function(){
  console.log('listening on *:3000');
});

sp.on("open", function () {
  console.log('open');
  // receives status of LEDs (on/off and color)
  sp.on('data', function(data) {
    console.log('data received: ' + data.toString('hex'));
	//data[0] holds higher 8 bits
    var id = data.readUInt32BE(0) >> 24;
    console.log("ID: " + id);
    var R = (data.readUInt32BE(0) >> 16) & 255;
    console.log("Red: " + R);
    var G = (data.readUInt32BE(0) >> 8) & 255;
    console.log("Green: " + G);
    var B = data.readUInt32BE(0) & 255;
    console.log("Blue: " + B);
    if( id > 0  && id <= ledNum) {
    	ledMap["bulb-"+id] = [R,G,B];
    	console.log(ledMap);
    }
    io.emit('led status', ledMap); // ledID : [on/off,R,G,B]
  });

});
