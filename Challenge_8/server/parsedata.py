import csv
import glob, os


def parseData():
    allAccessPoints = {}
    rawSample = {}

    for file in glob.glob("regions/*.txt"):
        os.system("cat " + file + " | grep -v '\(G\|g\)roup' > " + file + "_cpy.txt")
        os.system("mv " + file + "_cpy.txt " + file)
        os.system("cat " + file + " | grep -v '^,' > " + file + "_cpy.txt")
        os.system("mv " + file + "_cpy.txt " + file)
        with open(file, 'rb') as csvfile:
            cvsreader = csv.reader(csvfile, delimiter=',')
            count = 1
            accessPoint={}
            sampleList = {}
            for row in cvsreader:
                try:
                    if row:
                        regionName = file[:-7]
                        bssid = row[1]
                        signal = row[2]

                        if count not in sampleList:
                            sampleList[count] = [(bssid, signal)]
                        else:
                            sampleList[count].append((bssid, signal))


                        if (bssid not in accessPoint):
                            accessPoint[bssid] = [signal]
                        else:
                            accessPoint[bssid].append(signal)
                    else:
                        count += 1
                except:
                    pass

            for sample in sampleList:
                rawSample[tuple(sampleList[sample])] = file[:-7]



        for bssidkey in accessPoint:
            # Take the average
            signals = accessPoint[bssidkey]
            averageSignal = sum([int(x) for x in signals]) / len(signals)
            # accessPoint[bssidkey] = averageSignal

            regionNameKey = regionName + " " + file[-7:-4]
            if (regionNameKey) not in allAccessPoints:
                allAccessPoints[regionNameKey] = [(bssidkey, averageSignal)]
            else:
                allAccessPoints[regionNameKey].append((bssidkey, averageSignal))






    # # BSSID not inside the dictionary
    # if (regionName + " " + str(count)) not in allAccessPoints:
    #     # allAccessPoints[regionName] = {}
    #     allAccessPoints[regionName + " " + str(count)] = [(bssid, signal)]
    # else:
    #     if (regionName + " " + str(count)) not in allAccessPoints:
    #         allAccessPoints[regionName + " " + str(count)] = [(bssid, signal)]
    #     else:
    #         allAccessPoints[regionName + " " + str(count)].append((bssid, signal))



    regionAccessPoints = {}

    for key in allAccessPoints:
        rssidlist = tuple(allAccessPoints[key])
        regionName = key
        regionAccessPoints[rssidlist] = regionName.split(' ')[0]

    # print allAccessPoints
    # counter = 0
    # for key in regionAccessPoints:
    #     counter += 1
    #     print regionAccessPoints[key] + ': ',
    #     print str(key)
    return regionAccessPoints, rawSample

#print len(parseData())


#print globalCounter

# print regionAccessPoints

# for key in allAccessPoints:
#     print key,
#     for signals in allAccessPoints[key]:
#         print signals,
#     print


# for file in glob.glob("*.txt"):
#     with open(file, 'rb') as csvfile:
#         cvsreader = csv.reader(csvfile, delimiter=',')
#         count = 1
#         for row in cvsreader:
#             if row:
#                 regionName = file[:-4]
#                 bssid = row[1]
#                 signal = row[2]
#                 # BSSID not inside the dictionary
#                 if regionName not in allAccessPoints:
#                     allAccessPoints[regionName] = {}
#                     globalCounter += 1
#                     allAccessPoints[regionName][count] = [(bssid, signal)]
#                 else:
#                     if count not in allAccessPoints[regionName]:
#                         allAccessPoints[regionName][count] = [(bssid, signal)]
#                         globalCounter += 1
#                     else:
#                         allAccessPoints[regionName][count].append((bssid, signal))
#             else:
#                 count += 1


