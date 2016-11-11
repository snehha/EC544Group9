import requests
import json


# host = 'https://api.particle.io/v1/devices/'


def getPhotonWifi():
    result = ''
    while not result:
        payload = {'Authorization': 'Bearer 7956c0012c6ed8f50b755a89f93ee1c68aec447c'}
        try:
            r = requests.get('https://api.particle.io/v1/devices/42003c001447353236343033/wifiData', headers=payload)
            r = r.json()
            result = r['result'].encode('ascii')
        except:
            pass

    tempList = []
    for value in result.strip().split('\n'):
        scan = value.split(',')
        tempList.append((scan[1], scan[2]))
    return tuple(tempList)
