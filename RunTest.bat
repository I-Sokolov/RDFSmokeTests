@echo on

echo Build %1 .......
%RDF_TEST_DEVENV% %1\%1.sln /Build %2
if not .%ERRORLEVEL% == .0 (echo !!!! Failed to build %1.vcxproj !!!! & goto END)

echo Run %1 .........
cd output
%3\%1.exe %4 %5 %6 %7 %8 %9
if not .%ERRORLEVEL% == .0 (echo !!!! Failed test %1.exe !!!! & goto END)
cd ..

:END
