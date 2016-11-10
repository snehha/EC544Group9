import csv
import glob, os



allAccessPoints = {}

for file in glob.glob("*.txt"):
    with open(file, 'rb') as csvfile:
        cvsreader = csv.reader(csvfile, delimiter=',')
        count = 1
        for row in cvsreader:
            if row:
                regionName = file[:-4]
                bssid = row[1]
                signal = row[2]
                # BSSID not inside the dictionary
                if (str(count) + regionName) not in allAccessPoints:
                    #allAccessPoints[regionName] = {}
                    allAccessPoints[str(count) + regionName] = [(bssid, signal)]
                else:
                    if (str(count) + regionName) not in allAccessPoints:
                        allAccessPoints[str(count) + regionName] = [(bssid, signal)]
                    else:
                        allAccessPoints[str(count) + regionName].append((bssid, signal))
            else:
                count += 1

regionAccessPoints = {}

for key in allAccessPoints:
    rssidlist = tuple(allAccessPoints[key])
    regionName = key
    regionAccessPoints[rssidlist] = regionName

# print allAccessPoints
counter = 0
for key in allAccessPoints:
    counter += 1
    print str(key) + ": ",
    print allAccessPoints[key]

print counter





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


