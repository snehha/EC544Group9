# from app import app
from flask import Flask, send_file, request, Response, render_template
from flask_socketio import SocketIO, send, emit
from pyKnn import *

app = Flask(__name__)


@app.route('/')
@app.route('/index')
def index():
    return render_template('location.html')


@app.route('/static/fourthfloor.png')
def image():
    return send_file('static//img//fourthfloor.png', mimetype='image/png')


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
    # global region
    # print ('got refresh')
    emit('location_event', getPrediction())


# testCoordinates()

if __name__ == '__main__':
    socketio.run(app)

app.run(host="0.0.0.0", debug=True)
