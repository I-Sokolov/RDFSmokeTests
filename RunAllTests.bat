@echo off

REM %cd% will give you the current working directory (variable)
REM %~dp0 will give you the full path to the batch file's directory (fixed)
REM %0 could give the name of the current subroutine
REM %~dpnx0 gives the full path to the batch 
cd /d %~dp0

echo git pull...
git pull
if not .%ERRORLEVEL% == .0 (echo !!!! FAILED Pull from test reprository. Press Ctr+C to stop or any key to continue !!!! & pause)
git log -1

IF .%RDF_TEST_DEVENV% == . set RDF_TEST_DEVENV="C:\Program Files\Microsoft Visual Studio\2022\Professional\Common7\IDE\devenv.com"
IF not EXIST %RDF_TEST_DEVENV% (echo "Not found devenv.com from Visual Studio 2022. Set RDF_TEST_DEVENV" & goto END)

IF EXIST output rd output /s /q
IF EXIST output (echo !!!!  Failed to cleanup output !!! & goto END)
mkdir output
IF not EXIST output (echo !!!!  Failed to create output !!! & goto END)

::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::;

echo ----------------------------- Configurator  -----------------------------

call RunTest Configurator "Release|Any CPU" net5.0-windows
if not .%ERRORLEVEL% == .0 (echo !!!! FAILED to configurate tests !!!! & goto END)

call output\SetConfig.bat

:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
::

if .%RDF_TEST_ONLY_KERNEL% == .1 goto ONLY_KERNEL

echo ----------------------------- CPP IfcEngine Test -----------------------------
call RunTest CppIfcEngineTests "Release|x64" .
if not .%ERRORLEVEL% == .0 (echo !!!! FAILED CPP IfcEngine Test !!!! & goto END)

echo ----------------------------- C# IfcEngine Test -----------------------------
call RunTest CsIfcEngineTests "Release|Any CPU" net5.0
if not .%ERRORLEVEL% == .0 (echo !!!! FAILED C# IfcEngine Test !!!! & goto END)

if not .%RDF_TEST_TOOLBOX_EX%. == .1. goto SKIP_TOOLBOX_EX
echo ----------------------------- EXPRESS ToolBoxEx Test ---------------------------
call RunTest ToolBoxEx "Release|x64" .
if not .%ERRORLEVEL% == .0 (echo !!!! FAILED ToolBoxEx Test !!!! & goto END)
:SKIP_TOOLBOX_EX

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
