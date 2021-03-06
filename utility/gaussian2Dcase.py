# generate lgf files to be used by main.
import argparse
from sklearn import datasets
from sklearn.metrics.pairwise import pairwise_kernels
import networkx as nx
import pdb
def toNetworkX(filename):
    dot = nx.DiGraph()
    with open(filename) as f:
        st = f.readline()
        st = f.readline()
        while(True):
            if(st.find('@arcs')>=0):
                break
            if(st.find('label')>=0):
                st = f.readline()
                continue
            node_label = st.strip()
            dot.add_node(node_label)
            st = f.readline()
        st = f.readline()
        while(len(st)>0):
            if(st.find('@attributes')>=0):
                break
            if(st.find('label')>=0):
                st = f.readline()
                continue
            s,t,_,c = st.strip().split('\t')
            dot.add_edge(s,t,label=str(c))
            st = f.readline()
        if(st.find('source')>=0):
            _,s_id = st.strip().split(' ')
            dot.nodes[s_id]['label'] = 's'
        st = f.readline()
        if(st.find('target')>=0):
            _,t_id = st.strip().split(' ')
            dot.nodes[t_id]['label'] = 't'
    return dot
    
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