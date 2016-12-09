from flask import Flask
from flask_socketio import SocketIO
import ast

from flask import send_file, request, Response, render_template
from flask_socketio import send, emit
import threading
from pyKnn import *
from reqRSS import *
import os

import serial

# Compass
import time
from Adafruit_HMC6352 import HMC6352

currentLoc = ''
knnRefresh = 2.0
# clockwise = true
# corners = [15, 24, 27, 36]
# elevatorRegions = [53, 54]
# previousLoc = 0;

# Initialise the HMC6352
hmc = HMC6352()
hmc.userCalibration()

#usbPort = os.system('ls /dev/ttyACM*')
try :
    photonSerial = serial.Serial(
        port='/dev/ttyACM0',
        baudrate=9600,
        parity=serial.PARITY_ODD,
        stopbits=serial.STOPBITS_TWO,
        bytesize=serial.EIGHTBITS
    )
except:
    photonSerial = serial.Serial(
        port='/dev/ttyACM1',
        baudrate=9600,
        parity=serial.PARITY_ODD,
        stopbits=serial.STOPBITS_TWO,
        bytesize=serial.EIGHTBITS
    )
    
if photonSerial.isOpen():
    photonSerial.close()
photonSerial.open()
photonSerial.isOpen()

server = Flask(__name__)
socketio = SocketIO(server)

@server.route('/')
@server.route('/index')
def index():
    return render_template('location.html')


@server.route('/static/fourthfloor.png')
def image():
    return send_file('static//img//fourthfloor.png', mimetype='image/png')

@server.route('/static/uparrow.png')
def upArrowImage():
    return send_file('static//img//uparrow.png', mimetype='image/png')

@server.route('/static/downarrow.png')
def downArrowImage():
    return send_file('static//img//downarrow.png', mimetype='image/png')

@server.route('/static/leftarrow.png')
def leftArrowImage():
    return send_file('static//img//leftarrow.png', mimetype='image/png')

@server.route('/static/rightarrow.png')
def rightArrowImage():
    return send_file('static//img//rightarrow.png', mimetype='image/png')

@server.route('/static/gobutton.png')
def goButtonImage():
    return send_file('static//img//gobutton.png', mimetype='image/png')

@server.route('/static/stopbutton.png')
def stopButtonImage():
    return send_file('static//img//stopbutton.png', mimetype='image/png')

@server.route('/ssid', methods=['GET','POST'])
def getSSIDs():
    global currentLoc
    global previousLoc
    #Logic to get ssid from photon via webhook
    dataReceivedFromUpdate = request.data
    dataReceivedFromUpdate = tuple([tuple(group.split(',')) for group in ast.literal_eval(dataReceivedFromUpdate)])
    print dataReceivedFromUpdate
    

    # print("*****************: " + dataReceivedFromUpdate)
    if dataReceivedFromUpdate:
        # PHOTON --> LOCAL ClOUD -> WEBHOOK -> THIS FUNCTION
        print("Got SSIDs from Photon")
        if (dataReceivedFromUpdate):
            newLoc = getPrediction(dataReceivedFromUpdate)
            if (currentLoc is newLoc):
                return 'acknowledge but same'
            else:
                if (currentLoc != '') :         # set previous location if there is one
                    previousLoc = int(currentLoc)
                currentLoc = newLoc
                sendMessage()   # sends predicted location to website
                predictTurn()   # calculates whether car needs to turn based on location
                #sendPrediction(currentLoc)
                return 'status 200'
        else:
            return 'error 500'
    else:
        return 'error 500'
    return 'error 500'

def getCompassReading():
    try:
        heading = hmc.readData()
        print "Compass Heading: %.1f" % heading
        time.sleep(0.6)
        return heading
    except:
        getCompassReading()

north = 45
east = 135
south = 225
west = 315
compassThreshold = 10
turn = False
turnThreshold = 70

previousHeading = getCompassReading()

def predictTurn():
    global turn
    global previousHeading
    current = int(currentLoc)

    # get compass reading to determine if turn is needed in starting region
    heading = getCompassReading

    # check if turn has been completed
    if (turn) :
        if ( abs(heading - previousHeading) >  turnThreshold) :    # turn has been completed
            photonSerial.write("stopTurn")
            turn = False

    if ( current == 16 ) : 
        if (heading >= east and heading <= (east + compassThreshold)) :
            photonSerial.write("knnRight")
            turn = True
            previousHeading = heading
    elif ( current == 23 ) : 
        if (heading >= west and heading <= (west + compassThreshold)) :
            photonSerial.write("knnLeft")
            turn = True
            previousHeading = heading
    elif ( current == 25 ) :
        if (heading >= north and heading <= (north + compassThreshold)) :
            photonSerial.write("knnRight")
            turn = True
            previousHeading = heading
    elif ( current == 26 ) :
        if (heading >= south and heading <= (south + compassThreshold)) :
            photonSerial.write("knnLeft")
            turn = True
            previousHeading = heading
    elif ( current == 28 ) : 
        if (heading >= west and heading <= (west + compassThreshold)) :
            photonSerial.write("knnRight")
            turn = True
            previousHeading = heading
    elif ( current == 35 ) :
        if (heading >= east and heading <= (east + compassThreshold)) :
            photonSerial.write("knnLeft")
            turn = True
            previousHeading = heading
    elif ( current == 53 ) :
        if (heading >= south and heading <= (south + compassThreshold)) :
            photonSerial.write("knnRight")
            turn = True
            previousHeading = heading
    elif ( current == 54 ) :
        if (heading >= north and heading <= (north + compassThreshold)) :
            photonSerial.write("knnLeft")
            turn = True
            previousHeading = heading

#@socketio.on('refresh')
def sendMessage():
    # global region
    emit('location_event', currentLoc)
    # emit('location_event', region);

@socketio.on('moveCar')
def moveCar(command):
    print('Sending Command: ')
    print (command)
    photonSerial.write(command)
    # sendCommand(command)

# region = 27
# bottomRightRegion = 15;
# limit = 37


# def testCoordinates():
#     global region
#     region = region + 1
#     if region is limit:
#         region = 53
#     if region is 55:
#         region = bottomRightRegion





# testCoordinates()

# def sendKnn():
#     try:
#         threading.Timer(knnRefresh,sendKnn).start()
#     except(KeyboardInterrupt):
#         thread.__stop()

#     if(currentLoc):
#         sendPrediction(currentLoc)
#     print("Sending Prediction ",currentLoc)

# sendKnn()

if __name__ == '__main__':
    print("Starting socketIO app")
    socketio.run(server, debug=True, host='0.0.0.0')
