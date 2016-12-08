import math
import operator
import threading
from parsedata import parseData
from reqRSS import getPhotonWifi

trainingSet, fullSet = parseData()
pruneSet = dict(fullSet)
print("Parsed Data Correctly")

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
def getPrediction(sample):
    #sample = getPhotonWifi()
    #for tup in sample:
    #    print tup
    prediction = predict(sample,7)
    # L.append(prediction)
    # counter += 1
    # if(counter == 5):
    #     print prediction
    #     counter = 0
    return int(prediction[14:])
    #threading.Timer(1,myTimer).start()

# Have to change so accepts tuple of tuples
def euclideanDistance(instance1, instance2, length):
    distance = 0
    count = 0;
    n = len(instance1) / 2
    for x in instance1:
        myCount = 0;
        for y in instance2:
            if (x[0] == y[0]):
                count += 1
                myCount+=1
                distance += pow((int(x[1]) - int(y[1])), 2)
        if(myCount==0):
            distance+=100
                # print x[0]
    #if (count < n):
    #    distance += 200
    # print distance
    return math.sqrt(distance)


# Example test instance [(12314124,-75),(4124324,-25),.....]
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
        #print distances[x]
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
    #print sortedVotes
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
