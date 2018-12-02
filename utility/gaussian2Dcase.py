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
g = psp.Gaussian2DGraph(NUM_POINTS, 0.6)
x=g.get_x_pos_list()
y=g.get_y_pos_list()
color_list = ['#3FF711', 'r', 'g', 'm','y','k','c','#00FF00']
marker_list = ['o', 'v', 's', '*', '+', 'x', 'D', '1']
# divide into >=4 parts
g.run(False)
# cat requirement, >=4, <=8
p = g.get_partations()
cv = g.get_critical_values()
# check category requirement
cat_1 = 4;
for i in range(len(p)):
    if(p[i]>=4):
        cat_1 = p[i]
        break
cat_2 = p[i+1]
cat_3 = p[i+2]
lambda_list = [cv[i-1],cv[i],cv[i+1]]
cat_num_list = [cat_1, cat_2, cat_3]
if(cat_3>8):
    print(cat_num_list, 'no satisfied split found, try to rerun the program!')
    exit(0)
print(lambda_list, 'critical values at three figures')    
for index, cat_num in enumerate(cat_num_list):
    ax = plt.subplot(1,3,index+1)    
    cat = g.get_category(cat_num)
    print('num of cat:', cat_num)
    for i in range(cat_num):
        xx=[]
        yy=[]
        for j in range(len(cat)):
            if(cat[j]==i):
                xx.append(x[j])
                yy.append(y[j])
        plt.scatter(xx,yy, c=color_list[i], marker = marker_list[i])
    ax.set_title('$\lambda = %.2f$' % lambda_list[index])
plt.savefig('4part.eps')
plt.show()
#cat