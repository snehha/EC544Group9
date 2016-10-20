var app = require('express')();
var http = require('http').Server(app);
var io = require('socket.io')(http);
//database file
var fs = require("fs");
var file = "historicalDB.db";
var exists = fs.existsSync(file);

// create database
if(!exists) {
  console.log("Creating DB file.");
  fs.openSync(file, "w");
}
var sqlite3 = require("sqlite3").verbose();
var db = new sqlite3.Database(file);
var dbTables = [];
// Create table for temperatures
db.serialize(function() {
  if(!exists) {
    db.run("CREATE TABLE temperatureAverages (temperature FLOAT, timeReceived TEXT)");
  }
  var temperatureDB = db.prepare("INSERT INTO temperatureAverages VALUES (?,?)");
  temperatureDB.finalize();
});

app.get('/', function(req, res){
  res.sendfile('temperature.html');
});
app.get('/canvasjs.min.js', function(req, res){
  res.sendfile('canvasjs.min.js');
});
app.get('/graph', function(req, res){
  res.sendfile('graphb.html');
});

io.on('connection',function(socket){
	//Website events
	// send database to graph.html
  socket.on('loadDB',function(){
    console.log("loading Db");
      var tempDB = {};
	  // Load average temperatures and time from database
	  db.each("SELECT rowid AS id, temperature, timeReceived FROM temperatureAverages", function(err, row) {
		tempDB[row.timeReceived] = row.temperature;
	  });
	  setTimeout(emitDB,1000);
	  function emitDB() {
      	io.emit('load graph', tempDB, dbTables);
      	console.log("Sending database to website");
      }
    });
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

	//Get list of devices currently on
	var devicesPr = particle.listDevices({ auth: myToken });
	devicesPr.then(
	  function(devices){
	  	listDev = devices.body;

	  	// create database table for devices
	  	for( var dev in listDev) {
	  		var tableName = "photon" + listDev[dev].id;
  			db.run("CREATE TABLE IF NOT EXISTS " + tableName + " (temperature FLOAT, timeReceived TEXT)");
  			console.log("Creating database table if it doesn't exist for " + tableName);
  			dbTables.push(tableName);
	  	}

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
		let name = listDev[i].name;
		particle.getVariable({ deviceId: id, name: 'temperature', auth: myToken }).then(function(data) {
		  //console.log('Device variable retrieved successfully from id:',id,'with data:',data);
		  sendTemp(name,data);
		  saveTemp(id,data);
		}, function(err) {
		  console.log('An error occurred while getting attrs from device with id:',id);
		  if(name in temp_dict) {
			delete temp_dict[name];
	      	console.log("Removing ", name);
	      }
		});
	}
	//console.log("-------------------------------------");
}

// Temperature Dictionary for Sensor Data
var temp_dict = {};

function sendTemp(name,data){
	temp_dict[name] = data.body.result;
}

function saveTemp(id,data){
	var temperature = data.body.result;
	if(temperature) {
		var tableName = "photon" + id;
		var dateReceived = new Date().toISOString();
		var temperatureDB = db.prepare("INSERT INTO " + tableName + " VALUES (?,?)");
	    temperatureDB.run(temperature, dateReceived);
	    temperatureDB.finalize();    
	    console.log("Storing " + id + " temperature into database: " + temperature + " " + dateReceived);
	}
}
var timing = setInterval(myTimer,2000);

function myTimer() {
  // SEND DICTIONARY TO Client
  io.emit('temp_event', temp_dict);
  // calculate average temperature
  var average = 0;
  var count = 0;
  for(var i in temp_dict) {
    average += parseFloat(temp_dict[i]);
    count++;
  }
  // save average to database
  if( count != 0 ){
    average = average / count;
    average = average.toFixed(2)
    var dateReceived = new Date().toISOString();
    var temperatureDB = db.prepare("INSERT INTO temperatureAverages VALUES (?,?)");
    temperatureDB.run(average,dateReceived);
    temperatureDB.finalize();    
    console.log("Storing average into database: " + average + " " + dateReceived);
    // Send average to graph client
    io.emit('refresh graph',dateReceived,average);
  } 
}