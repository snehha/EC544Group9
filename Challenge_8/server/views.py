from flask import Flask
from flask_socketio import SocketIO
import ast

from flask import send_file, request, Response, render_template
from flask_socketio import send, emit
import threading
from pyKnn import *
from reqRSS import *

currentLoc = ''
knnRefresh = 2.0


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
    #Logic to get ssid from photon via webhook
    dataReceivedFromUpdate = request.data
    dataReceivedFromUpdate = tuple([tuple(group.split(',')) for group in ast.literal_eval(dataReceivedFromUpdate)])
    print dataReceivedFromUpdate
    

    # print("*****************: " + dataReceivedFromUpdate)
    if dataReceivedFromUpdate:
        # PHTON --> LOCAL ClOUD -> WEBHOOK -> THIS FUNCTION
        print("Got SSIDs from Photon")
        if (dataReceivedFromUpdate):
            newLoc = getPrediction(dataReceivedFromUpdate)
            if (currentLoc is newLoc):
                return 'acknowledge but same'
            else:
                currentLoc = newLoc
                sendPrediction(currentLoc)
                return 'status 200'
        else:
            return 'error 500'
    else:
        return 'error 500'
    return 'error 500'

def send_location(location):
    socketio.emit('location_event', location)
    index()

@socketio.on('refresh')
def sendMessage():
    # global region
    emit('location_event', currentLoc)
    # emit('location_event', region);

@socketio.on('moveCar')
def moveCar(command):
    print('Sending Command: ')
    print (command)
    sendCommand(command)

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
