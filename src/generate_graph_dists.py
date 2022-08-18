import json


with open("intro_graph.json") as f:
    data = json.load(f)

vertices = data['vertices']

dist_data = dict()

for this_vertex in vertices:
    this_name = this_vertex["name"]
    this_pos = this_vertex["pos"]

    dist_data[this_name] = []

    for other_vertex in vertices:
        other_name = other_vertex["name"]
        other_pos = other_vertex["pos"]
        dist = round(((this_pos[0] - other_pos[0]) ** 2) + ((this_pos[1] - other_pos[1]) ** 2) ** .5, 5)

        dist_data[this_name].append(dict([("name", other_name), ("dist", dist)]))

with open("dist_graph.json", 'w') as f:
    json.dump(dist_data, f, indent=4)
