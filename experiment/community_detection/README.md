# Description
`experiment_two_level.py` is the main script. It contains many utility functions for computing, data loading and saving, and plotting.
`runner.py` is the experiment runner. It is a wrapper of `experiment_two_level.py`.
`plotter.py` loads plotting point data(output from `runner.py`) and plotting the figure with `matplotlib`.

The following command will produce ideal result occasionally. By ideal result we mean the clustering tree has
the same topology structure with that of the ground truth. 
```python
python experiment_two_level.py --z_in_2=2.5 --z_o=0.2 --alg=info-clustering
```
# Limitations
GN method is implementated in pure Python. It is quite slow to run one fit.