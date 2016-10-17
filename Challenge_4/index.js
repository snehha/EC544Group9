var Particle = require('particle-api-js');
var particle = new Particle();

var uName = 'phosho9@gmail.com';
var pWord = 'ilikepie9';

var token = particle.login({username:uName,password:pWord}).then(
//Success
function(data){
	console.log("API call completed on promise resolve:",data.body.access_token);

	//Get list of devices once logged in
	var token = data.body.access_token;// from result of particle.login
	var devicesPr = particle.listDevices({ auth: token });
	devicesPr.then(
	  function(devices){
	    console.log('Devices: ', devices.body);

	    //Flash firware once I get list of all devices on
	    for(var i = 0; i < devices.body.length; i++){
	    	var id = devices.body[i].id;
	    	console.log(devices.body[i].id);
			particle.flashDevice({ deviceId: id, files: { file1: './router.router.ino' }, auth: token }).then(function(data) {
			  console.log('Device flashing started successfully:', data);
			}, function(err) {
			  console.log('An error occurred while flashing the device:', err);
			});
		}

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

//Setup listeners