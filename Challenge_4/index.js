var app = require('express')();
var http = require('http').Server(app);
var io = require('socket.io')(http);
var firebase = require("firebase");
var shortid = require("shortid");

firebase.initializeApp({
  serviceAccount: {
    projectId: "group9-node",
    clientEmail: "nodejs@group9-node.iam.gserviceaccount.com",
    privateKey: "-----BEGIN PRIVATE KEY-----\nMIIEvQIBADANBgkqhkiG9w0BAQEFAASCBKcwggSjAgEAAoIBAQDxXYNc283NWCyk\njL1mLDfDKtNXarzWenPclEU23FC/nAYYqQSvBr4yleaPCfwdOw48Uttx2PwzAdE2\nmQOZg6+qN3vb4fqQLM5CNWftKuFouuU/OJBAOM4G1TnjxWrAzJxQV/don0QW0BV+\n5qi2qkEGnaXeKYdXmHJawxMzDsyOeT0clKFb/9Pt4LnW/g1VaSQRDPbhnqCamsp/\nv5RwSAY1brKZndJ/c6deBkv0lv2Al8YPO9QHKzY2Z6xPQDysLDFHlMa0k8I7jZkU\naKTkDPThgpW/hNKGd6iiWtOQ146xsC38nAO2A/jipP7ZVCgxp30HaRsuOgSOMxpU\ndCgz8iiTAgMBAAECggEAMyqUv2mlPiO0Cwn+2JsFEy2P8dchwwHgb+FKru+TepsT\nu8SQxp3SUhzu7GG8fWHYibcy5/aMuC9pb36OgculwJrUee900318GBMEPgW6FR6R\nnI5cHhEss8wd4ogmMkrt1CZhv18L6x4fgHBbUaXT9RgUYn1BQLODgnQaLNe1S3pE\nqlkuRPQOqqk96H1fkoSk2roruySXJvtoDkUFdPGFwBC3c5+8OPR555EAtRZAdcad\n2x+RjrBzLbTL7ZQdoXjBGoiuH/A0FXKYp5CBXWqJBvtyoDDV6rUrE9T441T+cJAz\nc/3FizNrc4s/6SLdgt3SkmlUnsfZ/Gt0Sc0C+foXqQKBgQD/AGymGWN2VJctLIDB\n5Tzk4ZPQRtB34tDxNedyUfKGEuasNSTuJViCGNaNjXpluXVNtztDoQoj1Y0cTrbz\nG0N0c9H5sJhAnofk4NlucRf/XJXZ8D/m4PEWr+3o2URcCjUDBlV8TFtYhJemu1JQ\nU16ZkOwWTie9faXw6YeYuxhgLwKBgQDyT2vyFNCX7IpFfTqIvsuAJo3ouPtowg9o\nhiJvJGDWp7W2mWiDfUT595NaRXJ4C/5EyoPuyxtFFdb538XoJwqhwNdCtoLeB2eV\ndoS/aj4yR8uN/U8EKYzgPcoDT5ieli9qfkJfhQ9EycLDe0lYQECkzutNJGNp+luU\n0kEt4qLg3QKBgCFZcdwrN9nS6E6NIm25SER6x/UWPc9gB4l0TkeiNgCY2jgtXx4S\nHkgtbWnn1dkV7yRx0TtgBU54cFMCbGTQ7Pp+5zgrg034LbeePHF4MvY/qo1tIT5V\nrtITwh6Qw6Lx5sr1ehqeddfWx2qT1wkQTi/xRx8Hq6TZmOkv8X7lASQbAoGADQ4g\nZ8OdVYImFQZhP/rfpgDtxmspCITkJaKMemaAXTBeBv+O9P6r17fyFXwGZddnlLdA\nkn4Y8wjxMzdOR2rLFNn5/xssQ+AsQY5IKrQDs9vQaM8MEdJXR8Gsf68rLugyl89D\nrjfSEce5GaUr13hmwzpuzRI31P7rLkKBxoIeenUCgYEApO3tCAhxMdUglJywad5i\nq4Mr4Sm2RbntvYG4rZb6Ntei5nO5DVh3o9ZIUPE1PPEPg7039eK7mRabHfARSIoK\ng/HfMgxiiPal5T1y3Dj+u4xordb5/CmbgTYKtJ28aSJEXtlz8AnTk3r5lhUOo7Oh\nLEE3bYnxifRluNaJenJr+AU=\n-----END PRIVATE KEY-----\n"
  },
  databaseURL: "https://group9-node.firebaseio.com"
});

var db = firebase.database();

var ref = db.ref("temperatures");

// Attach an asynchronous callback to read the data at our posts reference
ref.on("value", function(snapshot) {
  console.log(snapshot.val());
}, function (errorObject) {
  console.log("The read failed: " + errorObject.code);
});



