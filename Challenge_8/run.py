from flask import Flask
from flask_socketio import SocketIO

server = Flask(__name__)
socketio = SocketIO(server)

from server import views

if __name__ == '__main__':
    print("Starting socketIO app")
    socketio.run(server, debug=True)
    
 	#print("starting knn timer")
	# sendKnn()
    
    print("starting App")
    #app.run(host='0.0.0.0')
