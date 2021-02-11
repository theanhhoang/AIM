import matplotlib.pyplot as plt
from matplotlib.patches import Wedge, Rectangle
import matplotlib.animation as animation
import numpy as np
import math

fig = plt.figure(frameon=False, figsize=(8, 8))
ax = fig.add_subplot(111, aspect='equal')

plt.xlim(-15.9, 15.9)
plt.ylim(-15.9, 15.9)

patches = []

lmda = 3
r_radius = 9.6
l_radius = 19.2

r_circle_length = math.pi*r_radius/2
l_circle_length = math.pi*l_radius/2


left_lane_val = (l_radius - r_radius - lmda ) / 2
right_lane_val = left_lane_val + lmda
corner_val = right_lane_val + r_radius


# corner_val = lmda + r_radius + l_radius
# left_lane_val = l_radius
# right_lane_val = r_radius

print("left: " + str(left_lane_val))
print("right: " + str(right_lane_val))
print("corner: " + str(corner_val))

# lower left
patches.append(Wedge((-1 * corner_val, -1 * corner_val), l_radius + .25, 0, 90, width=0.5, color='g'))
patches.append(Wedge((-1 * corner_val, -1 * corner_val), r_radius + .25, 0, 90, width=0.5, color='y'))

# upper left
patches.append(Wedge((-1 * corner_val, corner_val), l_radius + .25, 270, 0, width=0.5, color='y'))
patches.append(Wedge((-1 * corner_val, corner_val), r_radius + .25, 270, 0, width=0.5, color='b'))

# upper right
patches.append(Wedge((corner_val, corner_val), l_radius + .25, 180, 270, width=0.5, color='b'))
patches.append(Wedge((corner_val, corner_val), r_radius + .25, 180, 270, width=0.5, color='r'))

# lower right
patches.append(Wedge((corner_val, -1 * corner_val), l_radius + .25, 90, 180, width=0.5, color='r'))
patches.append(Wedge((corner_val, -1 * corner_val), r_radius + .25, 90, 180, width=0.5, color='g'))

# right vertical
patches.append(Rectangle((left_lane_val - .25, -1 * corner_val), 0.5, corner_val * 2, color='g'))
patches.append(Rectangle((right_lane_val - .25, -1 * corner_val), 0.5, corner_val * 2, color='g'))

# left vertical
patches.append(Rectangle((-1 * left_lane_val - .25, -1 * corner_val), 0.5, corner_val * 2, color='b'))
patches.append(Rectangle((-1 * right_lane_val - .25, -1 * corner_val), 0.5, corner_val * 2, color='b'))

# top horizontal
patches.append(Rectangle((-1 * corner_val, left_lane_val - .25), corner_val * 2, 0.5, color='r'))
patches.append(Rectangle((-1 * corner_val, right_lane_val - .25), corner_val * 2, 0.5, color='r'))

# bottom horizontal
patches.append(Rectangle((-1 * corner_val, -1 * left_lane_val - .25), corner_val * 2, 0.5, color='y'))
patches.append(Rectangle((-1 * corner_val, -1 * right_lane_val - .25), corner_val * 2, 0.5, color='y'))

for patch in patches:
    ax.add_patch(patch)


line, = ax.plot([], [], 'ko')


def init():
    line.set_data([], [])
    return line,


