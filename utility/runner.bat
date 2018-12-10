@echo off
python empirical_compare.py
git add *
git commit -m "upload by script"
git push origin tuning