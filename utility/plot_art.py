# \author: zhaofeng-shu33
'''
    plot artificial dataset
'''
import os
import random
import math
import cmath
import argparse
import pdb

from matplotlib import pyplot as plt
import numpy as np


from info_cluster import InfoCluster
import schema
color_list = ['#3FF711', 'r', 'g', 'm','y','k','c','#00FF00']
marker_list = ['o', 'v', 's', '*', '+', 'x', 'D', '1']
MAX_CAT = len(color_list)
USE_PDT = False
SHOW_PIC = False
class ThreeCircle:
    def __init__(self, np_list, gamma_1=1, gamma_2=1):
        '''
        np is the number of points at each circle
        '''
        #  (0, 0.1) to (0, 0.2)
        pos_list = []
        self._np = 10;
        for i in range(10):
            pos_list.append([0, random.random()*0.1+0.1])
        
        for i in range(1,4): # radius: 0.1*i
            self._np += np_list[i-1]
            for j in range(np_list[i-1]):
                r = 0.1*i
                angle = 2*math.pi * random.random()
                pos_list.append([r * math.cos(angle), r * math.sin(angle)])
                
        self._gamma_1 = gamma_1;
        self._gamma_2 = gamma_2;
        self.affinity_matrix = np.zeros([self._np, self._np])
        self.pos_list = pos_list
        for s_i in range(len(pos_list)):
            for s_j in range(s_i+1,len(pos_list)):
                x_1,y_1 = pos_list[s_i]
                x_2,y_2 = pos_list[s_j]
                r_1, phi_1 = cmath.polar(complex(x_1, y_1))
                r_2, phi_2 = cmath.polar(complex(x_2, y_2))                
                self.affinity_matrix[s_i, s_j] = self.compute_similarity(r_1, phi_1, r_2, phi_2)
                self.affinity_matrix[s_j, s_i] = self.affinity_matrix[s_i, s_j]
                
    def compute_similarity(self, r_1, phi_1, r_2, phi_2):
        phi_distance = min(abs(phi_1 - phi_2), 2*math.pi-abs(phi_1 - phi_2))
        return math.exp(-1.0 * self._gamma_1 * math.pow(r_1 - r_2, 2) - self._gamma_2 * math.pow(phi_distance, 2))
        
    def run(self):
        g = InfoCluster(affinity = 'precomputed')
        g.fit(self.affinity_matrix, use_pdt = USE_PDT)
        self.partition_num_list = g.partition_num_list
        self.critical_values = g.critical_values    
        self.get_category = g.get_category         
        self.g = g
    
class FourPart:
    def __init__(self, _np, _gamma=1):
        '''
        np is the number of points at each part
        '''
        #  (0, 0.1) to (0, 0.2)
        self._gamma = _gamma
        pos_list = []
        part_center = [[3,3],[3,-3],[-3,-3],[-3,3]]
        for i in range(4): # radius: 0.1*i
            for j in range(_np):
                x = part_center[i][0] + random.gauss(0,1) # standard normal distribution disturbance
                y = part_center[i][1] + random.gauss(0,1)                
                pos_list.append([x, y])
        self.pos_list = np.asarray(pos_list)
        
    def run(self):
        g = InfoCluster(gamma = self._gamma)
        g.fit(self.pos_list, use_pdt = USE_PDT)
        self.partition_num_list = g.partition_num_list
        self.critical_values = g.critical_values
        self.get_category = g.get_category        
        self.g = g
def check_cat(min_num, partition):
    '''
    return the index of partition whose first element is no smaller than min_num,
    '''
    for i in range(len(partition)):
        if(partition[i]>=min_num):
            break
    if(i>=len(partition)-2):
        return -1
    elif(partition[i+2] > MAX_CAT):
        return -2
    else:    
        return i
def plot_cluster(pos_list, cat, cat_num, axis):
    global color_list, marker_list
    p = np.asarray(pos_list)
    for i in range(cat_num):
        xx=[]
        yy=[]
        for j in range(len(cat)):
            if(cat[j]==i):
                xx.append(p[j,0])
                yy.append(p[j,1])
        axis.scatter(xx, yy, c=color_list[i], marker=marker_list[i])    
    
def plot_inner(index, grach_cluster_object, fileName):  
    '''
    Parameters
    ----------
    index: starting index whose element has at least required categories
    grach_cluster_object: instance of GraphCluster
    fileName: graph output file name
    '''
    p = grach_cluster_object.partition_num_list
    cv = grach_cluster_object.critical_values
    i = index

    lambda_list = [cv[i-1],cv[i],cv[i+1]]
    cat_num_list = [p[i], p[i+1], p[i+2]]    
    for index, cat_num in enumerate(cat_num_list):
        ax = plt.subplot(1,3,index+1)          
        cat = grach_cluster_object.get_category(cat_num)
        print('num of cat:', cat_num)
        plot_cluster(grach_cluster_object.pos_list, cat, cat_num, ax)
        if(index>0):
            plt.yticks(())
        ax.set_title('$\lambda = %.2f$' % lambda_list[index])
    if(SHOW_PIC):
        plt.show()
    plt.savefig(os.path.join(schema.BUILD_DIR, fileName))
  
def plot_FourPart():
    print('plot_FourPart')
    global color_list, marker_list, MAX_CAT
    i = -1
    while(i < 0): # check category requirement, regenerate the points if necessary
        g = FourPart(25, 0.6)
        g.run()    
        # divide into >=4 parts        
        i = check_cat(4, g.partition_num_list)
    plot_inner(i, g, '4part.eps')
    
def plot_ThreeCircle():
    print('plot_ThreeCircle')
    global color_list, marker_list, MAX_CAT
    i = -1
    while(i < 0): # check category requirement, regenerate the points if necessary
        gamma_2 = 1
        ratio = 2000
        g = ThreeCircle([60,100,140], ratio*gamma_2, gamma_2)
        g.run()    
        # divide into >=2 parts        
        i = check_cat(2, g.partition_num_list)
    plot_inner(i, g, '3circle.eps')

    
if __name__ == '__main__':
    parser = argparse.ArgumentParser()      
    parser.add_argument('--use_pdt', type=bool, help='use parameteric Dilworth Truncation implementation of info-cluster to draw',
        default=False, nargs='?', const=True)
    parser.add_argument('--show_pic', type=bool, help='whether to show the picture while program is running',
        default=False, nargs='?', const=True)
    parser.add_argument('--ignore_four_part', type=bool, help='ignore plotting four part case', default=False, nargs='?', const=True)
    parser.add_argument('--debug', type=bool, help='enter debug mode', default=False, nargs='?', const=True)
    args = parser.parse_args()
    USE_PDT = args.use_pdt
    SHOW_PIC = args.show_pic
    plt.figure(figsize=(9.2, 3))
    plt.subplots_adjust(wspace=.05)
    if(args.debug):
        pdb.set_trace()
    if not(args.ignore_four_part):
        plot_FourPart()
    plot_ThreeCircle()