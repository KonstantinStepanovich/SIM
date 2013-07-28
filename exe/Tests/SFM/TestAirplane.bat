set binpath=%SimPath%\bin\MSVC90\Release\TestSFMBatch.exe
set workdir=%SimPath%\exe\Tests\SFM
set output=%SimPath%\output\Tests\Result\SFM\Airplanes\%1

echo ----------
echo Airplane "%1" tests...
echo altitude step = %2 km, M step = %3, fuel mass ratio = %4, payload mass = %5 kg
mkdir %output%

echo turn acceleration(alt, M)
start /B/WAIT/D%workdir% %binpath% TA %1 %2 %3 %4 %5 > "%output%\turn acceleration.csv"
echo Max Vy(alt, M)
start /B/WAIT/D%workdir% %binpath% Vy %1 %2 %3 %4 %5 > "%output%\Max Vy.csv"
echo Max Ny(alt, M)
start /B/WAIT/D%workdir% %binpath% Ny %1 %2 %3 %4 %5 > "%output%\Max Ny.csv"
echo Max Nx(alt, M)
start /B/WAIT/D%workdir% %binpath% Nx %1 %2 %3 %4 %5 > "%output%\Max Nx.csv"
echo thrust req(alt, M)
start /B/WAIT/D%workdir% %binpath% Pr %1 %2 %3 %4 %5 > "%output%\thrust req.csv"
echo consumption(alt, M)
start /B/WAIT/D%workdir% %binpath% Cer %1 %2 %3 %4 %5 > "%output%\consumption.csv"
echo range(alt, M)
start /B/WAIT/D%workdir% %binpath% R %1 %2 %3 %4 %5 > "%output%\range.csv"

echo Vmin(alt)
start /B/WAIT/D%workdir% %binpath% Vmin %1 %2 %4 %5 > "%output%\Vmin.csv"
echo Vmax(alt)
start /B/WAIT/D%workdir% %binpath% Vmax %1 %2 %4 %5 > "%output%\Vmax.csv"
echo Vend(alt)
start /B/WAIT/D%workdir% %binpath% Ve %1 %2 %4 %5 > "%output%\Vend.csv"
echo Vcruise(alt)
start /B/WAIT/D%workdir% %binpath% Vc %1 %2 %4 %5 > "%output%\Vcruise.csv"
echo VyTAS(alt)
start /B/WAIT/D%workdir% %binpath% VyTAS %1 %2 %4 %5 > "%output%\VyTAS.csv"