# This example uses relaxed caveman graph ( a generated graph model commonly used in the study of social community)
# In this graph clustering problem, when the drifting parameter p is small. Spectral Clustering algorithm can recover
# the structure of the original graph while info-clustering is too sensentive and cannot recover the original graph.
# the adjusted rand score shows this result.
# Lowing p to 0.07 can achieve ari=1 for info-clustering, p=0.1 cannot.
from info_cluster import InfoCluster
import random
import numpy as np
import networkx as nx
from scipy.sparse import csr_matrix
from sklearn.cluster import SpectralClustering
from sklearn import metrics
import nxviz as nv
import matplotlib.pyplot as plt
from sklearn import metrics
import argparse
def plot(graph, labels):
    nodelist = []
    label_dic = {}
    for i in labels:
        label_dic[i] = 0
    label_cnt = len(label_dic.keys())
    for i in range(label_cnt):
        for n in rcg.nodes():
            if(labels[n] == i):
                nodelist.append(n)
    ssm = nx.adjacency_matrix(graph)
    plt.matshow(csr_matrix.todense(ssm))
    plt.show()
if __name__ == '__main__':    
    parser = argparse.ArgumentParser()
    parser.add_argument('--plot',default=False, type=bool, nargs='?', const=True)     
    args = parser.parse_args()
    ic = InfoCluster(affinity = 'precomputed')
    sc = SpectralClustering(affinity='precomputed')

    rcg = nx.relaxed_caveman_graph(8, 11, 0.07)
    am = nx.adjacency_matrix(rcg)
    am_d = np.asarray(csr_matrix.todense(am), dtype=float)
    cnt_non_diag = 0
    w, h = am.nonzero()
    for i in range(len(w)):
        if(np.abs(w[i]-h[i])>=11):
            cnt_non_diag += 1
    print('non diag element cnt', cnt_non_diag)        
    tree_labels = [i for i in range(8) for j in range(11)]
    ic.fit(am_d)
    print(ic.partition_num_list)
    ic_labels = ic.get_category(5)
    #ic.print_hierachical_tree()
    print(ic_labels)
    print('ari', metrics.adjusted_rand_score(tree_labels, ic_labels))
    if(args.plot):
        plot(rcg, ic_labels)
    sc.fit(am)
    print(sc.labels_)
    print('ari', metrics.adjusted_rand_score(tree_labels, sc.labels_))
    if(args.plot):
        plot(rcg, sc.labels_)

