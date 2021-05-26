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

        text_line = []

        counter = 0
        for line_in_file in f:
            counter+=1
            # print(float(i)/10)
            data = line_in_file.split(";")

            in_point = data[0]
            out_point = data[1]
            earliest_arrival = float(data[3])
            in_time = float(data[4])
            out_time = float(data[5])
            speed = float(data[7])

            pos_x = 10000
            pos_y = 10000
            if (in_point == "WER_0" and out_point == "WER_7"):
                if (float(i)/10 >= earliest_arrival and float(i)/10 < in_time):
                    pos_x = -1 * corner_val
                    pos_y = -1* right_lane_val
                if (float(i)/10 >= in_time and float(i)/10 <= out_time +0.5):
                    pos_x = -1 * corner_val+  (float(i)-in_time*10)*speed/10 
                    pos_y = -1* right_lane_val
            elif (in_point == "WEL_0" and out_point == "WEL_7"):
                if (float(i)/10 >= earliest_arrival and float(i)/10 < in_time):
                    pos_x = (-1 * corner_val)
                    pos_y = (-1* left_lane_val)
                if (float(i)/10 >= in_time and float(i)/10 <= out_time +0.5):
                    pos_x = (-1 * corner_val+  (float(i)-in_time*10)*speed/10  )
                    pos_y = (-1* left_lane_val)
            elif (in_point == "EWR_0" and out_point == "EWR_7"):
                if (float(i)/10 >= earliest_arrival and float(i)/10 < in_time):
                    pos_x = (corner_val)
                    pos_y = (right_lane_val)
                if (float(i)/10 >= in_time and float(i)/10 <= out_time +0.5):
                    pos_x = (corner_val-  (float(i)-in_time*10)*speed/10  )
                    pos_y = (right_lane_val)
            elif (in_point == "EWL_0" and out_point == "EWL_7"):
                if (float(i)/10 >= earliest_arrival and float(i)/10 < in_time):
                    pos_x = (corner_val)
                    pos_y = (left_lane_val)
                if (float(i)/10 >= in_time and float(i)/10 <= out_time +0.5):
                    pos_x = (corner_val-  (float(i)-in_time*10)*speed/10  )
                    pos_y =(left_lane_val)
            elif (in_point == "SNR_0" and out_point == "SNR_7"):
                if (float(i)/10 >= earliest_arrival and float(i)/10 < in_time):
                    pos_x = (right_lane_val)
                    pos_y = (-1 * corner_val)
                if (float(i)/10 >= in_time and float(i)/10 <= out_time +0.5):
                    pos_x = (right_lane_val)
                    pos_y = ( -1 * corner_val+(float(i)-in_time*10)*speed/10  )
            elif (in_point == "SNL_0" and out_point == "SNL_7"):
                if (float(i)/10 >= earliest_arrival and float(i)/10 < in_time):
                    pos_x = (left_lane_val)
                    pos_y = (-1 * corner_val)
                if (float(i)/10 >= in_time and float(i)/10 <= out_time +0.5):
                    pos_x = (left_lane_val)
                    pos_y = ( -1 * corner_val+(float(i)-in_time*10)*speed/10  )
            elif (in_point == "NSR_0" and out_point == "NSR_7"):
                if (float(i)/10 >= earliest_arrival and float(i)/10 < in_time):
                    pos_x = (-1*right_lane_val)
                    pos_y = (corner_val)
                if (float(i)/10 >= in_time and float(i)/10 <= out_time +0.5):
                    pos_x= (-1*right_lane_val)
                    pos_y = ( corner_val - (float(i)-in_time*10)*speed/10  )
            elif (in_point == "NSL_0" and out_point == "NSL_7"):
                if (float(i)/10 >= earliest_arrival and float(i)/10 < in_time):
                    pos_x = (-1*left_lane_val)
                    pos_y = (corner_val)
                if (float(i)/10 >= in_time and float(i)/10 <= out_time +0.5):
                    pos_x = (-1*left_lane_val)
                    pos_y = ( corner_val - (float(i)-in_time*10)*speed/10  )




            elif (in_point == "WSR_0" and out_point == "WSR_1"):
                if (float(i)/10 >= earliest_arrival and float(i)/10 < in_time):
                    pos_x = (-1 * corner_val)
                    pos_y = (-1* right_lane_val)
                if (float(i)/10 >= in_time and float(i)/10 <= out_time +0.5):
                    angle = (   (float(i)-in_time*10)*speed/10/r_circle_length   )*math.pi/2
                    # print(angle)
                    # print(i, float(i)-in_time*10,   (float(i)-in_time*10)*speed/10/r_circle_length)
                    pos_x=(-1*corner_val + np.sin(angle)*r_radius  )
                    pos_y=( -1*right_lane_val - r_radius + np.cos(angle)*r_radius  )
            elif (in_point == "ENR_0" and out_point == "ENR_1"):
                if (float(i)/10 >= earliest_arrival and float(i)/10 < in_time):
                    pos_x=(corner_val)
                    pos_y=(right_lane_val)
                if (float(i)/10 >= in_time and float(i)/10 <= out_time +0.5):
                    angle = (   (float(i)-in_time*10)*speed/10/r_circle_length   )*math.pi/2
                    pos_x=(corner_val - np.sin(angle)*r_radius  )
                    pos_y=(right_lane_val + r_radius - np.cos(angle)*r_radius  )
            elif (in_point == "NWR_0" and out_point == "NWR_1"):
                if (float(i)/10 >= earliest_arrival and float(i)/10 < in_time):
                    pos_x=(-1*right_lane_val)
                    pos_y=(corner_val)
                if (float(i)/10 >= in_time and float(i)/10 <= out_time +0.5):
                    angle = (   (float(i)-in_time*10)*speed/10/r_circle_length   )*math.pi/2
                    pos_x=(-1*right_lane_val  - r_radius + np.cos(angle)*r_radius )
                    pos_y=(corner_val - np.sin(angle)*r_radius  )
            elif (in_point == "SER_0" and out_point == "SER_1"):
                if (float(i)/10 >= earliest_arrival and float(i)/10 < in_time):
                    pos_x=(right_lane_val)
                    pos_y=(-1*corner_val)
                if (float(i)/10 >= in_time and float(i)/10 <= out_time +0.5):
                    angle = (   (float(i)-in_time*10)*speed/10/r_circle_length   )*math.pi/2
                    pos_x=(right_lane_val  + r_radius - np.cos(angle)*r_radius )
                    pos_y=(-1*corner_val + np.sin(angle)*r_radius  )
            elif (in_point == "WNL_0" and out_point == "WNL_7"):
                if (float(i)/10 >= earliest_arrival and float(i)/10 < in_time):
                    pos_x=(-1 * corner_val)
                    pos_y=(-1* left_lane_val)
                if (float(i)/10 >= in_time and float(i)/10 <= out_time +0.5):
                    angle = (   (float(i)-in_time*10)*speed/10/l_circle_length   )*math.pi/2
                    # print(angle)
                    # print(i, float(i)-in_time*10,   (float(i)-in_time*10)*speed/10/r_circle_length)
                    pos_x=(-1*corner_val + np.sin(angle)*l_radius )
                    pos_y=( -1*left_lane_val + l_radius - np.cos(angle)*l_radius   )
            elif (in_point == "ESL_0" and out_point == "ESL_7"):
                if (float(i)/10 >= earliest_arrival and float(i)/10 < in_time):
                    pos_x=(corner_val)
                    pos_y=(left_lane_val)
                if (float(i)/10 >= in_time and float(i)/10 <= out_time +0.5):
                    angle = (   (float(i)-in_time*10)*speed/10/l_circle_length   )*math.pi/2
                    pos_x=(corner_val - np.sin(angle)*l_radius )
                    pos_y=(left_lane_val - l_radius + np.cos(angle)*l_radius   )
            elif (in_point == "NEL_0" and out_point == "NEL_7"):
                if (float(i)/10 >= earliest_arrival and float(i)/10 < in_time):
                    pos_x=(-1*left_lane_val)
                    pos_y=(corner_val)
                if (float(i)/10 >= in_time and float(i)/10 <= out_time +0.5):
                    angle = (   (float(i)-in_time*10)*speed/10/l_circle_length   )*math.pi/2
                    pos_x=(-1*left_lane_val  + l_radius - np.cos(angle)*l_radius )
                    pos_y=(corner_val - np.sin(angle)*l_radius   )
            elif (in_point == "SWL_0" and out_point == "SWL_7"):
                if (float(i)/10 >= earliest_arrival and float(i)/10 < in_time):
                    pos_x=(left_lane_val)
                    pos_y=(-1*corner_val)
                if (float(i)/10 >= in_time and float(i)/10 <= out_time +0.5):
                    angle = (   (float(i)-in_time*10)*speed/10/l_circle_length   )*math.pi/2
                    pos_x=(left_lane_val  - l_radius + np.cos(angle)*l_radius )
                    pos_y=(-1*corner_val + np.sin(angle)*l_radius   )

            if (pos_x<16 and pos_x>-16 and pos_y<16 and pos_y>-16):
                new_position_x.append(pos_x)
                new_position_y.append(pos_y)

                text_line.append(str(counter))
            # if (len(new_position_x) > 0 and len(new_position_y) > 0):

    # line.set_data([left_lane_val, right_lane_val], [-1 * corner_val+i,-1 * corner_val+0.5*i])
        if (len(new_position_x) > 0 and len(new_position_y) > 0):
            # print(new_position_x, new_position_y)
            line.set_data(new_position_x, new_position_y)
            for txt in ax.texts:
                txt.set_visible(False)
            for text_idx in range(len(new_position_x)):
                text = ax.text(new_position_x[text_idx], new_position_y[text_idx]+1, text_line[text_idx], color='red', fontsize=10)
                # text.set_x(new_position_x[text_idx])
                # text.set_y(new_position_y[text_idx]+1)
                # text.set_text(str(text_line[text_idx]))

    return line,

# print([left_lane_val, right_lane_val], [-1 * corner_val,-1 * corner_val])


# change frames parameter, for example: 10 frame with interval 100ms will result in 1 second of animation
anim = animation.FuncAnimation(fig, animate, init_func=init,
                               frames=750, interval=100, blit=False)
Writer = animation.FFMpegWriter(fps=30)

anim.save('/home/theanhhoang/Desktop/AIM/src/basic_animation.mp4', writer=Writer)

plt.show()
