import random
import math
import cmath
import numpy as np
import psp # [package] principal sequence of partition
from sklearn.metrics.pairwise import pairwise_kernels
class InfoCluster:
    '''Info clustering is a kind of hierarchical clustering method.
    It uses top down approach to build the hierarchical tree.
    
    Parameters
    ----------
    gamma : float, default=1.0
        Kernel coefficient for rbf kernels.
    affinity : string, default 'rbf'
        may be one of 'precomputed', 'rbf'.        
    '''
    def __init__(self, gamma=1, affinity='rbf'):    
        self._gamma = gamma;
        self.affinity = affinity
    def fit(self, X):
        '''Construct an affinity graph from X using rbf kernel function,
        then applies info clustering to this affinity graph.
        Parameters
        ----------
        X : array-like, shape (n_samples, n_features)
           
        '''
        n_samples = X.shape[0]
        if(self.affinity == 'precomputed'):
            affinity_matrix = X
        else:
            affinity_matrix = pairwise_kernels(X, metric='rbf', gamma = self._gamma)
            
        sim_list = []
        for s_i in range(n_samples):
            for s_j in range(s_i+1, n_samples):
                sim_list.append((s_i, s_j, affinity_matrix[s_i, s_j]))       

        self.g = psp.PyGraph(n_samples, sim_list)
        self.g.run(False)
        
        self.critical_values = to_py_list(self.g.get_critical_values())
        self.partition_num_list = to_py_list(self.g.get_partitions())    
    def get_category(self, i):
        return to_py_list(self.g.get_category(i))        
        
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
        g.fit(self.affinity_matrix)
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
        g.fit(self.pos_list)
        self.partition_num_list = g.partition_num_list
        self.critical_values = g.critical_values
        self.get_category = g.get_category        
        self.g = g
def to_py_list(L):
    '''
    convert an iterable object (exported from C++ class) to python list        
    '''
    return [i for i in L]