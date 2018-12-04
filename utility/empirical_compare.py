# \author: zhaofeng-shu33
import sklearn
import graph_cluster
from sklearn import metrics
from sklearn import cluster

def Gaussian():
    NUM_EACH_PART = 25
    g = graph_cluster.FourPart(NUM_EACH_PART, 0.6)
    
    ground_truth = []
    for i in range(4):
        for j in range(NUM_EACH_PART):
            ground_truth.append(i)
            
    y_pred_kmeans = cluster.KMeans(n_clusters=4).fit_predict(g.pos_list)
    ars_kmeans = metrics.adjusted_rand_score(ground_truth, y_pred_kmeans)
    
    g.run()
    y_pred_ic = g.get_category(4)
    ars_ic = metrics.adjusted_rand_score(ground_truth, y_pred_ic)
    print(ars_kmeans, ars_ic)
        
if __name__ == '__main__':
    Gaussian()
