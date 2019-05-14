'''load pickle data to plot
'''
import os
import pickle
import argparse

import matplotlib.pyplot as plt


def plot_ari(filename, plot_title=''):
    f = open(os.path.join('build', filename), 'wb')
    data = pickle.load(f)
    if data[0]['z_in_1'] != data[1]['z_in_1']:
        x_title = 'z_in_1'
    elif data[0]['z_in_2'] != data[1]['z_in_2']:
        x_title = 'z_in_2'
    else:
        x_title = 'z_o'
    x_data = [i[x_title] for i in data]
    outer_ari_data = [i['outer_ari'] for i in data]
    inner_ari_data = [i['inner_ari'] for i in data]
    plt.plot(x_data, outer_ari_data)
    plt.plot(x_data, inner_ari_data)
    plt.show()
    plt.savefig(filename.replace('pickle','svg'))


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('filename', help='pickle file to load')
    
    args = parser.parse_args()
    
    plot_ari(args.filename)    
