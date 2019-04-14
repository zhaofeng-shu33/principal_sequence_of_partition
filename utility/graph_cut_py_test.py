# \brief: use python unittest framework to check the rightness of psp exported class PyGraph
import math
import unittest
import psp
import numpy as np
from info_cluster import InfoCluster,to_py_list

        
def construct_pos_list(x_pos_list, y_pos_list):
    return np.asarray([[x_pos_list[i], y_pos_list[i]] for i in range(len(x_pos_list))])

class TestPyGraph(unittest.TestCase):
    def test_4point(self):
        _gamma = 1
        g = psp.Gaussian2DGraph(4, _gamma)
        pos_list = construct_pos_list(g.get_x_pos_list(), g.get_y_pos_list())
        # Method 1
        g.run()        
        cv_list = to_py_list(g.get_critical_values())
        p_list = to_py_list(g.get_partitions())
        cat_1_list = to_py_list(g.get_category(2))
        # Method 2
        info_cluster = InfoCluster(gamma = _gamma)
        info_cluster.fit(np.asarray(pos_list))
        cat_2_list = info_cluster.get_category(4)
        # assert
        total_diff = get_total_diff(cat_1_list, cat_2_list)
        self.assertTrue(total_diff<1e-5)
        
        total_diff = get_total_diff(cv_list, info_cluster.critical_values)
        self.assertTrue(total_diff<1e-5)
        
        self.assertEqual(p_list, info_cluster.partition_num_list)

    def test_8point(self):
        _gamma = 0.6
        num_points = 8
        g = psp.Gaussian2DGraph(num_points, _gamma)
        pos_list = construct_pos_list(g.get_x_pos_list(), g.get_y_pos_list())
        # Method 1
        g.run()
        cv_list = to_py_list(g.get_critical_values())
        p_list = to_py_list(g.get_partitions())
        cat_1_list = to_py_list(g.get_category(4))
        # Method 2
        info_cluster = InfoCluster(gamma=_gamma)
        info_cluster.fit(pos_list)
        cat_2_list = info_cluster.get_category(4)
        
        # assert
        total_diff = get_total_diff(cat_1_list, cat_2_list)
        self.assertTrue(total_diff<1e-5)
        
        total_diff = get_total_diff(cv_list, info_cluster.critical_values)
        self.assertTrue(total_diff<1e-5)

        self.assertEqual(p_list, info_cluster.partition_num_list)

    def test_12point(self):
        _gamma = 0.6
        num_points = 12
        g = psp.Gaussian2DGraph(num_points, _gamma)
        pos_list = construct_pos_list(g.get_x_pos_list(), g.get_y_pos_list())
        # get the >=4 clusters solution using two method
        # Method 1
        g.run()
        cat_1_list = to_py_list(g.get_category(4))
        # Method 2
        info_cluster = InfoCluster(gamma=_gamma)
        cat_2_list = info_cluster.get_category(4, pos_list)
        
        # assert
        total_diff = get_total_diff(cat_1_list, cat_2_list)
        self.assertTrue(total_diff<1e-5)
        
def get_total_diff(L1, L2):
    total_diff = 0
    for i in range(len(L1)):
        total_diff += np.abs(L1[i] - L2[i])
    return total_diff        
if __name__ == '__main__':
    unittest.main()