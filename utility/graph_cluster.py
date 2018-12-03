import random
import math
import psp # [package] principal sequence of partition 
class GraphCluster:
    def __init__(self, np, gamma=1, pos_list = []):
        self._gamma = gamma;
        self._np = np;
        if(pos_list!=[]):
            self.pos_sim_list = [];
            for s_i in range(len(pos_list)):
                for s_j in range(s_i+1,len(pos_list)):
                    x_1,y_1 = pos_list[s_i]
                    x_2,y_2 = pos_list[s_j]
                    sim = self.compute_similarity(x_1, y_1, x_2, y_2)
                    self.pos_sim_list.append((s_i, s_j, sim))
        else:
            raise Exception("pos_list is empty")
    def run(self):
        self.g = psp.PyGraph(self._np,self.pos_sim_list, self._gamma)
        self.g.run(False)
        self.critical_values = to_py_list(self.g.get_critical_values())
        self.partition_num_list = to_py_list(self.g.get_partitions())
    def get_category(self, i):
        return to_py_list(self.g.get_category(i))        
    def compute_similarity(self, x_1, y_1, x_2, y_2):
        return math.exp(-1.0 * self._gamma* math.pow(x_1 - x_2, 2) / 2 - self._gamma * math.pow(y_1 - y_2, 2) / 2)
        
class ThreeCircle(GraphCluster):
    def init(self, np, gamma=1):
        '''
        np is the number of points at each circle
        '''
        #  (0, 0.1) to (0, 0.2)
        pos_list = []
        for i in range(10):
            pos_list.append([0, random.random()*0.1+0.1])
        for i in range(1,4): # radius: 0.1*i
            for j in range(np):
                r = 0.1*i
                angle = 2*math.pi * random.random()
                pos_list.append([r * math.cos(angle), r * math.sin(angle)])     
        super(ThreeCircle, self).__init__(3*np + 10, gamma, pos_list)
        
class FourPart(GraphCluster):
    def init(self, np, gamma=1):
        '''
        np is the number of points at each part
        '''
        #  (0, 0.1) to (0, 0.2)
        pos_list = []
        part_center = [[3,3],[3,-3],[-3,-3],[-3,3]]
        for i in range(4): # radius: 0.1*i
            for j in range(np):
                x = part_center[i][0] + random.gauss(0,1) # standard normal distribution disturbance
                y = part_center[i][1] + random.gauss(0,1)                
                pos_list.append([x, y])     
        super(FourPart, self).__init__(4*np, gamma, pos_list)
        
def to_py_list(L):
    '''
    convert an iterable object (exported from C++ class) to python list        
    '''
    return [i for i in L]