# generate lgf files to be used by main.
import argparse
from sklearn import datasets
from sklearn.metrics.pairwise import pairwise_kernels
def convert(filename, affinity_matrix):
    Ls = ['@nodes', 'label']
    ms = affinity_matrix.shape[0]
    for i in range(ms):
        Ls.append(str(i))
    Ls.append('@arcs')
    Ls.append('\t\tlabel\tcapacity')
    edge_cnt = 0
    for i in range(ms):
        for j in range(i+1, ms):
            w = affinity_matrix[i,j]
            if(w < 1e-10):
                continue
            Ls.append('\t'.join([str(i), str(j), str(edge_cnt), str(w)]))
            edge_cnt += 1
    with open(filename.replace('gml','lgf'),'w') as f:
        f.write('\n'.join(Ls))
        
if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--size', default=8, type=int)
    parser.add_argument('--gamma', default=0.6, type=float)
    
    args = parser.parse_args()
    if(args.size % 4 != 0):
        print('size should be multiple of 4')
        exit(0)    
    pos_list, _ = datasets.make_blobs(n_samples = args.size, centers=[[3,3],[-3,-3],[3,-3],[-3,3]], cluster_std=1)
    affinity_matrix = pairwise_kernels(pos_list, metric='rbf', gamma = args.gamma)
    filename = 'build/gaussian-%d-%.2f.lgf' %(args.size, args.gamma)
    convert(filename, affinity_matrix)