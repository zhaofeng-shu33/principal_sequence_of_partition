# \brief: use python unittest framework to check the rightness of psp exported class PyGraph
import math
import unittest
import psp
from graph_cluster import GraphCluster,to_py_list

        
def construct_pos_list(x_pos_list, y_pos_list):
    return [[x_pos_list[i], y_pos_list[i]] for i in range(len(x_pos_list))]

class TestPyGraph(unittest.TestCase):
    def test_4point(self):
        gamma = 1
        g = psp.Gaussian2DGraph(4,gamma)
        pos_list = construct_pos_list(g.get_x_pos_list(), g.get_y_pos_list())
        # Method 1
        g.run(False)        
        cv_list = to_py_list(g.get_critical_values())
        p_list = to_py_list(g.get_partitions())
        cat_2_list = to_py_list(g.get_category(2))
        # Method 2
        g2 = GraphCluster(4, gamma, pos_list)
        g2.run()
        # assert
        self.assertEqual(cat_2_list, g2.get_category(2))
        self.assertEqual(cv_list, g2.critical_values)
        self.assertEqual(p_list, g2.partition_num_list)

    def test_8point(self):
        gamma = 0.6
        num_points = 8
        g = psp.Gaussian2DGraph(num_points,gamma)
        pos_list = construct_pos_list(g.get_x_pos_list(), g.get_y_pos_list())
        # Method 1
        g.run(False)
        cv_list = to_py_list(g.get_critical_values())
        p_list = to_py_list(g.get_partitions())
        cat_2_list = to_py_list(g.get_category(4))
        # Method 2
        g2 = GraphCluster(num_points, gamma, pos_list)
        g2.run()
        # assert
        self.assertEqual(cat_2_list, g2.get_category(4))
        self.assertEqual(cv_list, g2.critical_values)
        self.assertEqual(p_list, g2.partition_num_list)        
if __name__ == '__main__':
    unittest.main()