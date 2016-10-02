var SerialPort = require("serialport");
var app = require('express')();
var http = require('http').Server(app);
var io = require('socket.io')(http);

var portName = process.argv[2],
portConfig = {
	baudRate: 9600,
	//parser: SerialPort.parsers.readline('\n')
  parser: SerialPort.parsers.byteLength(4)
};
// Temperature Dictionary for Sensor Data
var temp_dict = {};

var sp = new SerialPort(portName, portConfig);

app.get('/', function(req, res){
  res.sendfile('temperature.html');
});
app.get('/canvasjs.min.js', function(req, res){
  res.sendfile('canvasjs.min.js');
});
app.get('/graph', function(req, res){
  res.sendfile('graph.html');
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
var sendReset = 0;
var sendPoll = 1;
//Create Buffer object to pass to pass to sp.write
var buffer = Buffer.allocUnsafe(4);


var maxDevices = 10;
var current = [];
var regKeys = new Array();
//var availKeys = [...Array(maxDevices).keys()]
var availKeys = new Array();
for(var i = 1; i < maxDevices+1; i++){
  availKeys[i-1] = i;
}

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
});

  availKeys.sort(function (a,b){
    return a - b;
  });

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
  buffer.writeInt32BE(sendPoll,0);
  sp.write(buffer);
  console.log("Send code: ",buffer.toString('hex'),'to poll');

  count++;
  if(count == n){
    cleanKeys();
    count = 0;
  }

  if(waiting == 1)
    waiting = 0;

  // SEND DICTIONARY TO Client
  io.emit('temp_event', temp_dict);
}

sp.on("open", function () {
  console.log('Serialport Has Started: Listening for Joins');
  //Write 0 to buffer to reset devices
  buffer.writeInt32BE(sendReset,0);
  sp.write(buffer);
  console.log("RESET ALL with code: ",buffer.toString('hex'),'of length: ',buffer.length,'bytes');

  var timing = setInterval(myTimer,3000);

  sp.on('data', function(data) {
    console.log('data:',data.toString('hex'));
    if(data.readInt16BE(0) == -1){
      console.log("Someone wants to join, resetting timer to 6 sconds once")
      var setTime = 6;
      clearTimeout(timing);
      timing = setInterval(myTimer,setTime*1000);
      waiting = 1;

      //Adjust Keys arrays
      var key = availKeys.shift();
      regKeys.push(key);
      current.push(key);
      console.log('Will send key:',key);

      //Send Data
      buffer.writeUInt16BE(data.readUInt16BE(2),0); //Place randNum first
      buffer.writeUInt16BE(key,2);  //Key goes second
      console.log('buffer sent: ',buffer.toString('hex'));
      sp.write(buffer);
    }

  	else{
      if(!waiting){
        //Set timer to poll every setTime seconds
        console.log("Entered Wait and Resetting timer to 3 seconds")
        clearTimeout(timing);
        var setTime = 3;
        timing = setInterval(myTimer,setTime*1000);
        waiting = 2;
      }
			try {
        //data[0] holds higher 8 bits
	      var myIDmsb = (data.readUInt8(0)-1); //These are both integers
        var myIDlsb = data.readUInt8(1);
        var myID = (myIDmsb << 8) | myIDlsb;
	      var temperature = data.readUInt8(2) + data.readUInt8(3)/100;
        console.log("Got id: ",myID);
        console.log("Got temperature: ",temperature)

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
