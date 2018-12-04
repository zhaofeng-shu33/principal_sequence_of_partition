# \author: zhaofeng-shu33
'''
    plot artificial dataset
'''
from matplotlib import pyplot as plt
import graph_cluster

color_list = ['#3FF711', 'r', 'g', 'm','y','k','c','#00FF00']
marker_list = ['o', 'v', 's', '*', '+', 'x', 'D', '1']
MAX_CAT = len(color_list)
def check_cat(min_num, partition):
    '''
    return the index of partition whose first element is no smaller than min_num,
    '''
    for i in range(len(partition)):
        if(partition[i]>=min_num):
            break
    if(i>=len(partition)-2):
        return -1
    elif(partition[i+2] > MAX_CAT):
        return -2
    else:    
        return i

def plot_inner(index, grach_cluster_object, fileName):  
    '''
    Parameters
    ----------
    index: starting index whose element has at least required categories
    grach_cluster_object: instance of GraphCluster
    fileName: graph output file name
    '''
    p = grach_cluster_object.partition_num_list
    cv = grach_cluster_object.critical_values
    i = index

    lambda_list = [cv[i-1],cv[i],cv[i+1]]
    cat_num_list = [p[i], p[i+1], p[i+2]]    
    for index, cat_num in enumerate(cat_num_list):
        ax = plt.subplot(1,3,index+1)          
        cat = grach_cluster_object.get_category(cat_num)
        print('num of cat:', cat_num)
        for i in range(cat_num):
            xx=[]
            yy=[]
            for j in range(len(cat)):
                if(cat[j]==i):
                    xx.append(grach_cluster_object.pos_list[j][0])
                    yy.append(grach_cluster_object.pos_list[j][1])
            plt.scatter(xx,yy, c=color_list[i], marker = marker_list[i])
        ax.set_title('$\lambda = %.2f$' % lambda_list[index])
    plt.savefig(fileName)
    plt.show()
  
def plot_FourPart():
    global color_list, marker_list, MAX_CAT
    i = -1
    while(i < 0): # check category requirement, regenerate the points if necessary
        g = graph_cluster.FourPart(25, 0.6)
        g.run()    
        # divide into >=4 parts        
        i = check_cat(4, g.partition_num_list)
    plot_inner(i, g, '4part.eps')
    
def plot_ThreeCircle():
    global color_list, marker_list, MAX_CAT
    i = -1
    while(i < 0): # check category requirement, regenerate the points if necessary
        gamma_2 = 1
        ratio = 2000
        g = graph_cluster.ThreeCircle([60,100,140], ratio*gamma_2, gamma_2)
        g.run()    
        # divide into >=4 parts        
        i = check_cat(2, g.partition_num_list)
    plot_inner(i, g, '3circle.eps')

    
if __name__ == '__main__':
    #plot_FourPart()
    plot_ThreeCircle()