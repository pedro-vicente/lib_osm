@echo off
if not defined DevEnvDir (
 call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
 if errorlevel 1 goto :eof
)

set MSVC_VERSION="Visual Studio 16 2019"
echo using %MSVC_VERSION%

:build
echo building...
rm -rf CMakeCache.txt CMakeFiles
cmake -H. -Bbuild -A x64 -G %MSVC_VERSION%
msbuild build\lib_osm.sln /target:build /property:configuration=debug
build\Debug\osm_read.exe malibu_park.osm
