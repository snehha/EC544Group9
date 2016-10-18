var app = require('express')();
var http = require('http').Server(app);
var io = require('socket.io')(http);
//database file
var fs = require("fs");
var file = "historicalDB.db";
var exists = fs.existsSync(file);

app.get('/', function(req, res){
  res.sendfile('index.html');
});

io.on('connection',function(socket){
	//Website events
});

http.listen(3000, function(){
  console.log('listening on *:3000');
});

var Particle = require('particle-api-js');
var particle = new Particle();

var uName = 'phosho9@gmail.com';
var pWord = 'ilikepie9';

var listDev;
var myToken;

var token = particle.login({username:uName,password:pWord}).then(
//Success
function(data){
	console.log("API call completed on promise resolve:",data.body.access_token);

	//Get list of devices once logged in
	myToken = data.body.access_token;


	//----------EVENT SETUP-----------//
	//Publish Event
	/*var publishEventPr = particle.publishEvent({ name: 'sendData', data: {}, auth: token });
	publishEventPr.then(
	  function(data) {
	    if (data.body.ok) { console.log("Event published succesfully") }
	  },
	  function(err) {
	    console.log("Failed to publish event: " + err)
	  }
	);*/


	//Grab Event stream
	/*particle.getEventStream({ auth: token}).then(function(stream) {
	  stream.on('event', function(data) {
	    console.log("Event: " + data);
	  });
	});*/
	//--------------------------------//

	//Get list of devices currently on
	var devicesPr = particle.listDevices({ auth: myToken });
	devicesPr.then(
	  function(devices){
	  	listDev = devices.body;

	  	//Flash device if reset is on
//RUN	//node index.js reset
	  	if(process.argv[2] == "reset"){
		    for(var i = 0; i < devices.body.length; i++){
		    	let id = devices.body[i].id;
	    		//Flash firware if user wants reset
				particle.flashDevice({ deviceId: id, files: { file1: './router.router.ino' }, auth: token }).then(function(data) {
				  console.log('Device flashing started successfully:', data);
				}, function(err) {
				  console.log('An error occurred while flashing the device:', err);
				});
			}
		}

		//Set up timer to read data
		var myTimer = setInterval(getTemp,4000);

	  },
	  function(err) {
	    console.log('List devices call failed: ', err);
	  }
	);

},
//Failure
function(err){
	console.log("API call completed on promise fail: ",err)
});

function getTemp(){
	//console.log("-------------------------------------");
	for(var i = 0; i < listDev.length; i++){
		let id = listDev[i].id;
		particle.getVariable({ deviceId: id, name: 'temperature', auth: myToken }).then(function(data) {
		  console.log('Device variable retrieved successfully from id:',id,'with data:',data);
		  sendTemp(id,data);
		}, function(err) {
		  console.log('An error occurred while getting attrs from device with id:',id);
		});
	}
	//console.log("-------------------------------------");
}

function sendTemp(id,data){

}