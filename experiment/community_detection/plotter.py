'''load pickle data to plot
'''
import os
import pickle
import argparse
import pdb

import matplotlib.pyplot as plt

def load_other_data(filename, alg, other_alg):
    new_filename = filename.replace(alg, other_alg)
    if(os.path.exists(os.path.join('build', new_filename))):
        f = open(os.path.join('build', new_filename), 'rb')
        data = pickle.load(f)
        return [i['norm_rf'] for i in data]
    return False

def plot_ari(filename, plot_title=''):
    '''combine different algorithms
    '''
    f = open(os.path.join('build', filename), 'rb')
    data = pickle.load(f)
    if data[0]['z_in_1'] != data[1]['z_in_1']:
        x_title = 'z_in_1'
    elif data[0]['z_in_2'] != data[1]['z_in_2']:
        x_title = 'z_in_2'
    else:
        x_title = 'z_o'
        
    if(filename.find('info-clustering')>0):
        alg = 'info-clustering'
        other_alg = 'gn'
    elif(filename.find('gn')>0):
        alg = 'gn'
        other_alg = 'info-clustering'
        
    x_data = [i[x_title] for i in data]
    outer_ari_data = [i['norm_rf'] for i in data]
    plt.plot(x_data, outer_ari_data, label=alg)
    data_2 = load_other_data(filename, alg, other_alg)
    if(data_2):
        plt.plot(x_data, data_2, label=other_alg)
    plt.xlabel(x_title)
    plt.ylabel('norm rf')
    if(x_title == 'z_o'):
        title_str = 'z_in_1 = %.2f, z_in_2 = %.2f' % (data[0]['z_in_1'], data[0]['z_in_2'])
    elif(x_title == 'z_in_1'):
        title_str = 'z_in_2 = %.2f, z_o = %.2f' % (data[0]['z_in_2'], data[0]['z_o'])
    else:
        title_str = 'z_in_1 = %.2f, z_o = %.2f' % (data[0]['z_in_1'], data[0]['z_o'])
       
    plt.title('Comparision of Algorithm at ' + title_str)
    plt.legend()
    plt.savefig(os.path.join('build', filename.replace('pickle','svg')))    
    plt.show()


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('filename', help='pickle file to load')
    parser.add_argument('--debug', default=False, type=bool, nargs='?', const=True, help='whether to enter debug mode') 
    args = parser.parse_args()
    if(args.debug):
        pdb.set_trace()
    plot_ari(args.filename)    
