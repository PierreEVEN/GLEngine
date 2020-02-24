@echo off

set baseVcxProjPath=.\Engine.vcxprojBase
set GeneratorRelativePath=
set vcxProjRelativePath=.\Engine.vcxproj


type %baseVcxProjPath% > %vcxProjRelativePath%
echo. >> %vcxProjRelativePath%
echo   ^<ItemGroup^> >> %vcxProjRelativePath%
echo ###############################  Searching for Header files  ###############################

setlocal EnableDelayedExpansion
for /L %%n in (1 1 500) do if "!__cd__:~%%n,1!" neq "" set /a "len=%%n+1"
setlocal DisableDelayedExpansion
FOR /r . %%i IN (*.h) DO (
	set "absPath=%%i"
	setlocal EnableDelayedExpansion
	set "relPath=!absPath:~%len%!"
	set result=!GeneratorRelativePath!!relPath!
	echo !result!
	echo     ^<ClInclude Include="!result!" ^/^> >> %vcxProjRelativePath%
	endlocal
)
echo   ^<^/ItemGroup^> >> %vcxProjRelativePath%
echo   ^<ItemGroup^> >> %vcxProjRelativePath%
echo ###############################  Searching for Source files  ###############################
FOR /r . %%i IN (*.cpp) DO (
	set "absPath=%%i"
	setlocal EnableDelayedExpansion
	set "relPath=!absPath:~%len%!"
	set result=!GeneratorRelativePath!!relPath!
	echo !result!
	echo     ^<ClCompile Include="!result!" ^/^> >> %vcxProjRelativePath%
	endlocal
)
echo ###############################  Searching for C source files  ###############################
FOR /r . %%i IN (*.c) DO (
	set "absPath=%%i"
	setlocal EnableDelayedExpansion
	set "relPath=!absPath:~%len%!"
	set result=!GeneratorRelativePath!!relPath!
	echo !result!
	echo     ^<ClCompile Include="!result!" ^/^> >> %vcxProjRelativePath%
	endlocal
)
echo   ^<^/ItemGroup^> >> %vcxProjRelativePath%
echo ^<^/Project^> >> %vcxProjRelativePath%


echo ###############################  Finnished vcxProj file generation  ###############################
echo.
echo.
echo done
echo.
echo.
echo.
echo.
echo.

exit 0