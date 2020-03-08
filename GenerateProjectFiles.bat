@echo off

echo building project files
cd .\Tools\ProjectBuilder\bin\Debug\netcoreapp3.1\
start /wait ProjectBuilder.exe %CD%
