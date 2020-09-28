import matplotlib.pyplot as plt
from matplotlib.patches import Wedge, Rectangle
import matplotlib.animation as animation
import numpy as np

fig = plt.figure(frameon=False, figsize=(8, 8))
ax = fig.add_subplot(111, aspect='equal')

plt.xlim(-53, 53)
plt.ylim(-53, 53)

patches = []

lmda = 10
r_radius = 32
l_radius = 64

left_lane_val = (l_radius - r_radius - lmda ) / 2
right_lane_val = left_lane_val + lmda
corner_val = right_lane_val + r_radius

print("left: " + str(left_lane_val))
print("right: " + str(right_lane_val))
print("corner: " + str(corner_val))

# lower left
patches.append(Wedge((-1 * corner_val, -1 * corner_val), l_radius + .5, 0, 90, width=1, color='g'))
patches.append(Wedge((-1 * corner_val, -1 * corner_val), r_radius + .5, 0, 90, width=1, color='y'))

# upper left
patches.append(Wedge((-1 * corner_val, corner_val), l_radius + .5, 270, 0, width=1, color='y'))
patches.append(Wedge((-1 * corner_val, corner_val), r_radius + .5, 270, 0, width=1, color='b'))

# upper right
patches.append(Wedge((corner_val, corner_val), l_radius + .5, 180, 270, width=1, color='b'))
patches.append(Wedge((corner_val, corner_val), r_radius + .5, 180, 270, width=1, color='r'))

# lower right
patches.append(Wedge((corner_val, -1 * corner_val), l_radius + .5, 90, 180, width=1, color='r'))
patches.append(Wedge((corner_val, -1 * corner_val), r_radius + .5, 90, 180, width=1, color='g'))

# right vertical
patches.append(Rectangle((left_lane_val - .5, -1 * corner_val), 1, corner_val * 2, color='g'))
patches.append(Rectangle((right_lane_val - .5, -1 * corner_val), 1, corner_val * 2, color='g'))

# left vertical
patches.append(Rectangle((-1 * left_lane_val - .5, -1 * corner_val), 1, corner_val * 2, color='b'))
patches.append(Rectangle((-1 * right_lane_val - .5, -1 * corner_val), 1, corner_val * 2, color='b'))

# top horizontal
patches.append(Rectangle((-1 * corner_val, left_lane_val - .5), corner_val * 2, 1, color='r'))
patches.append(Rectangle((-1 * corner_val, right_lane_val - .5), corner_val * 2, 1, color='r'))

# bottom horizontal
patches.append(Rectangle((-1 * corner_val, -1 * left_lane_val - .5), corner_val * 2, 1, color='y'))
patches.append(Rectangle((-1 * corner_val, -1 * right_lane_val - .5), corner_val * 2, 1, color='y'))

for patch in patches:
    ax.add_patch(patch)


line, = ax.plot([], [], 'ko')


def init():
    line.set_data([], [])
    return line,


def animate(i):
    line.set_data([left_lane_val], [-1 * corner_val + i])
    return line,


anim = animation.FuncAnimation(fig, animate, init_func=init,
                               frames=100, interval=20, blit=False)
Writer = animation.FFMpegWriter(fps=30)

anim.save('basic_animation.mp4', writer=Writer)

plt.show()
