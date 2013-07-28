set binpath=%SimPath%\bin\MSVC90\Release\TestSFMBatch.exe
set workdir=%SimPath%\exe\Tests\SFM
set output=%SimPath%\output\Tests\Result\SFM\Airframes\%1

echo ----------
echo Airframe "%1" tests...
echo M step = %2 km, AOA step = %3 deg
mkdir %output%

echo LD(M, alpha)
start /B/WAIT/D%workdir% %binpath% LD %1 %2 %3 > "%output%\LD.csv"
echo AOA optimal(M)
start /B/WAIT/D%workdir% %binpath% AO %1 %2 > "%output%\AOA optimal.csv"
echo Max LD(M)
start /B/WAIT/D%workdir% %binpath% LDM %1 %2 > "%output%\Max LD.csv"
echo Glide Mach Number
start /B/WAIT/D%workdir% %binpath% GM %1 > "%output%\Glide Mach Number.csv"

call TestTorque.bat %1