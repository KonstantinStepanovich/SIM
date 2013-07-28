@echo off

del error.log

mkdir %SimPath%\output\Tests\Result\SFM
echo ----------
echo Output cleanup
del /F/S/Q %SimPath%\output\Tests\Result\SFM\*.*

echo ----------
echo Jet Engines tests...
mkdir %SimPath%\output\Tests\Result\SFM\JetEngines
set altStep=1.0
set Mstep=0.2
call TestJetEngine.bat AL-31F %altStep% %Mstep%
call TestJetEngine.bat R-195 %altStep% %Mstep%

echo ----------
echo Airframes tests...
mkdir %SimPath%\output\Tests\Result\SFM\Airframes
set Mstep=0.6
set AOAstep=1.0
call TestAirframe.bat Su-27 %Mstep% %AOAstep%
call TestAirframe.bat Su-25 %Mstep% %AOAstep%

echo ----------
echo Airplanes tests...
mkdir %SimPath%\output\Tests\Result\SFM\Airplanes
set altStep=1.0
set VaKmhStep=200.0
set fuelMassRatio=0.5
set payloadMass=0.0
call TestAirplane.bat Su-27 %altStep% %VaKmhStep% %fuelMassRatio% %payloadMass%
call TestAirplane.bat Su-25 %altStep% %VaKmhStep% %fuelMassRatio% %payloadMass%

echo ----------
echo Done!
pause