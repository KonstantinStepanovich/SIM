@echo off
for /R %1 %%f in ("%3") do (
	call c:\lua\lua.bat "%SimPath%\exe\Tests\Common\CompareTableFile.lua" "%1\%%~nf.csv" "%2\%%~nf.csv" "%4"
)