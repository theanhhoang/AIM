import numpy as np
import json

#lane: WR, WL, ER, EL, NR, NL, SR, SL

lanes = ["WR", "WL", "ER", "EL", "NR", "NL", "SR", "SL"]
laneDirections = [[["WER_0", "WER_1", "WER_2", "WER_3", "WER_4", "WER_5", "WER_6", "WER_7"], \
                            ["WSR_0", "WSR_1"]], \
                    [["WEL_0", "WEL_1", "WEL_2", "WEL_3", "WEL_4", "WEL_5", "WEL_6", "WEL_7"], \
                            ["WNL_0", "WNL_1", "WNL_2", "WNL_3", "WNL_4", "WNL_5", "WNL_6", "WNL_7"]], \
                    [["EWR_0", "EWR_1", "EWR_2", "EWR_3", "EWR_4", "EWR_5", "EWR_6", "EWR_7"], \
                            ["ENR_0", "ENR_1"]], \
                    [["EWL_0", "EWL_1", "EWL_2", "EWL_3", "EWL_4", "EWL_5", "EWL_6", "EWL_7"], \
                            ["ESL_0", "ESL_1", "ESL_2", "ESL_3", "ESL_4", "ESL_5", "ESL_6", "ESL_7"]], \
                    [["NSR_0", "NSR_1", "NSR_2", "NSR_3", "NSR_4", "NSR_5", "NSR_6", "NSR_7"], \
                            ["NWR_0", "NWR_1"]], \
                    [["NSL_0", "NSL_1", "NSL_2", "NSL_3", "NSL_4", "NSL_5", "NSL_6", "NSL_7"], \
                            ["NEL_0", "NEL_1", "NEL_2", "NEL_3", "NEL_4", "NEL_5", "NEL_6", "NEL_7"]], \
                    [["SNR_0", "SNR_1", "SNR_2", "SNR_3", "SNR_4", "SNR_5", "SNR_6", "SNR_7"], \
                            ["SER_0", "SER_1"]], \
                    [["SNL_0", "SNL_1", "SNL_2", "SNL_3", "SNL_4", "SNL_5", "SNL_6", "SNL_7"], \
                            ["SWL_0", "SWL_1", "SWL_2", "SWL_3", "SWL_4", "SWL_5", "SWL_6", "SWL_7"]]  \
                ]

arrivalTime = np.random.poisson(0.5625,6400)
arrivalTime = np.cumsum(arrivalTime)

# print(arrivalTime)

trajectory = np.random.randint(0,10,6400)
lane = np.random.randint(0,8, 6400)
#     vehs = {}
vehs = []

for i in range(len(arrivalTime)):
    veh = {}
    veh["arrivalTime"] = arrivalTime[i] + np.random.random()*100//1/100

    # 80% vehicle go straigth, 20% turn right or left
    if (trajectory[i] > 1):
        veh["trajectory"] = laneDirections[lane[i]][0]
    else:
        veh["trajectory"] = laneDirections[lane[i]][1]
    veh["id"] = i
    veh["lane"] = lane[i]


    # if str(     (arrivalTime[i]/10)      ) not in vehs:
    #     vehs[str(     (arrivalTime[i]/10)      )] = []
    # vehs[str(     (arrivalTime[i]/10)      )].append(veh)
    vehs.append(veh)

# print(vehicleArrival)
vehicleArrival = {}

for i in range(40):
    
    print(lanes[vehs[i]["lane"]], vehs[i]["arrivalTime"])
    # print(vehicleArrival[lane][vehNo])
    # if lanes[vehs[i]["lane"]] not in vehicleArrival:
    #     vehicleArrival[lanes[vehs[i]["lane"]]] = []

    # vehicleArrival[lanes[vehs[i]["lane"]]].append(vehs[i])
    vehicleArrival[i] = vehs[i]
# print(vehicleArrival)

with open('vehicleArrivalNew.json', 'w') as outfile:
    json.dump(vehicleArrival, outfile, indent=4)
