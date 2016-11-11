import math
import random
import operator
import csv
import glob, os
from collections import defaultdict
from parsedata import parseData

maxSet = parseData()
trainingSet = dict(maxSet)
pruneSet = dict(trainingSet)

for x in maxSet:
	if random.randrange(1,1000) > 200:
		del pruneSet[x]
	else:
		del trainingSet[x]

print len(pruneSet)
print len(trainingSet)
print len(maxSet)

print all(item in list(maxSet.keys()) for item in list(pruneSet.keys()))
print all(item in list(maxSet.keys()) for item in list(trainingSet.keys()))

#Have to change so accepts tuple of tuples
def euclideanDistance(instance1, instance2, length):
	distance = 0
	for x in instance1:
		for y in instance2:
			if(x[0] == y[0]):
				distance += pow((int(x[1]) - int(y[1])), 2)
				#print x[0]
	return math.sqrt(distance)

#Example test instance [(12314124,-75),(4124324,-25)]
#Return value is the keys of the k nearest neighbors- iterate and grab labels
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


def testAccuracy():
	sizeTrainingSet = len(trainingSet)
	count = 0;
	for tester in pruneSet: 
		most = getResponse(getNeighbors(tester,5))
		print most,
		print ":",
		print maxSet[tester]
		#print type(most)
		#print type(str(maxSet[tester]))
		if(most == str(maxSet[tester])):
	 		count+=1 
	print float(count)/sizeTrainingSet
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

testAccuracy()