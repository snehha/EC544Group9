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

var maxDevices = 10;
var regKeys = new Array();
var availKeys = [...Array(maxDevices).keys()]

var current = []

//Every nth iteration, check if the keys received are all in the regKeys, else remove the ones not there and add it to availKeys
var n = 5;
var count = 0;

function cleanKeys(){
  /*console.log("Registered Keys: ",regKeys);
  console.log("Available Keys: ",availKeys);
  console.log("Current is: ",current);*/

  regKeys = regKeys.filter(function(element,index,array){
  	console.log("Working on:",element);
    if(!(current.includes(element))){
      availKeys.unshift(element);
      delete temp_dict[element];
      console.log("Removing",element);
      return false;
    }
    else
      return true;
  });

  /*console.log("Registered Keys: ",regKeys);
  console.log("Available Keys: ",availKeys);
  console.log("Current is: ",current);*/
}

function myTimer() {
  console.log("-----------------------------POLLING-----------------------------");
  sp.write('s');
  // SEND DICTIONARY TO Client
  io.emit('temp_event', temp_dict);
  count++;
  if(count == n){
    cleanKeys();
    count = 0;
  }
  current.length = 0;
}

sp.on("open", function () {
  console.log('Serialport Has Started: Listening for Joins');
  //Send out r to reset all the arduino ids
  sp.write('r');
  console.log("RESET ALL");

  //Poll every 2 seconds by sending s
  var timing = setInterval(myTimer, 3000);

  sp.on('data', function(data) {
    console.log('data received: ' + data);
    //Listen for Joins
    if(data[0] == "j"){
      clearTimeout(timing);
      timing = setInterval(myTimer,3000);
      var key = availKeys.shift();
      regKeys.push(key);
      current.push(key);
      var str = "i" + data.slice(-(data.length-1)) + "," + key.toString() + "\n";
      console.log("data sent: " + str);
      sp.write(str);
    }

  	if(data[0] == '{'){

			try {
      	var parsedData = JSON.parse(data);
	      var myID = parsedData.id;
	      var temperature = parsedData.temp;

				//DATA EVENT
				temp_dict[myID] = temperature; 			//From the json event
				var totalTemp = 0;
				var counter = 0;
				//ADD TIME FUNCTION
        current.push(myID);
        if(!regKeys.includes(myID))
        	regKeys.push(myID);
        console.log(current);

			} catch (e) {
				console.log('Something went wrong: ' + e);

			}
		}
  });
});
