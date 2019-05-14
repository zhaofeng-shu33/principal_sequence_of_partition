'''load pickle data to plot
'''
import os
import pickle
import argparse
import pdb

import matplotlib.pyplot as plt


def plot_ari(filename, plot_title=''):
    f = open(os.path.join('build', filename), 'rb')
    data = pickle.load(f)
    is_old_format = True
    if(type(data[0][0]) == float):
        is_old_format = False
    if(is_old_format):
        if data[0][0] != data[1][0]:
            x_title = 'z_in_1'
        elif data[0][1] != data[1][1]:
            x_title = 'z_in_2'
        else:
            x_title = 'z_o'
        x_data = [i[3][x_title] for i in data]
        outer_ari_data = [i[3]['outer_ari'] for i in data]
        inner_ari_data = [i[3]['inner_ari'] for i in data]        
    else:
        if data[0]['z_in_1'] != data[1]['z_in_1']:
            x_title = 'z_in_1'
        elif data[0]['z_in_2'] != data[1]['z_in_2']:
            x_title = 'z_in_2'
        else:
            x_title = 'z_o'
        x_data = [i[x_title] for i in data]
        outer_ari_data = [i['outer_ari'] for i in data]
        inner_ari_data = [i['inner_ari'] for i in data]
    plt.plot(x_data, outer_ari_data, label='macro')
    plt.plot(x_data, inner_ari_data, label='micro')
    plt.xlabel(x_title)
    plt.ylabel('ari')
    plt.title('Comparision of Algorithm at z = , z = ')
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
