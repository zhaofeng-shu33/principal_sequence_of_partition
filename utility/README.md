1. To start refreshly, you need delete the build directory and run
```
python schema.py 
```
to generate tuning configure files.

2. Use `python fine_tuning.py` to generate or update `parameter.json`.
3. Use `python empirical_compare.py` which loads `parameter.json` to generate the LaTeX table files