def animate(i):
    # print(i, -1 * corner_val+i)

    with open("output.txt") as f:
        next(f)
        new_position_x = []
        new_position_y = []

        for line_in_file in f:
            # print(float(i)/10)
            data = line_in_file.split(";")

            in_point = data[0]
            out_point = data[1]
            earliest_arrival = float(data[3])
            in_time = float(data[4])
            out_time = float(data[5])
            speed = float(data[7])

            if (in_point == "WER_0" and out_point == "WER_7"):
                if (float(i)/10 >= earliest_arrival and float(i)/10 < in_time):
                    new_position_x.append(-1 * corner_val)
                    new_position_y.append(-1* right_lane_val)
                if (float(i)/10 >= in_time and float(i)/10 <= out_time +0.5):
                    new_position_x.append(-1 * corner_val+  (float(i)-in_time*10)*speed/10  )
                    new_position_y.append(-1* right_lane_val)
            elif (in_point == "WEL_0" and out_point == "WEL_7"):
                if (float(i)/10 >= earliest_arrival and float(i)/10 < in_time):
                    new_position_x.append(-1 * corner_val)
                    new_position_y.append(-1* left_lane_val)
                if (float(i)/10 >= in_time and float(i)/10 <= out_time +0.5):
                    new_position_x.append(-1 * corner_val+  (float(i)-in_time*10)*speed/10  )
                    new_position_y.append(-1* left_lane_val)
            elif (in_point == "EWR_0" and out_point == "EWR_7"):
                if (float(i)/10 >= earliest_arrival and float(i)/10 < in_time):
                    new_position_x.append(corner_val)
                    new_position_y.append(right_lane_val)
                if (float(i)/10 >= in_time and float(i)/10 <= out_time +0.5):
                    new_position_x.append(corner_val-  (float(i)-in_time*10)*speed/10  )
                    new_position_y.append(right_lane_val)
            elif (in_point == "EWL_0" and out_point == "EWL_7"):
                if (float(i)/10 >= earliest_arrival and float(i)/10 < in_time):
                    new_position_x.append(corner_val)
                    new_position_y.append(left_lane_val)
                if (float(i)/10 >= in_time and float(i)/10 <= out_time +0.5):
                    new_position_x.append(corner_val-  (float(i)-in_time*10)*speed/10  )
                    new_position_y.append(left_lane_val)
            elif (in_point == "SNR_0" and out_point == "SNR_7"):
                if (float(i)/10 >= earliest_arrival and float(i)/10 < in_time):
                    new_position_x.append(right_lane_val)
                    new_position_y.append(-1 * corner_val)
                if (float(i)/10 >= in_time and float(i)/10 <= out_time +0.5):
                    new_position_x.append(right_lane_val)
                    new_position_y.append( -1 * corner_val+(float(i)-in_time*10)*speed/10  )
            elif (in_point == "SNL_0" and out_point == "SNL_7"):
                if (float(i)/10 >= earliest_arrival and float(i)/10 < in_time):
                    new_position_x.append(left_lane_val)
                    new_position_y.append(-1 * corner_val)
                if (float(i)/10 >= in_time and float(i)/10 <= out_time +0.5):
                    new_position_x.append(left_lane_val)
                    new_position_y.append( -1 * corner_val+(float(i)-in_time*10)*speed/10  )
            elif (in_point == "NSR_0" and out_point == "NSR_7"):
                if (float(i)/10 >= earliest_arrival and float(i)/10 < in_time):
                    new_position_x.append(-1*right_lane_val)
                    new_position_y.append(corner_val)
                if (float(i)/10 >= in_time and float(i)/10 <= out_time +0.5):
                    new_position_x.append(-1*right_lane_val)
                    new_position_y.append( corner_val - (float(i)-in_time*10)*speed/10  )
            elif (in_point == "NSL_0" and out_point == "NSL_7"):
                if (float(i)/10 >= earliest_arrival and float(i)/10 < in_time):
                    new_position_x.append(-1*left_lane_val)
                    new_position_y.append(corner_val)
                if (float(i)/10 >= in_time and float(i)/10 <= out_time +0.5):
                    new_position_x.append(-1*left_lane_val)
                    new_position_y.append( corner_val - (float(i)-in_time*10)*speed/10  )




            elif (in_point == "WSR_0" and out_point == "WSR_1"):
                if (float(i)/10 >= earliest_arrival and float(i)/10 < in_time):
                    new_position_x.append(-1 * corner_val)
                    new_position_y.append(-1* right_lane_val)
                if (float(i)/10 >= in_time and float(i)/10 <= out_time +0.5):
                    angle = (   (float(i)-in_time*10)*speed/10/r_circle_length   )*math.pi/2
                    # print(angle)
                    # print(i, float(i)-in_time*10,   (float(i)-in_time*10)*speed/10/r_circle_length)
                    new_position_x.append(-1*corner_val + np.sin(angle)*r_radius  )
                    new_position_y.append( -1*right_lane_val - r_radius + np.cos(angle)*r_radius  )
            elif (in_point == "ENR_0" and out_point == "ENR_1"):
                if (float(i)/10 >= earliest_arrival and float(i)/10 < in_time):
                    new_position_x.append(corner_val)
                    new_position_y.append(right_lane_val)
                if (float(i)/10 >= in_time and float(i)/10 <= out_time +0.5):
                    angle = (   (float(i)-in_time*10)*speed/10/r_circle_length   )*math.pi/2
                    new_position_x.append(corner_val - np.sin(angle)*r_radius  )
                    new_position_y.append(right_lane_val + r_radius - np.cos(angle)*r_radius  )
            elif (in_point == "NWR_0" and out_point == "NWR_1"):
                if (float(i)/10 >= earliest_arrival and float(i)/10 < in_time):
                    new_position_x.append(-1*right_lane_val)
                    new_position_y.append(corner_val)
                if (float(i)/10 >= in_time and float(i)/10 <= out_time +0.5):
                    angle = (   (float(i)-in_time*10)*speed/10/r_circle_length   )*math.pi/2
                    new_position_x.append(-1*right_lane_val  - r_radius + np.cos(angle)*r_radius )
                    new_position_y.append(corner_val - np.sin(angle)*r_radius  )
            elif (in_point == "SER_0" and out_point == "SER_1"):
                if (float(i)/10 >= earliest_arrival and float(i)/10 < in_time):
                    new_position_x.append(right_lane_val)
                    new_position_y.append(-1*corner_val)
                if (float(i)/10 >= in_time and float(i)/10 <= out_time +0.5):
                    angle = (   (float(i)-in_time*10)*speed/10/r_circle_length   )*math.pi/2
                    new_position_x.append(right_lane_val  + r_radius - np.cos(angle)*r_radius )
                    new_position_y.append(-1*corner_val + np.sin(angle)*r_radius  )
            elif (in_point == "WNL_0" and out_point == "WNL_7"):
                if (float(i)/10 >= earliest_arrival and float(i)/10 < in_time):
                    new_position_x.append(-1 * corner_val)
                    new_position_y.append(-1* left_lane_val)
                if (float(i)/10 >= in_time and float(i)/10 <= out_time +0.5):
                    angle = (   (float(i)-in_time*10)*speed/10/l_circle_length   )*math.pi/2
                    # print(angle)
                    # print(i, float(i)-in_time*10,   (float(i)-in_time*10)*speed/10/r_circle_length)
                    new_position_x.append(-1*corner_val + np.sin(angle)*l_radius )
                    new_position_y.append( -1*left_lane_val + l_radius - np.cos(angle)*l_radius   )
            elif (in_point == "ESL_0" and out_point == "ESL_7"):
                if (float(i)/10 >= earliest_arrival and float(i)/10 < in_time):
                    new_position_x.append(corner_val)
                    new_position_y.append(left_lane_val)
                if (float(i)/10 >= in_time and float(i)/10 <= out_time +0.5):
                    angle = (   (float(i)-in_time*10)*speed/10/l_circle_length   )*math.pi/2
                    new_position_x.append(corner_val - np.sin(angle)*l_radius )
                    new_position_y.append(left_lane_val - l_radius + np.cos(angle)*l_radius   )
            elif (in_point == "NEL_0" and out_point == "NEL_7"):
                if (float(i)/10 >= earliest_arrival and float(i)/10 < in_time):
                    new_position_x.append(-1*left_lane_val)
                    new_position_y.append(corner_val)
                if (float(i)/10 >= in_time and float(i)/10 <= out_time +0.5):
                    angle = (   (float(i)-in_time*10)*speed/10/l_circle_length   )*math.pi/2
                    new_position_x.append(-1*left_lane_val  + l_radius - np.cos(angle)*l_radius )
                    new_position_y.append(corner_val - np.sin(angle)*l_radius   )
            elif (in_point == "SWL_0" and out_point == "SWL_7"):
                if (float(i)/10 >= earliest_arrival and float(i)/10 < in_time):
                    new_position_x.append(left_lane_val)
                    new_position_y.append(-1*corner_val)
                if (float(i)/10 >= in_time and float(i)/10 <= out_time +0.5):
                    angle = (   (float(i)-in_time*10)*speed/10/l_circle_length   )*math.pi/2
                    new_position_x.append(left_lane_val  - l_radius + np.cos(angle)*l_radius )
                    new_position_y.append(-1*corner_val + np.sin(angle)*l_radius   )



    # line.set_data([left_lane_val, right_lane_val], [-1 * corner_val+i,-1 * corner_val+0.5*i])
        if (len(new_position_x) > 0 and len(new_position_y) > 0):
            # print(new_position_x, new_position_y)
            line.set_data(new_position_x, new_position_y)
    return line,

# print([left_lane_val, right_lane_val], [-1 * corner_val,-1 * corner_val])


# change frames parameter, for example: 10 frame with interval 100ms will result in 1 second of animation
anim = animation.FuncAnimation(fig, animate, init_func=init,
                               frames=750, interval=100, blit=False)
Writer = animation.FFMpegWriter(fps=30)

anim.save('/home/theanhhoang/Desktop/AIM/src/basic_animation.mp4', writer=Writer)

plt.show()
