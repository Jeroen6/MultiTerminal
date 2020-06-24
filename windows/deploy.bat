:: Run this file in the Qt shell (eg: Qt 5.13.1 (MinGW 7.3.0 64-bit) from start menu)
:: Run this in the release build directory (eg: build-configurator-Desktop_Qt_5_12_3_MinGW_32_bit-Release)
:: It copies the dll's and executables to the deploy folder.
:: windeployqt collects all relevant Qt dll's.

mkdir deploy
copy MultiTerminal.exe deploy\
copy ..\driver\SerialDriver.exe deploy\
cd deploy
windeployqt MultiTerminal.exe
cd ../
"C:\Program Files (x86)\Inno Setup 6\iscc.exe" installer.iss
