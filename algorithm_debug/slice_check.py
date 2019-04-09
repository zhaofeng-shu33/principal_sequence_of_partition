# check the slice function.
import numpy as np
np.set_printoptions(precision=3, suppress=True)
import pdb
GAMMA = 0.1
DATA = [[3.1, 3.2],
    [4.0, 4.0 ],
    [1.1, -2.2],
    [3.9, -2.0],
    [-3.9, -2.0],
    [-2.2, -3.5],
    [-3.9, 2.4],
    [-3.1, 2.6]]
# id = 8, sink node
# id = 2, source node

SOURCE_NODE_ID = 8
SINK_NODE_ID = 2
WHOLE_SET = set(range(9))
y_lambda = [[0, -0.0286] for i in range(8)]
y_lambda[0] = [0.993, -0.737]
y_lambda[1] = [0.909, 1e5]
def compute_similarity(x_1, y_1, x_2, y_2):
    return np.exp(-GAMMA*(np.power(x_1 - x_2, 2) + np.power(y_1 - y_2, 2)))
def construct_weight_matrix(_lambda):
    arr = np.zeros((9, 9))
    for i in range(8):
        for j in range(8):
            if(i != j):
                arr[i,j] = compute_similarity(DATA[i][0], DATA[i][1], DATA[j][0], DATA[j][1])
    i = 0
    while(i != SOURCE_NODE_ID):
        a, b = y_lambda[i]
        if(i != SINK_NODE_ID):
            arr[SOURCE_NODE_ID, i] = np.max([0, -1*np.min([a - 2 * _lambda, b])])
            arr[i, SINK_NODE_ID] = arr[i, SINK_NODE_ID] + np.max([0, np.min([a - 2 * _lambda, b])])
        i += 1
    return arr

def compute_cut(weight_matrix, sink_set = None, source_set = None):
    if(source_set is None and sink_set is not None):
        source_set = WHOLE_SET - sink_set
    elif(sink_set is None and source_set is not None):
        sink_set = WHOLE_SET - source_set
    else:
        raise(Exception('either source set or sink set should be nonempty'))
    cut_value = 0
    for s in source_set:
        for t in sink_set:
            cut_value += weight_matrix[s,t]
    return cut_value
    
if __name__ == '__main__':
    T = set([2,3])
    S = set([4,5,6,7,8])
    wm = construct_weight_matrix(0.089)
    print(compute_cut(wm, sink_set = T) - compute_cut(wm, source_set = S)) 
    wm = construct_weight_matrix(0.15)
    print(compute_cut(wm, sink_set = T) - compute_cut(wm, source_set = S))
    # different sign
    
    
    
