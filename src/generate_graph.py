#import yaml
import sys
import math
import numpy as np
from collections import OrderedDict
import json

"""
def setup_yaml():
    https://stackoverflow.com/a/8661021 
    represent_dict_order = lambda self, data:  self.represent_mapping('tag:yaml.org,2002:map', data.items())
    yaml.add_representer(OrderedDict, represent_dict_order)
setup_yaml()
"""

round_to = 5
graph = OrderedDict(vertices=[], edges=[])


def rotate_pt(arr, count):
    rot = np.array([
        [0, 1],
        [-1, 0]
    ])
    for x in range(count):
        arr = rot @ arr
    return arr


def add_vertex(path, v_num, v_pos, num_trans, conf_path, conf_v_num):
    transformed = rotate_pt(v_pos, num_trans)
    graph['vertices'].append(OrderedDict(
        name=path+"_"+str(v_num),
        pos=[round(float(transformed[0]), round_to), round(float(transformed[1]), round_to)],
        vertex_conflicts=[conf_path+"_"+str(conf_v_num)]
    ))


directions = ['W', 'N', 'E', 'S', 'W', 'N', 'E', 'S']
lane = ['R', 'L']


# Set these constants before generating graph
# lmda = lambda = distance between lanes (lambda is keyword in python)
# r_radius = right turn radius
# l_radius = left turn radius
# omega = distance from start to end CP's going straight
# constraint: lambda < r_radius < l_radius < omega, l_radius + r_radius + lambda = omega
lmda = 12*0.3048
r_radius = 6*0.3048
l_radius = 30*0.3048
omega = lmda + r_radius + l_radius


"""
for i in range(0, 4):
    approach = directions[i]
    left = directions[i + 1]
    straight = directions[i + 2]
    right = directions[i + 3]

    # definition of left lane going straight
    ll_straight = approach + straight + 'L'

    # definition of right lane going straight
    rl_straight = approach + straight + 'R'

    # CP 1: WEL_WNL
    add_vertex(ll_straight, approach+left+'L', [0, 0])
    # CP 2: WER_WSR
    add_vertex(rl_straight, approach+right+'R', [0,0])
    # CP 3: WEL_NSR
    add_vertex(ll_straight, left+right+'R', [0,0])
    # CP 4: WER_NSR
    add_vertex(rl_straight, left+right+'R', [0,0])
    # CP 5: WEL_NSL
    add_vertex(ll_straight, left+right+'L', [0,0])
    # CP 6: WER_NSL
    add_vertex(rl_straight, left+right+'L', [0,0])
    # CP 7: WEL_SWL
    add_vertex(ll_straight, right+approach+'L', [0,0])
    # CP 8: WER_ESL
    add_vertex(rl_straight, straight+right+'L', [0,0])
    # CP 9: WER_SWL
    add_vertex(rl_straight, right+approach+'L', [0,0])
    # CP 10: WEL_ESL
    add_vertex(ll_straight, straight+right+'L', [0,0])
    # CP 11: WEL_NEL
    add_vertex(ll_straight, left+straight+'L', [0,0])
    # CP 12: WER_SER
    add_vertex(rl_straight, right+straight+'R', [0,0])
    # CP 13: WNL_SWL
    add_vertex(approach+left+'L', right+approach+'L', [0,0])
"""

right_turn_dist = r_radius * math.pi / 2

right_straight_dist = np.array([
    r_radius,
    r_radius + lmda,
    omega - ((l_radius ** 2)-(r_radius ** 2)) ** .5,
    ((l_radius ** 2)-(r_radius ** 2)) ** .5,
    l_radius,
    l_radius + lmda,
    omega
])

left_straight_dist = np.array([
    r_radius,
    r_radius + lmda,
    ((l_radius ** 2) - ((r_radius + lmda) ** 2)) ** .5,
    omega - (((l_radius ** 2) - ((r_radius + lmda) ** 2)) ** .5),
    l_radius,
    l_radius + lmda,
    omega
])

left_turn_dist = np.array([
    l_radius * np.arcsin(r_radius / l_radius),
    l_radius * np.arccos(omega / (2 * l_radius)),
    l_radius * np.arcsin((r_radius + lmda)/l_radius),
    l_radius * np.arccos((r_radius + lmda)/l_radius),
    l_radius * np.arcsin(omega / (2 * l_radius)),
    l_radius * np.arccos(r_radius / l_radius),
    l_radius * math.pi / 2
])

print(right_turn_dist)
print(right_straight_dist)
print(left_straight_dist)
print(left_turn_dist)


