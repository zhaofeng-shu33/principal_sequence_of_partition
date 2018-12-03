# \brief: use python unittest framework to check the rightness of psp exported class PyGraph
import math
import unittest
import psp

class gaussian2Dcase:
    def __init__(self, np, gamma=1, pos_list = []):
        self._gamma = gamma;
        if(pos_list!=[]):
            self.pos_sim_list = [];
            for s_i in range(len(pos_list)):
                for s_j in range(s_i+1,len(pos_list)):
                    x_1,y_1 = pos_list[s_i]
                    x_2,y_2 = pos_list[s_j]
                    sim = compute_similarity(x_1, y_1, x_2, y_2)
                    self.pos_sim_list.append((s_i, s_j, sim))
        else:
            raise("pos_list is empty")
    def run(self):
        g = psp.PyGraph(self.pos_sim_list, gamma)
        self.critical_values = g.get_critical_values()
        self.partation_num_list = g.get_partations()
        self.category_list = g.get_category()
        
def compute_similarity(x_1, y_1, x_2, y_2):
    return math.exp(-1.0 * self._gamma* math.pow(x_1 - x_2, 2) / 2 - self._gamma * math.pow(y_1 - y_2, 2) / 2);
        
def construct_pos_list(x_pos_list, y_pos_list):
    return [[x_pos_list[i], y_pos_list[i]] for i in range(len(x_pos_list))]

class TestPyGraph(unittest.TestCase):
    def test_4point(self):
        gamma = 1
        g = psp.Gaussian2DGraph(4,gamma)
        pos_list = construct_pos_list(g.get_x_pos_list(), g.get_y_pos_list())
        # Method 1
        g.run()
        cat_list = g.get_category()
        cv_list = g.get_critical_values()
        p_list = g.get_partations()
        # Method 2
        g2 = gaussian2Dcase(4, gamma, pos_list)
        g2.run()
        # assert
        self.assertEqual(cat_list, g2.category_list)
        self.assertEqual(cv_list, g2.critical_values)
        self.assertEqual(p_list, g2.partation_num_list)
        
if __name__ == '__main__':
    unittest.main()