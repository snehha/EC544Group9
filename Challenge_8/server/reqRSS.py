import requests
import json


# host = 'https://api.particle.io/v1/devices/'


def getPhotonWifi():
    result = ''
    while not result:
        payload = {'Authorization': 'Bearer 6cd8c58e6cad516b135d7dd70762d897a7f6a04d'}
        try:
            r = requests.get('http://192.168.1.150:8080/v1/devices/290041001947353236343033/wifiData', headers=payload)
            r = r.json()
            result = r['result'].encode('ascii')
        except:
            pass

    tempList = []
    for value in result.strip().split('\n'):
        scan = value.split(',')
        tempList.append((scan[1], scan[2]))
    return tuple(tempList)

def sendPrediction(prediction):
    result = ''
    while not result:
        payload = {'args':str(prediction),'access_token':'6cd8c58e6cad516b135d7dd70762d897a7f6a04d'}
        try:
            r = requests.post('http://192.168.1.150:8080/v1/devices/290041001947353236343033/corner', data=payload)
        except:
            pass
    print(r.status_code)

def sendCommand(command):
    result = ''
    while not result:
        payload = {'args':command,'access_token':'6cd8c58e6cad516b135d7dd70762d897a7f6a04d'}
        try:
            r = requests.post('http://192.168.1.150:8080/v1/devices/290041001947353236343033/commandCar', data=payload)
        except:
            pass
    print(r.status_code)