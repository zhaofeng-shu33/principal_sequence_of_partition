'''
    wrapper of Girvan-Newman community detection algorithm
    >>> import networkx as nx
    >>> G=nx.Graph()
    >>> G.add_edge(1,3)
    >>> G.add_edge(1,2)
    >>> G.add_edge(3,2)
    >>> G.add_edge(4,5)
    >>> G.add_edge(4,6)
    >>> G.add_edge(5,6)
    >>> G.add_edge(1,6)
    >>> from GN import GN
    >>> GN().fit(G).Bestcomps
    [{1, 2, 3}, {4, 5, 6}]    
'''
from cmty import cmty
import networkx as nx
import numpy as np
class GN:
    def __init__(self):
        self.partition_num_list = []
        self.partition_list = []
    def fit(self, G_outer):
        '''
            G_outer: nx.Graph like object
            returns the partition
        '''    
        self.G = G_outer.copy()
        G = G_outer.copy()# copy the graph
        n = G.number_of_nodes()    #|V|
        A = nx.adj_matrix(G)    #adjacenct matrix

        m_ = 0.0    #the weighted version for number of edges
        for i in range(0,n):
            for j in range(0,n):
                m_ += A[i,j]
        self.m_ = m_/2.0

        #calculate the weighted degree for each node
        Orig_deg = {}
        self.Orig_deg = cmty.UpdateDeg(A, G.nodes())

        #run Newman alg
        self.runGirvanNewman() 
        return self  
        
    def runGirvanNewman(self):
        #let's find the best split of the graph
        BestQ = 0.0
        Q = 0.0
        while True:    
            cmty.CmtyGirvanNewmanStep(self.G)
            partition = list(nx.connected_components(self.G))
            self.partition_num_list.append(len(partition))
            self.partition_list.append(partition)
            Q = cmty._GirvanNewmanGetModularity(self.G, self.Orig_deg, self.m_)
            if Q > BestQ:
                BestQ = Q
                Bestcomps = partition    #Best Split
            if self.G.number_of_edges() == 0:
                break
        if BestQ > 0.0:
            self.Bestcomps = Bestcomps
    def get_category(self, i):
        index = 0
        for ind,val in enumerate(self.partition_num_list):
            if(val >= i):
                index = ind
                break
        cat = np.zeros(len(self.Orig_deg))
        t = 0
        for j in self.partition_list[index]:
            for r in j:
                cat[r] = t
            t += 1
        return cat
        