mkdir %1
mkdir %1\bin
xcopy ..\bin\MSVC90\Release\Launcher.exe %1\bin\*.* /Y
xcopy ..\bin\MSVC90\Release\GraphicsOSG.dll %1\bin\*.* /Y
xcopy ..\bin\MSVC90\Release\PhysicsODE.dll %1\bin\*.* /Y
xcopy ..\bin\MSVC90\Release\PhysicsPhysX.dll %1\bin\*.* /Y
xcopy ..\bin\MSVC90\Release\ShapeGen.dll %1\bin\*.* /Y
xcopy ..\bin\MSVC90\Release\osg*.dll %1\bin\*.* /Y
xcopy ..\bin\MSVC90\Release\ot*.dll %1\bin\*.* /Y
xcopy ..\bin\MSVC90\Release\PhysX3_x86.dll %1\bin\*.* /Y
xcopy ..\bin\MSVC90\Release\lua51.dll %1\bin\*.* /Y

mkdir %1\data
xcopy ..\exe\Pyramide %1\data\Pyramide /E /Y /I

echo start /Ddata bin\Launcher.exe /scenery="Pyramide\Pyramide.lua" /system="GraphicsOSG" /system="PhysicsODE" /system="ShapeGen" > %1\PyramideODE.bat
echo start /Ddata bin\Launcher.exe /scenery="Pyramide\Pyramide.lua" /system="GraphicsOSG" /system="PhysicsPhysX" /system="ShapeGen" > %1\PyramidePhysX.bat