mkdir %1
mkdir %1\bin
xcopy ..\bin\MSVC90\Release\MathModelTuner.exe %1\bin\*.* /Y
xcopy ..\bin\MSVC90\Release\MathModelSFM.dll %1\bin\*.* /Y
xcopy ..\bin\MSVC90\Release\lua51.dll %1\bin\*.* /Y
xcopy ..\..\..\SDK\bin\MSVC90\Release\QtCore4.dll %1\bin\*.* /Y
xcopy ..\..\..\SDK\bin\MSVC90\Release\QtGui4.dll %1\bin\*.* /Y
xcopy ..\..\..\SDK\bin\MSVC90\Release\QtSvg4.dll %1\bin\*.* /Y
xcopy ..\..\..\SDK\bin\MSVC90\Release\qwt.dll %1\bin\*.* /Y

mkdir %1\data
xcopy ..\exe\Common %1\data\Common /E /Y /I
xcopy ..\exe\SFM %1\data\SFM /E /Y /I
mkdir %1\data\Tools\MathModelTuner
xcopy ..\exe\Tools\MathModelTuner\Config %1\data\Tools\MathModelTuner\Config /E /Y /I
xcopy ..\exe\Tools\MathModelTuner\Representations %1\data\Tools\MathModelTuner\Representations /E /Y /I

echo start /Ddata bin\MathModelTuner.exe > %1\MathModelTuner.bat