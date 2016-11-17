import csv
import glob, os


def parseData():
    allAccessPoints = {}

    for file in glob.glob("*.txt"):
        with open(file, 'rb') as csvfile:
            cvsreader = csv.reader(csvfile, delimiter=',')
            count = 1
            for row in cvsreader:
                if row:
                    regionName = file[:-4]
                    bssid = row[1]
                    if bssid is '03341138252243':
                        pass
                    signal = row[2]
                    # BSSID not inside the dictionary
                    if (regionName + " " + str(count)) not in allAccessPoints:
                        #allAccessPoints[regionName] = {}
                        allAccessPoints[regionName + " " + str(count)] = [(bssid, signal)]
                    else:
                        if (regionName + " " + str(count)) not in allAccessPoints:
                            allAccessPoints[regionName + " " + str(count)] = [(bssid, signal)]
                        else:
                            allAccessPoints[regionName + " " + str(count)].append((bssid, signal))
                else:
                    count += 1

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
    return regionAccessPoints



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


