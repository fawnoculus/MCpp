@echo off

pushd ..
mkdir build
cmake -B build
popd
echo You should now have a Visual Studio Solution File in ./build
pause
