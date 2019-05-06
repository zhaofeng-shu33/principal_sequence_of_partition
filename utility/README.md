# Prerequisite
To run the Python code in the directory, you need to install packages listed in `requirement.txt` and some other packages
including 'oss2, info_cluster, tabulate'. Since `requirement.txt` is used in travis ci, we only include build required packages in 
`requirement.txt`.

1. To start refreshly, you need delete the build directory and run
```
python schema.py 
```
to generate tuning configure files.

* Use `python fine_tuning.py` to generate or update `parameter.json`.
* Use `python empirical_compare.py` which loads `parameter.json` to generate the LaTeX table files

## About cloud file storage. 
`TUNING_FILE` and `PARAMETER_FILE` defined in `schema.yaml` can be got from and uploaded to the aliyun oss server.
This functionality is used to share the computation results between different workstation.
The write operation needed authentication.
Currently the got method needs authentication two, but through web port it does not need authentication.
Download link is [parameter.json](http://data-visualization.leidenschaft.cn/research/info-clustering/code/utility/parameter.json).
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