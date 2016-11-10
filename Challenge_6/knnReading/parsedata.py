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
                if regionName not in allAccessPoints:
                    allAccessPoints[regionName] = {}
                    allAccessPoints[regionName][count] = [(bssid, signal)]
                else:
                    if count not in allAccessPoints[regionName]:
                        allAccessPoints[regionName][count] = [(bssid, signal)]
                    else:
                        allAccessPoints[regionName][count].append((bssid, signal))
            else:
                count += 1




print allAccessPoints

# for key in allAccessPoints:
#     print key,
#     for signals in allAccessPoints[key]:
#         print signals,
#     print


