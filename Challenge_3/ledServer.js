var app     =     require("express")();
var SerialPort = require("serialport");
var http    =     require('http').Server(app);
var io      =     require("socket.io")(http);

app.get('/', function(req, res){
  res.sendFile(__dirname + '/webview/test.html');
});

app.get('/1k.js', function(req, res){
  res.sendFile(__dirname + '/webview/1k.js');
});

http.listen(3000, function(){
  console.log('listening on *:3000');
});

var portName = process.argv[2],
portConfig = {
	baudRate: 9600,
	parser: SerialPort.parsers.byteLength(4)
};
var sp = new SerialPort(portName, portConfig);
var ledNum = 3;
var ledMap = {}; // ledID : [R,G,B]
//Create Buffer object to pass to pass to sp.write
var buffer = Buffer.allocUnsafe(4);

for( var i = 1; i <= ledNum; i++) {
	var led = [0,0,0]; // [R, G, B]
	ledMap['bulb-' + i] = led;
}

//When browser loads
io.on('connection', function(socket){
  console.log('a user connected');
  socket.emit('led status',ledMap);

  //Coordinator Sends all ones to tell arduino to send status
  //This should not need to be done
  /*buffer.writeInt32BE(-1, 0);
  sp.write(buffer);*/

  // when website connects, send status of LEDs
  //console.log("Sending LED status to website: ");
  //console.log(ledMap);

  socket.on('disconnect', function(){
  });

  // received command from website to toggle led and color
  socket.on('toggle led', function(id,R,G,B){
  	console.log("Website command: " + id + " " + R + " " + G + " " + B);
  	// send led num and RGB to arduino
    //Send string to signal start of text
    //Send values
    buffer.writeUInt8(id,0);
    buffer.writeUInt8(R,1);
    buffer.writeUInt8(G,2);
    buffer.writeUInt8(B,3);
  	console.log("Sending command ID,R,G,B: " + buffer.toString('hex'));
    //clearInterval(poller);
  	sp.write(buffer);
  });

});

var n = 5;
var poller;
var response;
function noResponse(){
  io.emit('disconnect site');
}
//Write 32 1s to Arduino every n seconds
function pollFunc(){
  buffer.writeInt32BE(-1,0);
  console.log("Sending:",buffer.toString('hex'));
  console.log("--------Polling-----------");
  sp.write(buffer);
}

//Every time coordinator starts, initializs timer to n seconds
sp.on("open", function () {
  console.log('open');
  poller = setInterval(pollFunc,1000*n);
  response = setInterval(noResponse,1000*2*n);
  
  // receives status of LEDs
  sp.on('data', function(data) {
    console.log('data received: ' + data.toString('hex'));
    clearInterval(response);
    response = setInterval(noResponse,1000*2*n);

    //If arduino sent all ones, it wants the current value of the lights so send 3 rows of 4 bytes
    /*if(data.readInt32BE() == -1){
      console.log("Arduino Wanting initial Value in ledMap");

      //Send synchronization String of all zeroes
      buffer.writeInt32BE(0,0);
      console.log("Sending:",buffer.toString('hex'));
      sp.write(buffer);
      var myBuff = Buffer.allocUnsafe(12);

      //Send Led values on client web
      for(var i = 1; i <= ledNum; i++){
        myBuff.writeUInt8(i,0+(i-1)*4);
        myBuff.writeUInt8(ledMap['bulb-'+i][0],1+(i-1)*4);
        myBuff.writeUInt8(ledMap['bulb-'+i][1],2+(i-1)*4);
        myBuff.writeUInt8(ledMap['bulb-'+i][2],3+(i-1)*4);
      }
      console.log("Sending:",myBuff.toString('hex'));
      sp.write(myBuff);
    }
    //Sending it's current Temp
    //Will not need, only for debugging and updating ledMap
    else{*/
    //poller = setInterval(pollFunc,1000*n);
    var id = data.readUInt8(0);
    console.log("ID: " + id);
    var R = data.readUInt8(1);
    console.log("Red: " + R);
    var G = data.readUInt8(2);
    console.log("Green: " + G);
    var B = data.readUInt8(3);
    console.log("Blue: " + B);
    if( id > 0  && id <= ledNum) {
    	ledMap["bulb-"+id] = [R,G,B];
    	console.log(ledMap);
    }
    io.emit('led status', ledMap); // ledID : [bulb,R,G,B]
  });
});
