@echo off
echo Current working directory: %cd%
cd %~dp0
echo Changed to script directory: %cd%
python auto_copy.py