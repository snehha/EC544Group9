var SerialPort = require("serialport");
var app = require('express')();
var http = require('http').Server(app);
var io = require('socket.io')(http);

var portName = process.argv[2],
portConfig = {
	baudRate: 9600,
	parser: SerialPort.parsers.readline("\n")
};

var sp = new SerialPort(portName, portConfig);

app.get('/', function(req, res){
  res.sendfile('index.html');
});

io.on('connection', function(socket){
  console.log('a user connected');
  socket.on('disconnect', function(){
  	console.log('a user has disconnected')
  });
  socket.on('chat message', function(msg){
    io.emit('chat message', msg);
    sp.write(msg + "\n");
  });
});

http.listen(3000, function(){
  console.log('listening on *:3000');
});

var key = 0;

sp.on("open", function () {

  console.log('Serialport Has Started');
  //-------Check for Empty Dictionary-------// In case the coordinator reset

  
  //Poll every two seconds (Send a unique get string)
  setInterval(function(){
    sp.write("s");
    console.log("Sending Get: s");
  },2000);

  sp.on('data', function(data) {

  	//Listen for Joins
  	if(data == "j"){
  		key++;
  		console.log("Sending key "+key)
  		sp.write("i" + key.toString())
  	}

    //Get Temperature
  	else{
	    console.log('data received: ' + data);
	    io.emit('chat message', data);
	}
  });
});