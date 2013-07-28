set binpath=%SimPath%\bin\MSVC90\Release\TestSFMBatch.exe
set workdir=%SimPath%\exe\Tests\SFM
set output=%SimPath%\output\Tests\Result\SFM\Airframes\%1\Torque

echo Creating files...
mkdir %output%
echo Mx(betta) (3x3)
start /B/WAIT/D%workdir% %binpath% MxMy %1 0 0.0 0.0 0.0 1.0 1.0 > "%output%\MxBetta 1 1.csv"
start /B/WAIT/D%workdir% %binpath% MxMy %1 0 0.0 0.0 0.5 1.0 1.0 > "%output%\MxBetta 1 2.csv"
start /B/WAIT/D%workdir% %binpath% MxMy %1 0 0.0 0.0 1.0 1.0 1.0 > "%output%\MxBetta 1 3.csv"
start /B/WAIT/D%workdir% %binpath% MxMy %1 0 0.5 0.0 0.0 1.0 1.0 > "%output%\MxBetta 2 1.csv"
start /B/WAIT/D%workdir% %binpath% MxMy %1 0 0.5 0.0 0.5 1.0 1.0 > "%output%\MxBetta 2 2.csv"
start /B/WAIT/D%workdir% %binpath% MxMy %1 0 0.5 0.0 1.0 1.0 1.0 > "%output%\MxBetta 2 3.csv"
start /B/WAIT/D%workdir% %binpath% MxMy %1 0 1.0 0.0 0.0 1.0 1.0 > "%output%\MxBetta 3 1.csv"
start /B/WAIT/D%workdir% %binpath% MxMy %1 0 1.0 0.0 0.5 1.0 1.0 > "%output%\MxBetta 3 2.csv"
start /B/WAIT/D%workdir% %binpath% MxMy %1 0 1.0 0.0 1.0 1.0 1.0 > "%output%\MxBetta 3 3.csv"
echo My(betta) (3x3)
start /B/WAIT/D%workdir% %binpath% MxMy %1 1 0.0 0.0 0.0 1.0 1.0 > "%output%\MyBetta 1 1.csv"
start /B/WAIT/D%workdir% %binpath% MxMy %1 1 0.0 0.0 0.5 1.0 1.0 > "%output%\MyBetta 1 2.csv"
start /B/WAIT/D%workdir% %binpath% MxMy %1 1 0.0 0.0 1.0 1.0 1.0 > "%output%\MyBetta 1 3.csv"
start /B/WAIT/D%workdir% %binpath% MxMy %1 1 0.5 0.0 0.0 1.0 1.0 > "%output%\MyBetta 2 1.csv"
start /B/WAIT/D%workdir% %binpath% MxMy %1 1 0.5 0.0 0.5 1.0 1.0 > "%output%\MyBetta 2 2.csv"
start /B/WAIT/D%workdir% %binpath% MxMy %1 1 0.5 0.0 1.0 1.0 1.0 > "%output%\MyBetta 2 3.csv"
start /B/WAIT/D%workdir% %binpath% MxMy %1 1 1.0 0.0 0.0 1.0 1.0 > "%output%\MyBetta 3 1.csv"
start /B/WAIT/D%workdir% %binpath% MxMy %1 1 1.0 0.0 0.5 1.0 1.0 > "%output%\MyBetta 3 2.csv"
start /B/WAIT/D%workdir% %binpath% MxMy %1 1 1.0 0.0 1.0 1.0 1.0 > "%output%\MyBetta 3 3.csv"
echo Mz(alpha) (3)
start /B/WAIT/D%workdir% %binpath% Mz %1 0.0 1.0 > "%output%\MzAlpha 1.csv"
start /B/WAIT/D%workdir% %binpath% Mz %1 0.5 1.0 > "%output%\MzAlpha 2.csv"
start /B/WAIT/D%workdir% %binpath% Mz %1 1.0 1.0 > "%output%\MzAlpha 3.csv"

echo Mx(omegaX) (3)
start /B/WAIT/D%workdir% %binpath% MxOmegaX %1 0.05 > "%output%\MxOmegaX 1.csv"
start /B/WAIT/D%workdir% %binpath% MxOmegaX %1 0.10 > "%output%\MxOmegaX 2.csv"
start /B/WAIT/D%workdir% %binpath% MxOmegaX %1 0.20 > "%output%\MxOmegaX 3.csv"
start /B/WAIT/D%workdir% %binpath% MxOmegaX %1 0.30 > "%output%\MxOmegaX 4.csv"
start /B/WAIT/D%workdir% %binpath% MxOmegaX %1 0.40 > "%output%\MxOmegaX 5.csv"

set refdir=%SimPath%\output\Tests\Reference\SFM\Airframes\%1\Torque

echo Comparing files...
echo Comparing Mx(betta)x3x3...
call ..\Common\CompareTableDir.bat %output% %refdir% MxBetta*.csv ToleranceMxMy.csv
echo Comparing My(betta)x3x3...
call ..\Common\CompareTableDir.bat %output% %refdir% MyMxBetta*.csv ToleranceMxMy.csv
echo Comparing Mz(alpha)x3...
call ..\Common\CompareTableDir.bat %output% %refdir% MzAlpha*.csv ToleranceMz.csv
echo Comparing MxOmegaX(alpha)x5...
call ..\Common\CompareTableDir.bat %output% %refdir% MxOmegaX*.csv ToleranceMxOmegaX.csv