from info_cluster import InfoCluster
import networkx as nx
if __name__ == '__main__':
    g = nx.Graph() # undirected graph
    g.add_edge(0, 1, weight=1)
    g.add_edge(1, 2, weight=1)
    g.add_edge(0, 2, weight=5)
    ic = InfoCluster(affinity='precomputed') # use precomputed graph structure
    ic.fit(g)
    ic.print_hierachical_tree()