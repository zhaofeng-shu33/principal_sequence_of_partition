'''
In this experiment, we prepare a graph with two level hierachical structures.
The graph has k2 macro-communities and in each macro-community there are k1 micro-communities. Each micro-community
contains n nodes. There are also two parameters p1 and p2 which control the inter-community linkage respectively.
Ideally, when p2=0, the graph has k2 connected components and when p2=1, the graph is random at macro-community level.
p1 controls the micro-community linkage in the same manner.
Notice that p1 cannot be zero, otherwise the two-level structure of the graph does not exist.
By using info-clustering algorithm we try to recover the original graph with 
partition_num_list = [1, k2, k2*k1, n*k2*k1] under certain conditions of p1 and p2.

We also use other clustering algorithm which does not require predetermined cluster number. 
For example, Girvan-Newman algorithm and Affinity Propogation. There algorithms can get the macro-community first
and we rerun the algorithm to get the micro-community structures.

As we can see, if the graph has deep hierachical structure, info-cluster has advantages since we only need to run ic algorithm once.
'''
import random
import argparse
from datetime import datetime
import pdb
import logging
import os
import json

import numpy as np
import networkx as nx # for manipulating graph data-structure
import graphviz # for writing .gv file
from sklearn.metrics import adjusted_rand_score

from info_cluster import InfoCluster
from cmty import GN

logging.basicConfig(filename=os.path.join('build', 'two_level.log'), level=logging.INFO, format='%(asctime)s %(message)s')

n = 16
k1 = 4 # inner
k2 = 4 # outer
K = 18
color_list = ['red', 'orange', 'green', 'purple']
ground_truth_outer = []
ground_truth_inner = []

def modify_edge_weight(G, weight_method='triangle-power'):
    '''
        G: networkx like graph
        Notice the graph object G is modified by this function.
    '''
    if(weight_method=='triangle-power'):
        # for each edge, the weight equals the number of triangles + beta(default to 1)
        beta = 1
        for e in G.edges():
            i, j = e
            G[i][j]['weight'] = beta
            for n in G.nodes():
                if(G[i].get(n) is not None and G[j].get(n) is not None):
                    G[i][j]['weight'] += 1
            G[i][j]['weight'] = G[i][j]['weight']
            
def evaluate(num_times, alg):
    '''
        num_times: int
        alg: algorithm class
        
        the evaluated alg is a class, and should provide fit method , which operates on similarity matrix
        and get_category(i) method, where i is the specified category.
    '''
    report = {'outer_ari' : 0.0,
              'inner_ari' : 0.0,
              'depth': 0,
              'recover_percentage': 0.0,
              'num_times': num_times
             }
    logging.info('eval ' + str(type(alg)) + ' num_times=%d'%num_times)
    for i in range(num_times):
        G = construct(args.z_in_1, args.z_in_2, z_o)    
        modify_edge_weight(G, args.weight)
        alg.fit(G)
        out_ari = adjusted_rand_score(ground_truth_outer, alg.get_category(k2))
        inner_ari = adjusted_rand_score(ground_truth_inner, alg.get_category(k2 * k1))
        report['outer_ari'] += out_ari
        report['inner_ari'] += inner_ari
        if(out_ari > 0.99 and inner_ari > 0.99):
            report['recover_percentage'] += 1.0
        depth = alg.get_tree_depth()
        logging.info('round {0}: with o_ari={1}, i_ari={2} and depth={3}'.format(i, out_ari, inner_ari, depth))
        report['depth'] += depth
    for k in report.keys():
        report[k] /= num_times
    return report
    
