  #!/usr/bin/env python

import socket
import json
import requests
import urllib

TCP_IP = '192.168.1.150'
TCP_PORT = 8080
BUFFER_SIZE = 1024
MESSAGE = "GET /v1/events?access_token=6cd8c58e6cad516b135d7dd70762d897a7f6a04d HTTP/1.0\n\n"

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((TCP_IP, TCP_PORT))
s.send(MESSAGE)
s.setblocking(1)
running = True


# TODO chnge ip addfess 
def sendSSIDData(command):
    headers = {'content-type': 'text/html'}
    payload = str(command)

    
    try:
        print "INSIDE: ", payload
        r = requests.post('http://192.168.1.120:5000/ssid', data=payload)
    except:
        print("not ok")
    print(r.status_code)


while running:
    try:
        print "Attempting to read from socket..."
        while True:
            data = s.recv(BUFFER_SIZE)
            if len(data) == 0:      # remote end closed
                #print "Remote end closed"
                running = False
                break
            try:
                data = data.strip().split('\n')
                for i in data:
                    if (i[:4] != 'data'):
                        pass
                    else:
                        result = json.loads(i[6:])['data'].encode('ascii')
                        
                        #[(12314124,-75),(4124324,-25)]
                        tempList = []
                        for value in result.strip().split('\n'):
                            
                            scan = value.split(',')
                            try:
                                tempList.append(','.join((scan[1], scan[2])))
                            except:
                                pass
                        #print str(tempList)
                        sendSSIDData(tempList)
                        #print(" <---- sent to webserver ---- ")
                        
            except:
                pass
            
    except socket.error, e:
        if e[0] != 11:      # Resource temporarily unavailable
            print e

    # perform other program tasks
    #print "Sleeping..."
    time.sleep(1)



# data = s.recv(BUFFER_SIZE)
s.close()

# print "received data:", data