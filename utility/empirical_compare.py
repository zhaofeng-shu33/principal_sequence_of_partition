# \author: zhaofeng-shu33

# built-in module
import random
import json
import os
import logging
# third party module
import sklearn
from sklearn import metrics
from sklearn import cluster
from sklearn import datasets
from sklearn import preprocessing
from sklearn.model_selection import cross_validate
import numpy as np
from tabulate import tabulate

# user provided module
import info_cluster
from uci_glass import fetch_uci_glass
from uci_libras import fetch_uci_libras
import schema
# module level global variables go here

logging.basicConfig(filename=os.path.join(schema.BUILD_DIR, schema.EMPIRICAL_LOGGING_FILE), level=logging.INFO, format='%(asctime)s %(message)s')


def _k_means(feature, ground_truth, n_c):
    y_pred_kmeans = cluster.KMeans(n_clusters=n_c['nc']).fit_predict(feature)
    ars_kmeans = metrics.adjusted_rand_score(ground_truth, y_pred_kmeans)    
    return ars_kmeans

def _spectral_clustering(feature, ground_truth, n_c):
    c = cluster.SpectralClustering(n_clusters=n_c['nc'], affinity="nearest_neighbors", eigen_solver='arpack') # construct affinity matrix from rbf kernel function
    # cannot use cv since spectral clustering does not provide fitting method
    y_pred_sc = c.fit_predict(feature)
    sc = metrics.adjusted_rand_score(ground_truth, y_pred_sc)
    return sc

def _info_clustering(feature, ground_truth, parameter):
    g = info_cluster.InfoCluster(gamma = parameter['gamma'], affinity = parameter['affinity'], n_neighbors = parameter['n_neighbors'])
    y_pred_ic = g.get_category(parameter['nc'], feature)
    sc = metrics.adjusted_rand_score(ground_truth, y_pred_ic)
    return sc

def _affinity_propagation(feature, ground_truth, p_d):
    p = p_d['preference']
    d = p_d['damping_factor']
    af = cluster.AffinityPropagation(preference=p, damping=d).fit(feature)
    y_pred_af = af.labels_
    ars_af = metrics.adjusted_rand_score(ground_truth, y_pred_af)
    return ars_af
    
def _Agglomerative(feature, ground_truth, config):
    n_c = config['nc']
    _linkage = config['linkage']
    c = cluster.AgglomerativeClustering(n_clusters=n_c, linkage = _linkage)
    y_pred_sc = c.fit_predict(feature)
    sc = metrics.adjusted_rand_score(ground_truth, y_pred_sc)
    return sc    
    
def compute_adjusted_rand_score(feature, ground_truth, parameter_dic):
    dic = {}    
    for method, parameter in parameter_dic.items():
        _method = method.replace('-','_')
        exec('ari = _{0}(feature, ground_truth, parameter)'.format(_method))
        _ari = locals()['ari']
        dic[method] = _ari
        logging.info('%s ari is %.3f' % (method, _ari))    
    return dic
    
def Gaussian(parameter_dic):
    data = np.load('Gaussian.npx')
    pos_list = data[:,:2]
    ground_truth = data[:,-1]
    return compute_adjusted_rand_score(pos_list, ground_truth, parameter_dic)
    
    
def Circle(parameter_dic):
    data = np.load('Circle.npx')
    pos_list = data[:,:2]
    ground_truth = data[:,-1]
    return compute_adjusted_rand_score(pos_list, ground_truth, parameter_dic)
        
def Iris(parameter_dic):
    feature, ground_truth = datasets.load_iris(return_X_y = True)
    return compute_adjusted_rand_score(feature, ground_truth, parameter_dic)    

def Glass(parameter_dic):
    feature, ground_truth = fetch_uci_glass()
    feature = preprocessing.scale(feature)
    return compute_adjusted_rand_score(feature, ground_truth, parameter_dic)

def Libras(parameter_dic):
    feature, ground_truth = fetch_uci_libras()
    feature = preprocessing.scale(feature)
    return compute_adjusted_rand_score(feature, ground_truth, parameter_dic)
    
def compute():
    json_str = schema.get_file(schema.PARAMETER_FILE)
    p_dic = json.loads(json_str)
    dic = {}
    for key in p_dic:
        logging.info('Start computing for dataset %s'%key)
        exec('dic["{0}"] = {0}(p_dic["{0}"])'.format(key))
    return dic

def make_table(dic):
    table = [[i] for i in schema.METHOD_SCHEMA]
    for i in table:
        for _, v in dic.items():
            i.append('%.2f'%v.get(i[0]))
    _headers = ['adjusted rand index']
    _headers.extend(list(dic.keys()))
    for i in table:
        i[0] = i[0].replace('_', ' ')
    latex_table_string = tabulate(table, headers = _headers, tablefmt = 'latex_raw')
    schema.set_file(schema.LATEX_TABLE_NAME, latex_table_string)
    
if __name__ == '__main__':
    make_table(compute())
