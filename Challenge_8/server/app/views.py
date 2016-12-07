# from app import app
from flask import Flask, send_file, request, Response, render_template
from flask_socketio import SocketIO, send, emit
from camera import Camera
#from pyKnn import *

app = Flask(__name__)


@app.route('/')
@app.route('/index')
def index():
    return render_template('location.html')


@app.route('/static/fourthfloor.png')
def image():
    return send_file('static//img//fourthfloor.png', mimetype='image/png')

@app.route('/static/uparrow.png')
def upArrowImage():
    return send_file('static//img//uparrow.png', mimetype='image/png')

@app.route('/static/downarrow.png')
def downArrowImage():
    return send_file('static//img//downarrow.png', mimetype='image/png')

@app.route('/static/leftarrow.png')
def leftArrowImage():
    return send_file('static//img//leftarrow.png', mimetype='image/png')

@app.route('/static/rightarrow.png')
def rightArrowImage():
    return send_file('static//img//rightarrow.png', mimetype='image/png')

@app.route('/static/gobutton.png')
def goButtonImage():
    return send_file('static//img//gobutton.png', mimetype='image/png')

@app.route('/static/stopbutton.png')
def stopButtonImage():
    return send_file('static//img//stopbutton.png', mimetype='image/png')

def gen(camera):
    while True:
        frame = camera.get_frame()
        yield (b'--frame\r\n'
               b'Content-Type: image/jpeg\r\n\r\n' + frame + b'\r\n')

@app.route('/video_feed')
def video_feed():
    return Response(gen(Camera()),
                    mimetype='multipart/x-mixed-replace; boundary=frame')


socketio = SocketIO(app)


def send_location(location):
    socketio.emit('location_event', location)
    index()


region = 27
bottomRightRegion = 15;
limit = 37


def testCoordinates():
    global region
    region = region + 1
    if region is limit:
        region = 53
    if region is 55:
        region = bottomRightRegion


@socketio.on('refresh')
def sendMessage():
    global region
    # print ('got refresh')
    emit('location_event', getPrediction())
    #emit('location_event', region);

@socketio.on('moveCar')
def moveCar(command):
    print('moving car: ')
    print (command)

@socketio.on('startCar')
def moveCar(command):
    print ("start car: ")
    print(command)

@socketio.on('autopilot')
def autopilotToggle(command):
    print ("Autopilot: ")
    print(command)


# testCoordinates()

if __name__ == '__main__':
    socketio.run(app)
    app.run(host='0.0.0.0')

#app.run(host='0.0.0.0', debug=True)