app.get('/', function(req, res){
  res.sendfile('temperature.html');
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


firebase.initializeApp({
  serviceAccount: {
    projectId: "group9-node",
    clientEmail: "nodejs@group9-node.iam.gserviceaccount.com",
    privateKey: "-----BEGIN PRIVATE KEY-----\nMIIEvQIBADANBgkqhkiG9w0BAQEFAASCBKcwggSjAgEAAoIBAQDxXYNc283NWCyk\njL1mLDfDKtNXarzWenPclEU23FC/nAYYqQSvBr4yleaPCfwdOw48Uttx2PwzAdE2\nmQOZg6+qN3vb4fqQLM5CNWftKuFouuU/OJBAOM4G1TnjxWrAzJxQV/don0QW0BV+\n5qi2qkEGnaXeKYdXmHJawxMzDsyOeT0clKFb/9Pt4LnW/g1VaSQRDPbhnqCamsp/\nv5RwSAY1brKZndJ/c6deBkv0lv2Al8YPO9QHKzY2Z6xPQDysLDFHlMa0k8I7jZkU\naKTkDPThgpW/hNKGd6iiWtOQ146xsC38nAO2A/jipP7ZVCgxp30HaRsuOgSOMxpU\ndCgz8iiTAgMBAAECggEAMyqUv2mlPiO0Cwn+2JsFEy2P8dchwwHgb+FKru+TepsT\nu8SQxp3SUhzu7GG8fWHYibcy5/aMuC9pb36OgculwJrUee900318GBMEPgW6FR6R\nnI5cHhEss8wd4ogmMkrt1CZhv18L6x4fgHBbUaXT9RgUYn1BQLODgnQaLNe1S3pE\nqlkuRPQOqqk96H1fkoSk2roruySXJvtoDkUFdPGFwBC3c5+8OPR555EAtRZAdcad\n2x+RjrBzLbTL7ZQdoXjBGoiuH/A0FXKYp5CBXWqJBvtyoDDV6rUrE9T441T+cJAz\nc/3FizNrc4s/6SLdgt3SkmlUnsfZ/Gt0Sc0C+foXqQKBgQD/AGymGWN2VJctLIDB\n5Tzk4ZPQRtB34tDxNedyUfKGEuasNSTuJViCGNaNjXpluXVNtztDoQoj1Y0cTrbz\nG0N0c9H5sJhAnofk4NlucRf/XJXZ8D/m4PEWr+3o2URcCjUDBlV8TFtYhJemu1JQ\nU16ZkOwWTie9faXw6YeYuxhgLwKBgQDyT2vyFNCX7IpFfTqIvsuAJo3ouPtowg9o\nhiJvJGDWp7W2mWiDfUT595NaRXJ4C/5EyoPuyxtFFdb538XoJwqhwNdCtoLeB2eV\ndoS/aj4yR8uN/U8EKYzgPcoDT5ieli9qfkJfhQ9EycLDe0lYQECkzutNJGNp+luU\n0kEt4qLg3QKBgCFZcdwrN9nS6E6NIm25SER6x/UWPc9gB4l0TkeiNgCY2jgtXx4S\nHkgtbWnn1dkV7yRx0TtgBU54cFMCbGTQ7Pp+5zgrg034LbeePHF4MvY/qo1tIT5V\nrtITwh6Qw6Lx5sr1ehqeddfWx2qT1wkQTi/xRx8Hq6TZmOkv8X7lASQbAoGADQ4g\nZ8OdVYImFQZhP/rfpgDtxmspCITkJaKMemaAXTBeBv+O9P6r17fyFXwGZddnlLdA\nkn4Y8wjxMzdOR2rLFNn5/xssQ+AsQY5IKrQDs9vQaM8MEdJXR8Gsf68rLugyl89D\nrjfSEce5GaUr13hmwzpuzRI31P7rLkKBxoIeenUCgYEApO3tCAhxMdUglJywad5i\nq4Mr4Sm2RbntvYG4rZb6Ntei5nO5DVh3o9ZIUPE1PPEPg7039eK7mRabHfARSIoK\ng/HfMgxiiPal5T1y3Dj+u4xordb5/CmbgTYKtJ28aSJEXtlz8AnTk3r5lhUOo7Oh\nLEE3bYnxifRluNaJenJr+AU=\n-----END PRIVATE KEY-----\n"
  },
  databaseURL: "https://group9-node.firebaseio.com"
});

var db = firebase.database();

var ref = db.ref("temps");

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
	/*
	particle.getEventStream({ auth: token}).then(function(stream) {
	  stream.on('f_temperature', function(data) {
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
		let name = listDev[i].name;
		particle.getVariable({ deviceId: id, name: 'temperature', auth: myToken }).then(function(data) {
		  //console.log('Device variable retrieved successfully from id:',id,'with data:',data);
		  sendTemp(name,data);
		}, function(err) {
		  console.log('An error occurred while getting attrs from device with id:',id);
		  if(name in temp_dict) {
			  delete temp_dict[name];
	      	  console.log("Removing ", name);
	      }
		});
	}
}

// Temperature Dictionary for Sensor Data
var temp_dict = {};

function sendTemp(name,data){
	temp_dict[name] = data.body.result;
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
    average = average.toFixed(2);

    var stringDate = new Date().toISOString();

  	var eventRef = ref.push({
		device: "average",
		time: stringDate,
		value: average
  	});
  } 
}
