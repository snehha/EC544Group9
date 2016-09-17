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
var key = 0;


//function myTimer() {
  /*for (var key in temp_dict){
    totalTemp += temp_dict[key];
    counter++;
  }*/
  /*sp.write('s');
  console.log("SENDING POLL NOW")
  //var average = totalTemp/counter;
   //Sent to HTML Client
  //io.emit('temp_event', average);
}*/

sp.on("open", function () {
  console.log('Serialport Has Started');
  //-------Check for Empty Dictionary-------// In case the coordinator reset
  sp.write('r');

  setInterval(myTimer, 2000);

  sp.on('data', function(data) {
    console.log('data received: ' + data);
    //Listen for Joins
    if(data[0] == "j"){
      key++;
      //console.log("Sending key: " + "i" + data.slice(-(data.length-1)) + "," + key.toString()+"\n")
      var str = "i" + data.slice(-(data.length-1)) + "," + key.toString()+"\n";
      sp.write(str);
    }

  	if(data[0] == '{'){

			try {
      	var parsedData = JSON.parse(data);
	      var myID = parsedData.ID;
	      var temperature = parsedData.temp;
	      console.log('ID: ' + myID );
	      console.log('Temp: ' + temperature );

				//DATA EVENT
				temp_dict[myID] = temperature; 			//From the json event
				var totalTemp = 0;
				var counter = 0;
				//ADD TIME FUNCTION

				function myTimer() {
					// SEND DICTIONARY TO Client
          sp.write('s');
					io.emit('temp_event', temp_dict);
				}

			} catch (e) {
				console.log('Something went wrong: ' + e);

			}
		}
  });
});
