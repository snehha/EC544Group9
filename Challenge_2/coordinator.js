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

//Format of strings to send
var msgSend = "";
var sendReset = 0;
var sendPoll = 1;
//Variable to receive message
var msgReceive = "";
//4-byte buffer to store the messages to send
var buffer = new ArrayBuffer(4);
//Views for the buffer
var int16View = new Int16Array(buffer); //Will be used to send pair of two bytes
var int32View = new Int32Array(buffer); //Will be used to send poll/reset

var maxDevices = 10;
var regKeys = new Array();
//var availKeys = [...Array(maxDevices).keys()]
var availKeys = [];
for(var i = 0; i < maxDevices; i++){
  availKeys[i] = i;
}
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
  //Write 1 to buffer to send
  int32View = sendPoll;
  sp.write(buffer);

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
  //Write 0 to buffer to reset devices
  int32View = sendReset;
  sp.write(buffer);
  console.log("RESET ALL");

  var timing = setInterval(myTimer,3000);

  sp.on('data', function(data) {
    console.log('data received: ' + data);
    //Listen for Joins -> FFFF
    if(data[0] == 255 && data[1] == 255){
      //Reset timer to setTime seconds 
      var setTime = 6;
      clearTimeout(timing);
      timing = setInterval(myTimer,setTime*1000);
      waiting = 1;

      //Adjust Keys arrays
      var key = availKeys.shift();
      regKeys.push(key);
      current.push(key);

      //Send Data
      int16View[0] = parseInt(data.slice(2,4)); //Place randNum first
      int16View[1] = key; //Place id second
      //console.log("data sent: " + str);
      sp.write(buffer);
    }

  	else{
      if(!waiting){
        //console.log("!wait");
        //Set timer to poll every setTime seconds
        clearTimeout(timing);
        var setTime = 3;
        timing = setInterval(myTimer,setTime*1000);
        waiting = 2;
      }
			try {
        //data[0] holds higher 8 bits
	      var myID = (((0 | data[0]) << 8) | data[1]); //These are both integers
	      var temperature = data[2] + data[3] / 100;

				//DATA EVENT
				temp_dict[myID.toString()] = temperature; 			//From the json event

				//var totalTemp = 0;
				var counter = 0;
				//Make sure arrays are up to date based on current responses
        if(!current.includes(myID.toString()))
          current.push(myID);
        if(!regKeys.includes(myID.toString()))
        	regKeys.push(myID);
        if(availKeys.includes(myID.toString()))
          availKeys.splice(availKeys.indexOf(myID.toString()),1);

			} catch (e) {
				console.log('Something went wrong: ' + e);

			}
		}
  });
});
