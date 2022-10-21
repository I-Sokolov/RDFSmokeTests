@echo off

echo git pull...
git pull
if not .%ERRORLEVEL% == .0 (echo !!!! FAILED Pull Test !!!! & goto END)

REM %cd% will give you the current working directory (variable)
REM %~dp0 will give you the full path to the batch file's directory (fixed)
REM %0 could give the name of the current subroutine
REM %~dpnx0 gives the full path to the batch 
cd /d %~dp0

IF .%RDF_TEST_DEVENV% == . set RDF_TEST_DEVENV="C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\Common7\IDE\devenv.com"

::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::;

echo ----------------------------- Configurator  -----------------------------
call RunTest Configurator "Release|Any CPU" ..\Configurator.bin\net5.0-windows
if not .%ERRORLEVEL% == .0 (echo !!!! FAILED to configurate tests !!!! & goto END)

::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::;
:: chek parameters

IF .%RDF_TEST_INCLUDE_PATH% == . (echo ERROR: RDF_TEST_INCLUDE_PATH is not set & goto END)
IF .%RDF_TEST_LIB% == . (echo ERROR: RDF_TEST_LIB is not set & goto END)
IF .%RDF_TEST_DLL% == . (echo ERROR: RDF_TEST_DLL is not set & goto END)
IF .%RDF_TEST_ENGINE_CS% == . (echo ERROR: RDF_TEST_ENGINE_CS is not set & goto END)
if .%RDF_TEST_ONLY_KERNEL% == .1 goto ONLY_KERNEL
IF .%RDF_TEST_IFCENGINE_CS% == . (echo ERROR: RDF_TEST_IFCENGINE_CS is not set & goto END)
IF .%RDF_TEST_IFC4_CS% == . (echo ERROR: RDF_TEST_IFC4_CS is not set & goto END)
IF .%RDF_TEST_AP242_CS% == . (echo ERROR: RDF_TEST_AP242_CS is not set & goto END)
:ONLY_KERNEL

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
copy %RDF_TEST_ENGINE_CS%\engine.cs output /Y
copy %RDF_TEST_ENGINE_CS%\geom.cs output /Y
if .%RDF_TEST_ONLY_KERNEL% == .1 goto ONLY_KERNEL
copy %RDF_TEST_IFCENGINE_CS%\ifcengine.cs output /Y
copy %RDF_TEST_IFC4_CS%\IFC4.cs output /Y
copy %RDF_TEST_AP242_CS%\AP242.cs output /Y
:ONLY_KERNEL
@echo off


:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
::

if .%RDF_TEST_ONLY_KERNEL% == .1 goto ONLY_KERNEL

echo ----------------------------- CPP IfcEngine Test -----------------------------
call RunTest CppIfcEngineTests "Release|x64" .
if not .%ERRORLEVEL% == .0 (echo !!!! FAILED CPP IfcEngine Test !!!! & goto END)

echo ----------------------------- C# IfcEngine Test -----------------------------
call RunTest CsIfcEngineTests "Release|Any CPU" net5.0
if not .%ERRORLEVEL% == .0 (echo !!!! FAILED C# IfcEngine Test !!!! & goto END)

:ONLY_KERNEL

echo ----------------------------- CPP Geometry Kernel Test -----------------------------
call RunTest CppEngineTests "Release|x64" .
if not .%ERRORLEVEL% == .0 (echo !!!! FAILED CPP Geometry Kernerl Engine Test !!!! & goto END)

echo ----------------------------- C# Geometry Kernel Test -----------------------------
call RunTest CsEngineTests "Release|Any CPU" net5.0
if not .%ERRORLEVEL% == .0 (echo !!!! FAILED C# Geometry Kernel Engine Test !!!! & goto END)

echo ------------------------------ OK. TESTS ARE PASSED --------------------------

:END
pause
