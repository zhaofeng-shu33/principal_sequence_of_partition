1. To start refreshly, you need delete the build directory and run
```
python schema.py 
```
to generate tuning configure files.

2. Use `python fine_tuning.py` to generate or update `parameter.json`.
3. Use `python empirical_compare.py` which loads `parameter.json` to generate the LaTeX table files

A wrapper module called `info_cluster` is provided to facilitate the use of the algorithm.
You need to install ete3 package.
```Python
from info_cluster import InfoCluster
ic = InfoCluster()
ic.fit([[0,0],[0,1],[5,5]]) # three points on Euclidean space
ic.print_hierachical_tree()
```
the result is like
```shell
      /-0
   /-|
--|   \-1
  |
   \-2
```