@echo off
set /P systemName=Enter system name 
echo Creating files for system %systemName% ...
echo Creating source ...
mkdir ..\..\src\SimSystems\%systemName%
xcopy SystemTemplate\*.* ..\..\src\SimSystems\%systemName%\*.* /E /Y
"c:\Program Files (x86)\gsar\bin\gsar.exe" -sSystemNameToReplace -r%systemName% -o ..\..\src\SimSystems\%systemName%\Header\*.hpp
"c:\Program Files (x86)\gsar\bin\gsar.exe" -sSystemNameToReplace -r%systemName% -o ..\..\src\SimSystems\%systemName%\Source\*.cpp
echo Creating MSVC project ...
mkdir ..\..\IDE\MSVC90\Projects\SimSystems\%systemName%
copy SystemTemplate.vcproj ..\..\IDE\MSVC90\Projects\SimSystems\%systemName%\%systemName%.vcproj /Y
"c:\Program Files (x86)\gsar\bin\gsar.exe" -sSystemNameToReplace -r%systemName% -o ..\..\IDE\MSVC90\Projects\SimSystems\%systemName%\%systemName%.vcproj
pause