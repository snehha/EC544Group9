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
  res.sendfile('temperature.html');
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

var maxDevices = 4;
var regKeys = new Array();
var availKey = new Array(maxDevices);

//Every nth iteration, check if the keys received are all in the regKeys, else remove the ones not there and add it to availKeys
var n = 10;
var temp_dict = {};
sp.on("open", function () {
  console.log('open');
  sp.on('data', function(data) {


  	if(n == 10){
  		//regKeys.forEach(availKeys.has())
  		n = -1;
  	}
  	n += 1;

  	//New Devices send this string
  	if(data == "Permission to Join"){
  		var key = availKeys.shift();
  		regKeys.push(key);
  		sp.write("Id is " + key)
  	}

  	else{
			console.log('data received: ' + data);

			//DATA EVENT
			temp_dict[myID] = temperature; 			//From the json event
			var totalTemp;
			var counter;
			//ADD TIME FUNCTION
			var secondsDelay = setInterval(myTimer, 2000);
			function myTimer() {
				for (var key in temp_dict){
					temp_dict[key] -> totalTemp;
					counter++;
				}
				var average;
				average = totalTemp/counter;
			}
			//Sent to HTML Client
	    io.emit('temp_event', average);
	}
  });
});
