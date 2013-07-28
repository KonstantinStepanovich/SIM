set binpath=%SimPath%\bin\MSVC90\Release\TestSFMBatch.exe
set workdir=%SimPath%\exe\Tests\SFM
set output=%SimPath%\output\Tests\Result\SFM\JetEngines\%1

echo ----------
echo Jet Engine "%1" tests...
echo altitude step = %2 km, M step = %3
mkdir %output%

echo thrust max(alt, M)
start /B/WAIT/D%workdir% %binpath% P %1 %2 %3 0.0 off > "%output%\thrust max.csv"
echo thrust aft(alt, M)
start /B/WAIT/D%workdir% %binpath% P %1 %2 %3 0.0 on > "%output%\thrust aft.csv"
echo consumption max(alt, M)
start /B/WAIT/D%workdir% %binpath% Ce %1 %2 %3 0.0 off > "%output%\consumption max.csv"
echo consumption aft(alt, M)
start /B/WAIT/D%workdir% %binpath% Ce %1 %2 %3 0.0 on > "%output%\consumption aft.csv"