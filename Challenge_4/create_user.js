var Particle = require('particle-api-js');
var particle = new Particle();
var uName = 'phosho9@gmail.com';
var pWord = 'ilikepie9';
particle.createUser({ username: 'example@email.com', password: 'pass' }.then(function(data) {
	var loginPromise = particle.login('example@email.com', 'pass');
	loginPromise.then(
      function(data) {
        console.log('Login successful! access_token:', data.access_token);
      },
      function(err) {
        console.log('Login failed:', err);
      }
    );
  }
});