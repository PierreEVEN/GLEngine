@echo off
set path=%CD%
echo building project files
cd .\Tools\ProjectBuilder\bin\Release\netcoreapp3.1\
start /wait ProjectBuilder.exe %path%
