var SerialPort = require("serialport");
var app = require('express')();
var http = require('http').Server(app);
var io = require('socket.io')(http);

var portName = process.argv[2],
portConfig = {
	baudRate: 9600,
	parser: SerialPort.parsers.readline("\n")
};
// Temperature Dictionary for Sensor Data
var temp_dict = {};

var sp = new SerialPort(portName, portConfig);

app.get('/', function(req, res){
  res.sendfile('temperature.html');
});

io.on('connection', function(socket){
  console.log('a user connected');
  socket.on('disconnect', function(){
  	console.log('a user has disconnected')
		// DELETE FROM temp_dict once disconnected
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
sp.on("open", function () {
  console.log('open');
  sp.on('data', function(data) {


  	if(n == 10){
  		//regKeys.forEach(availKeys.has())
  		n = -1;
  	}
  	n += 1;

  	//New Devices send this string
  	if(data[0] == "j"){
  		var key = availKeys.shift();
  		regKeys.push(key);
  		sp.write("Id is " + key)
  	}

  	if(data[0] == '{'){

       //console.log('data received: ' + data);
      //io.emit('chat message', data);
			try {
      	var parsedData = JSON.parse(data);
	      var myID = parsedData.id;
	      var temperature = parsedData.temp;
	      console.log('ID: ' + myID );
	      console.log('Temp: ' + temperature );

				//DATA EVENT
				temp_dict[myID] = temperature; 			//From the json event
				var totalTemp = 0;
				var counter = 0;
				//ADD TIME FUNCTION
				var secondsDelay = setInterval(myTimer, 2000);
				function myTimer() {
					// SEND DICTIONARY TO Client
					io.emit('temp_event', temp_dict);
				}

			} catch (e) {
				console.log('Something went wrong ' + e);

			}
		}
  });
});
