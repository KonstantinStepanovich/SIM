mkdir %1
mkdir %1\bin
xcopy ..\bin\MSVC90\Release\Launcher.exe %1\bin\*.* /Y
xcopy ..\bin\MSVC90\Release\GraphicsOSG.dll %1\bin\*.* /Y
xcopy ..\bin\MSVC90\Release\PhysicsODE.dll %1\bin\*.* /Y
xcopy ..\bin\MSVC90\Release\InputOIS.dll %1\bin\*.* /Y
xcopy ..\bin\MSVC90\Release\FlightModel.dll %1\bin\*.* /Y
xcopy ..\bin\MSVC90\Release\osg*.dll %1\bin\*.* /Y
xcopy ..\bin\MSVC90\Release\ot*.dll %1\bin\*.* /Y
xcopy ..\bin\MSVC90\Release\lua51.dll %1\bin\*.* /Y

mkdir %1\data
xcopy ..\exe\Common %1\data\Common /E /Y /I
xcopy ..\exe\FlightModel %1\data\FlightModel /E /Y /I
xcopy ..\exe\SFM %1\data\SFM /E /Y /I

rem echo start /Ddata\FlightModel bin/Launcher.exe scenery="Airplane.lua" /system="FlightModel" /system="GraphicsOSG" /system="PhysicsODE" /system="InputOIS" > %1\FlightModel.bat
echo start /Ddata bin\Launcher.exe /scenery="FlightModel\Scenes\Airplane.lua" /system="FlightModel" /system="GraphicsOSG" /system="PhysicsODE" /system="InputOIS" > %1\FlightModel.bat