# \author: zhaofeng-shu33
from matplotlib import pyplot as plt
import psp
NUM_POINTS = 16
g = psp.Gaussian2DGraph(NUM_POINTS)
x=g.get_x_pos_list()
y=g.get_y_pos_list()
plt.scatter(x,y)
plt.show()
#g.run(False)
#cat = g.get_category(4)
#cat