import json
import numpy as np

# def distance(x1, y1, x2, y2, radius):
#     L = np.sqrt( (x1-x2)**2 + (y1-y2)**2)
#     alpha = np.arcsin(  L/(2*radius)  )
#     print(alpha)
#     # return 2*np.pi()*alpha/180
#     print(2*alpha*radius)

# distance(-53,-11,-21.0,-2.42563, 64)


laneDirections = [["WER_0", "WER_1", "WER_2", "WER_3", "WER_4", "WER_5", "WER_6", "WER_7"], \
                    ["WSR_0", "WSR_1"], \
                    ["WEL_0", "WEL_1", "WEL_2", "WEL_3", "WEL_4", "WEL_5", "WEL_6", "WEL_7"], \
                    ["WNL_0", "WNL_1", "WNL_2", "WNL_3", "WNL_4", "WNL_5", "WNL_6", "WNL_7"], \
                    ["EWR_0", "EWR_1", "EWR_2", "EWR_3", "EWR_4", "EWR_5", "EWR_6", "EWR_7"], \
                    ["ENR_0", "ENR_1"], \
                    ["EWL_0", "EWL_1", "EWL_2", "EWL_3", "EWL_4", "EWL_5", "EWL_6", "EWL_7"], \
                    ["ESL_0", "ESL_1", "ESL_2", "ESL_3", "ESL_4", "ESL_5", "ESL_6", "ESL_7"], \
                    ["NSR_0", "NSR_1", "NSR_2", "NSR_3", "NSR_4", "NSR_5", "NSR_6", "NSR_7"], \
                    ["NWR_0", "NWR_1"], \
                    ["NSL_0", "NSL_1", "NSL_2", "NSL_3", "NSL_4", "NSL_5", "NSL_6", "NSL_7"], \
                    ["NEL_0", "NEL_1", "NEL_2", "NEL_3", "NEL_4", "NEL_5", "NEL_6", "NEL_7"], \
                    ["SNR_0", "SNR_1", "SNR_2", "SNR_3", "SNR_4", "SNR_5", "SNR_6", "SNR_7"], \
                    ["SER_0", "SER_1"], \
                    ["SNL_0", "SNL_1", "SNL_2", "SNL_3", "SNL_4", "SNL_5", "SNL_6", "SNL_7"], \
                    ["SWL_0", "SWL_1", "SWL_2", "SWL_3", "SWL_4", "SWL_5", "SWL_6", "SWL_7"]  \
                ]

with open('intro_graph_4.json') as f:
    graph = json.load(f)
    vertices = graph['vertices']
    edges = graph['edges']
# print(edges[0])

pairDistance = {}
for lane in laneDirections:
    for point in lane:
        if (point not in pairDistance.keys()):
            pairDistance[point] = {}
        for p in lane:
            pairDistance[point][p] = 0.0


    for point in lane:
        for p in pairDistance[point].keys():
            laneName = point.split("_")[0]
            pointIdx1 = point.split("_")[1]
            pointIdx2 = p.split("_")[1]
            lowIdx = min(int(pointIdx1), int(pointIdx2))
            highIdx = max (int(pointIdx1), int(pointIdx2))

            # print(point + " " + p + " " + laneName+ "_" + str(lowIdx) + "_" + str(highIdx)  )
            for i in range(lowIdx, highIdx):
                # print(laneName+ "_" + str(lowIdx) + "_" + str(highIdx))
                for e in edges:
                    if e["name"] == (laneName+ "_" + str(i) + "_" + str(i+1)):
                        # print("    " + e["name"])

                        pairDistance[point][p] += e["value"]
                        # print(e["value"])
                        # print(i)
# print(pairDistance)
with open('pairDistance_4.json', 'w') as outfile:
    json.dump(pairDistance, outfile, indent=4)