def add_edge(path, num_1, num_2, val):
    graph['edges'].append(OrderedDict(
        name=path+"_"+str(num_1)+"_"+str(num_2), 
        fro=path+'_'+str(num_1),
        to=path+'_'+str(num_2),
        value=round(val, round_to)
    ))


for i in range(0, 4):
    approach = directions[i]
    left = directions[i + 1]
    straight = directions[i + 2]
    right = directions[i + 3]

    right_turn_path = approach+right+'R'
    right_straight_path = approach+straight+'R'
    left_straight_path = approach+straight+'L'
    left_turn_path = approach+left+'L'

    rotate_1_right_turn_path = left+approach+'R'
    rotate_1_right_straight_path = left+right+'R'
    rotate_1_left_straight_path = left+right+'L'
    rotate_1_left_turn_path = left+straight+'L'

    rotate_2_right_turn_path = straight+left+'R'
    rotate_2_right_straight_path = straight+approach+'R'
    rotate_2_left_straight_path = straight+approach+'L'
    rotate_2_left_turn_path = straight+right+'L'

    rotate_3_right_turn_path = right+straight+'R'
    rotate_3_right_straight_path = right+left+'R'
    rotate_3_left_straight_path = right+left+'L'
    rotate_3_left_turn_path = right+approach+'L'

    left_lane_val = (r_radius + lmda - l_radius) / 2
    right_lane_val = left_lane_val - lmda
    corner_val = right_lane_val - r_radius

    # Right turn vertices

    add_vertex(
        right_turn_path, 0,
        np.array([corner_val, right_lane_val]), i,
        right_straight_path, 0
    )
    add_vertex(
        right_turn_path, 1,
        np.array([corner_val + r_radius, corner_val]), i,
        rotate_1_right_straight_path, 7
    )
    # Right turn edge
    # WSR_0_1, WER_WSR, NSR_WSR
    add_edge(
        right_turn_path, 0, 1,
        right_turn_dist
    )


    right_straight_y_coords = np.ones(7) * right_lane_val
    right_straight_x_coords = corner_val + right_straight_dist
    right_straight_pos = np.stack([right_straight_x_coords, right_straight_y_coords], axis=-1)

    # Left straight vertices
    add_vertex(
        right_straight_path, 0,
        np.array([corner_val, right_lane_val]), i,
        right_turn_path, 0
    )

    right_straight_conflict_paths = [
        rotate_1_right_straight_path,
        rotate_1_left_straight_path,
        rotate_2_left_turn_path,
        rotate_3_left_turn_path,
        rotate_3_left_straight_path,
        rotate_3_right_straight_path,
        rotate_3_right_turn_path
    ]

    right_straight_conflict_index = [6, 6, 6, 1, 1, 1, 1]

    for j in range(7):
        add_vertex(
            right_straight_path, j + 1,
            right_straight_pos[j, :], i,
            right_straight_conflict_paths[j], right_straight_conflict_index[j]
        )

    # Right straight edges
    # WER_0_1, WER_WSR, WER_NSR
    add_edge(
        right_straight_path, 0, 1,
        right_straight_dist[0]
    )
    
    # WER_1_2, WER_NSR, WER_NSL
    add_edge(
        right_straight_path, 1, 2,
        right_straight_dist[1] - right_straight_dist[0]
    )

    # WER_2_3, WER_NSL, WER_ESL
    add_edge(
        right_straight_path, 2, 3,
        right_straight_dist[2] - right_straight_dist[1]
    )

    # WER_3_4, WER_ESL, WER_SWL
    add_edge(
        right_straight_path, 3, 4,
        right_straight_dist[3] - right_straight_dist[2]
    )

    # WER_4_5, WER_SWL, SNL_WER
    add_edge(
        right_straight_path, 4, 5,
        right_straight_dist[4] - right_straight_dist[3]
    )

    # WER_5_6, SNL_WER, SNR_WER
    add_edge(
        right_straight_path, 5, 6,
        right_straight_dist[5] - right_straight_dist[4]
    )

    # WER_6_7, SNR_WER, WER_SER
    add_edge(
        right_straight_path, 6, 7,
        right_straight_dist[6] - right_straight_dist[5]
    )





    left_straight_y_coords = np.ones(7) * left_lane_val
    left_straight_x_coords = corner_val + left_straight_dist
    left_straight_pos = np.stack([left_straight_x_coords, left_straight_y_coords], axis=-1)

    # Left straight vertices
    add_vertex(
        left_straight_path, 0,
        np.array([corner_val, left_lane_val]), i,
        left_turn_path, 0
    )

    left_straight_conflict_paths = [
        rotate_1_right_straight_path,
        rotate_1_left_straight_path,
        rotate_3_left_turn_path,
        rotate_2_left_turn_path,
        rotate_3_left_straight_path,
        rotate_3_right_straight_path,
        rotate_1_left_turn_path
    ]

    left_straight_conflict_index = [5, 5, 3, 4, 2, 2, 7]

    for j in range(7):
        add_vertex(
            left_straight_path, j+1,
            left_straight_pos[j, :], i,
            left_straight_conflict_paths[j], left_straight_conflict_index[j]
        )

    # Left straight edges
    # WEL_0_1, WEL_WNL, WEL_NSR
    add_edge(
        left_straight_path, 0, 1,
        left_straight_dist[0]
    )
    
    # WEL_1_2, WEL_NSR, WEL_NSL
    add_edge(
        left_straight_path, 1, 2,
        left_straight_dist[1] - left_straight_dist[0]
    )

    # WEL_2_3, WEL_NSL, WEL_SWL
    add_edge(
        left_straight_path, 2, 3,
        left_straight_dist[2] - left_straight_dist[1]
    )

    # WEL_3_4, WEL_SWL, WEL_ESL
    add_edge(
        left_straight_path, 3, 4,
        left_straight_dist[3] - left_straight_dist[2]
    )

    # WEL_4_5, WEL_ESL, SNL_WEL
    add_edge(
        left_straight_path, 4, 5,
        left_straight_dist[4] - left_straight_dist[3]
    )

    # WEL_5_6, SNL_WEL, SNR_WEL
    add_edge(
        left_straight_path, 5, 6,
        left_straight_dist[5] - left_straight_dist[4]
    )

    # WEL_6_7, SNR_WEL, WEL_NEL
    add_edge(
        left_straight_path, 6, 7,
        left_straight_dist[6] - left_straight_dist[5]
    )

    pi_270 = np.pi * 3 / 2
    left_turn_radians = pi_270 + (left_turn_dist/l_radius)
    left_turn_x_coords = l_radius * np.cos(left_turn_radians) + corner_val
    left_turn_y_coords = l_radius * np.sin(left_turn_radians) - corner_val

    left_turn_coords = np.stack([left_turn_x_coords, left_turn_y_coords], axis=-1)
    # Left turn vertices
    add_vertex(
        left_turn_path, 0,
        np.array([corner_val, left_lane_val]), i,
        left_straight_path, 0
    )

    left_conflict_paths = [
        rotate_1_right_straight_path,
        rotate_3_left_turn_path,
        rotate_1_left_straight_path,
        rotate_2_left_straight_path,
        rotate_1_left_turn_path,
        rotate_2_right_straight_path,
        rotate_3_left_straight_path
    ]

    left_conflict_path_index = [4, 5, 3, 4, 2, 3, 7]
    for j in range(7):
        add_vertex(
            left_turn_path, j+1,
            left_turn_coords[j, :], i,
            left_conflict_paths[j], left_conflict_path_index[j]
        )

    # Left turn edges
    # WNL_0_1, WEL_WNL, NSR_WNL
    add_edge(
        left_turn_path, 0, 1,
        left_turn_dist[0].item()
    )

    # WNL_1_2, NSR_WNL, WNL_SWL
    add_edge(
        left_turn_path, 1, 2,
        left_turn_dist[1].item() - left_turn_dist[0].item()
    )
    
    # WNL_2_3, WNL_SWL, NSL_WNL
    add_edge(
        left_turn_path, 2, 3,
        left_turn_dist[2].item() - left_turn_dist[1].item()
    )
    
    # WNL_3_4, NSL_WNL, EWL_WNL
    add_edge(
        left_turn_path, 3, 4,
        left_turn_dist[3].item() - left_turn_dist[2].item()
    )
    
    # WNL_4_5, EWL_WNL, NEL_WNL
    add_edge(
        left_turn_path, 4, 5,
        left_turn_dist[4].item() - left_turn_dist[3].item()
    )
    
    # WNL_5_6, NEL_WNL, EWR_WNL
    add_edge(
        left_turn_path, 5, 6,
        left_turn_dist[5].item() - left_turn_dist[4].item()
    )
    
    # WNL_6_7, EWR_WNL, SNL_WNL
    add_edge(
        left_turn_path, 6, 7,
        left_turn_dist[6] - left_turn_dist[5].item()
    )

with open("intro_graph_4.json", 'w') as f:
    json.dump(graph, f, indent=4)