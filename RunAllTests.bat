@echo off

git pull
if not .%ERRORLEVEL% == .0 (echo !!!! FAILED Pull Test !!!! & goto END)

REM %cd% will give you the current working directory (variable)
REM %~dp0 will give you the full path to the batch file's directory (fixed)
REM %0 could give the name of the current subroutine
REM %~dpnx0 gives the full path to the batch 
cd /d %~dp0

::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::;
:: chek parameters

IF .%RDF_TEST_INCLUDE_PATH% == . (echo ERROR: RDF_TEST_INCLUDE_PATH is not set & goto END)
IF .%RDF_TEST_LIB% == . (echo ERROR: RDF_TEST_LIB is not set & goto END)
IF .%RDF_TEST_DLL% == . (echo ERROR: RDF_TEST_DLL is not set & goto END)
IF .%RDF_TEST_CS_ENGINE% == . (echo ERROR: RDF_TEST_CS_ENGINE is not set & goto END)

IF .%RDF_TEST_DEVENV% == . set RDF_TEST_DEVENV="C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\Common7\IDE\devenv.com"


echo -----------------------------                -----------------------------
echo Smoke testing %RDF_TEST_DLL%
echo	include path: %RDF_TEST_INCLUDE_PATH%
echo	library: %RDF_TEST_LIB%
echo ...

::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:: copy testing dll

IF EXIST output rd output /s /q
IF EXIST output (echo !!!!  Failed to cleanup output !!! & goto END)

mkdir output
IF not EXIST output (echo !!!!  Failed to create output !!! & goto END)

@echo on
copy %RDF_TEST_DLL% output /Y
copy %RDF_TEST_CS_ENGINE% output\xxxEngine.cs /Y
xcopy %RDF_TEST_INCLUDE_PATH%\*.cs output /Y
@echo off
if not exist %RDF_TEST_DLL% (echo !!!!  Failed to copy %RDF_TEST_DLL% to output !!! & goto END)

:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
::

if .%1 == .NOIFC goto NOIFC

echo ----------------------------- CPP IfcEngine Test -----------------------------
call RunTest CppIfcEngineTests "Release|x64" .
if not .%ERRORLEVEL% == .0 (echo !!!! FAILED CPP IfcEngine Test !!!! & goto END)

echo ----------------------------- C# IfcEngine Test -----------------------------
call RunTest CsIfcEngineTests "Release|Any CPU" net5.0
if not .%ERRORLEVEL% == .0 (echo !!!! FAILED C# IfcEngine Test !!!! & goto END)

:NOIFC

echo ------------------------------ OK. TESTS ARE PASSED --------------------------

:END
pause
