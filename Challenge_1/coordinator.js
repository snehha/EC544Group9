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
var n = 3;
var count = 0;

function cleanKeys(){
  console.log("------Debugging-------");
  console.log("Registered Keys: ",regKeys);
  console.log("Available Keys: ",availKeys);
  console.log("Current is: ",current);
  regKeys = regKeys.filter(function(element,index,array){
  	console.log("Checking:",element);
    if(!(current.includes(element))){
      availKeys.unshift(element);
      delete temp_dict[element];
      console.log("Removing",element);
      return false;
    }
    else
      return true;
  });
  regKeys = regKeys.filter(function(element, index, array) {
    return index == array.indexOf(element);
})
  availKeys.sort();

  current.length = 0;
  console.log("Registered Keys: ",regKeys);
  console.log("Available Keys: ",availKeys);
  console.log("Current is: ",current);
  console.log("------Debugging-------");
}

var waiting = 1;

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

  if(waiting == 1)
    waiting = 0;
}

sp.on("open", function () {
  console.log('Serialport Has Started: Listening for Joins');
  //Send out r to reset all the arduino ids
  sp.write('r');
  console.log("RESET ALL");

  var timing = setInterval(myTimer,3000);

  sp.on('data', function(data) {
    console.log('data received: ' + data);
    //Listen for Joins
    if(data[0] == "j"){
      clearTimeout(timing);
      timing = setInterval(myTimer,6000);
      waiting = 1;
      var key = availKeys.shift();
      regKeys.push(key);
      current.push(key);
      var str = "i" + data.slice(-(data.length-1)) + "," + key.toString() + "\n";
      console.log("data sent: " + str);
      sp.write(str);
    }

  	if(data[0] == '{'){
      if(!waiting){
        console.log("!wait");
        clearTimeout(timing);
        timing = setInterval(myTimer,3000);
        waiting = 2;
      }
			try {
      	var parsedData = JSON.parse(data);
	      var myID = parsedData.id;
	      var temperature = parsedData.temp;

				//DATA EVENT
				temp_dict[myID] = temperature; 			//From the json event
				var totalTemp = 0;
				var counter = 0;

				//Make sure arrays are up to date based on current responses
        if(!current.includes(myID))
          current.push(myID);
        if(!regKeys.includes(myID))
        	regKeys.push(myID);
        if(availKeys.includes(myID))
          availKeys.splice(availKeys.indexOf(myID),1);

			} catch (e) {
				console.log('Something went wrong: ' + e);

			}
		}
  });
});
