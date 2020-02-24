@echo off

echo building vcxProj file

cd .\Modules\Engine\
start /wait .\GenerateProjectFiles.bat
cd ..\ImGui\
start /wait .\GenerateProjectFiles.bat
cd ..\ProceduralLandscapeLibrary\
start /wait .\GenerateProjectFiles.bat
cd ..\Stdbi\
start /wait .\GenerateProjectFiles.bat
cd ..\Editor\
start /wait .\GenerateProjectFiles.bat
cd ..\Glad\
start /wait .\GenerateProjectFiles.bat
cd .\..\..\

echo starting sln projectFile
start .\GlEngine.sln
