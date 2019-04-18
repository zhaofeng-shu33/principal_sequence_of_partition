# Prerequisite
To run the Python code in the directory, you need to install packages listed in `requirement.txt`.

1. To start refreshly, you need delete the build directory and run
```
python schema.py 
```
to generate tuning configure files.

* Use `python fine_tuning.py` to generate or update `parameter.json`.
* Use `python empirical_compare.py` which loads `parameter.json` to generate the LaTeX table files

2. A wrapper module called `info_cluster` is provided to facilitate the use of the algorithm.
You need to install `ete3` package.
```Python
from info_cluster import InfoCluster
ic = InfoCluster()
ic.fit([[0,0],[0,1],[5,5]]) # three points on Euclidean space
ic.print_hierachical_tree()
```
The result is like
```shell
      /-0
   /-|
--|   \-1
  |
   \-2
```

3. Plotting routines are provided to visualize the clustering result.
Usage: `python plot_art.py --show_pic`. 
Notice that for the three circle plot is a little bit slow (1min above).