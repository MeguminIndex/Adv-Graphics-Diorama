cd %~f0
mkdir build
cd build
cmake .. -G "Visual Studio 14 2015 Win64"
cmake --build . --config Debug
cd ..
pause