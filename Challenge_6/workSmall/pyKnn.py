import math
import operator
import threading
from parsedata import parseData
from reqRSS import getPhotonWifi

trainingSet, fullSet = parseData()
pruneSet = dict(fullSet)

# for x in fullSet:
#     if random.randrange(1, 1000) > 100:
#         del pruneSet[x]
#
# print len(pruneSet)
# print len(fullSet)
# print len(trainingSet)


# print all(item in list(maxSet.keys()) for item in list(pruneSet.keys()))
# print all(item in list(maxSet.keys()) for item in list(trainingSet.keys()))

counter = 0
L = []
def myTimer():
    sample = getPhotonWifi()
    prediction = predict(sample,5)
    # L.append(prediction)
    # counter += 1
    # if(counter == 5):
    #     print prediction
    #     counter = 0
    print prediction
    threading.Timer(.5,myTimer).start()

# Have to change so accepts tuple of tuples
def euclideanDistance(instance1, instance2, length):
    distance = 0
    count = 0;
    n = len(instance1) /3
    for x in instance1:
        for y in instance2:
            if (x[0] == y[0]):
                count += 1
                distance += pow((int(x[1]) - int(y[1])), 2)
                # print x[0]
    if (count < n):
        distance = 0
    # print distance
    return math.sqrt(distance)


# Example test instance [(12314124,-75),(4124324,-25)]
# Return value is the keys of the k nearest neighbors- iterate and grab labels
def getNeighbors(testInstance, k):
    distances = []
    length = len(testInstance)
    for sample in trainingSet:
        dist = euclideanDistance(testInstance, sample, length)
        distances.append((sample, dist))
    distances.sort(key=operator.itemgetter(1))
    neighbors = []
    for x in range(k):
        neighbors.append(distances[x][0])
    # print neighbors
    return neighbors


def getResponse(neighbors):
    classVotes = {}
    for x in neighbors:
        response = trainingSet[x]
        if response in classVotes:
            classVotes[response] += 1
        else:
            classVotes[response] = 1
    sortedVotes = sorted(classVotes.iteritems(), key=operator.itemgetter(1), reverse=True)
    # print sortedVotes
    return sortedVotes[0][0]

def predict(sample,k):
    return getResponse(getNeighbors(sample, k))

# def testAccuracy():
#     sizePruneSet = len(pruneSet)
#     count = 0;
#     for tester in pruneSet:
#         most = getResponse(getNeighbors(tester, 5))
#         print most,
#         print ":",
#         print fullSet[tester]
#         if (most == str(fullSet[tester])):
#             count += 1
#     print float(count) / sizePruneSet


# ok = []
# for tester in pruneSet:
# 	ok = getNeighbors(tester,5)
# 	break
#  	#if(getResponse(getNeighbors(tester,5)) == pruneSet[tester]):
#  		#count+=1
# #for x in ok:
# 	#print x
# no = getResponse(ok)
# #print no
# #print count/sizeTrainingSet

myTimer()
