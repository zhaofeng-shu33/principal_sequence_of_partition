# \author: zhaofeng-shu33
from matplotlib import pyplot as plt
import psp
import numpy as np
def to_nparray(a):
    np_a = np.zeros(len(a))
    for i in range(len(a)):
        np_a[i]=a[i]
    return np_a
    
NUM_POINTS = 100
g = psp.Gaussian2DGraph(NUM_POINTS)
x=g.get_x_pos_list()
y=g.get_y_pos_list()
color_list = ['b', 'r', 'g', 'm','y','k']
marker_list = ['o','v','s','*','+','x']
# divide into >=4 parts
g.run(False)
cat = g.get_category(4)
print('num of cat:', max(cat)+1)
for i in range(4):
    xx=[]
    yy=[]
    for j in range(len(cat)):
        if(cat[j]==i):
            xx.append(x[j])
            yy.append(y[j])
    plt.scatter(xx,yy, c=color_list[i], marker = marker_list[i])
plt.show()
#cat