def construct(z_in_1, z_in_2, z_out):
    '''
       p2: type float, percentage of edges to be added at macro level.
       p1: type float, percentage of edges to be added at micro level.
    '''
    global n,k1,k2
    p_1 = z_in_1/(n-1)
    
    assert(p_1 <= 1)
    assert(z_out > 0)
    
    p_2 = z_in_2/(n*(k1-1))
    p_o = z_out/(n*k1*(k2-1))
    G = nx.Graph()
    cnt = 0
    for t in range(k2):
        for i in range(k1):
            for j in range(n):
                G.add_node(cnt, macro=t, micro=i)
                if(len(ground_truth_inner) != n*k1*k2):
                    ground_truth_outer.append(t)
                    ground_truth_inner.append(i+k2*t)
                cnt += 1
    for i in G.nodes(data=True):
        for j in G.nodes(data=True):
            if(j[0] <= i[0]):
                continue
            if(i[1]['macro'] != j[1]['macro']):
                if(random.random()<=p_o):
                    G.add_edge(i[0], j[0])
            else:
                if(i[1]['micro'] == j[1]['micro']):
                    if(random.random() <= p_1):
                        G.add_edge(i[0], j[0])
                else:
                    if(random.random() <= p_2):
                        G.add_edge(i[0], j[0])
    return G    
    
def graph_plot(G):
    '''
    generate the plot file which is the input of graphviz.
    G: networkx graph object
    '''
    global n, k1, k2
    time_str = datetime.now().strftime('%Y-%m-%d')
    g = graphviz.Graph(filename='two_level-%s.gv'%time_str, engine='neato') # g is used for plotting
    for i in G.nodes(data=True):
        macro_index = i[1]['macro']
        g.node(str(i[0]), shape='point', color=color_list[macro_index])
    for e in nx.edges(G):
        i,j = e
        i_attr = G.node[i]
        j_attr = G.node[j]
        if(i_attr['macro'] != j_attr['macro']):
            edge_len = 2
            weight_value = 0.1
        elif(i_attr['micro'] != j_attr['micro']):
            weight_value = 1
            edge_len = 1
        else:
            weight_value = 10
            edge_len = 0.5
        g.edge(str(i), str(j), weight=str(weight_value), penwidth="0.3", len=str(edge_len))    
    g.save(directory='fig')    
    
if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--save_graph', default=False, type=bool, nargs='?', const=True, help='whether to save the .gv file') 
    parser.add_argument('--ic', default=False, type=bool, nargs='?', const=True, help='whether to run info-cluster algorithm')
    parser.add_argument('--weight', default='triangle-power', help='for info-clustering method, the edge weight shold be used. This parameters'
        ' specifies how to modify the edge weight.')    
    parser.add_argument('--gn', default=False, type=bool, nargs='?', const=True, help='whether to run Girvan-Newman algorithm')                  
    parser.add_argument('--z_in_1', default=14.0, type=float, help='inter-micro-community node average degree')      
    parser.add_argument('--z_in_2', default=3.0, type=float, help='intra-micro-community node average degree')          
    parser.add_argument('--z_o', default=-1, type=float, help='intra-macro-community node average degree')              
    parser.add_argument('--debug', default=False, type=bool, nargs='?', const=True, help='whether to enter debug mode')                  
    parser.add_argument('--evaluate', default=0, type=int, help='whether to evaluate the method instead of run once')                      
    args = parser.parse_args()
    if(args.debug):
        pdb.set_trace()
    if(args.z_o == -1):
        z_o = K - args.z_in_1 - args.z_in_2
    else:
        z_o = args.z_o
    G = construct(args.z_in_1, args.z_in_2, z_o)    
    if(args.save_graph):
        graph_plot(G)
    if(args.evaluate > 0):
        methods = []
        if(args.ic):
            methods.append(InfoCluster(affinity='precomputed'))
        if(args.gn):
            methods.append(GN())
        for method in methods:
            report = evaluate(args.evaluate, method)
            logging.info('final report' + json.dumps(report))
    else:
        if(args.ic):           
            print('use info-clustering algorithm...')
            alg = InfoCluster(affinity='precomputed')        
            modify_edge_weight(G, args.weight)
        if(args.gn):
            print('use Girvan-Newman algorithm...')        
            alg = GN()
        else:
            print('at least --ic or --gn should be provided')
            exit(0)
        alg.fit(G)
        print(alg.partition_num_list)
