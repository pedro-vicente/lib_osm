@echo off
if not defined DevEnvDir (
 call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" x86_amd64
 if errorlevel 1 goto :eof
)
cmake -H. -Bbuild -G "Visual Studio 14 2015 Win64" 
msbuild build\lib_osm.sln /target:build /property:configuration=debug
build\Debug\osm_read.exe malibu_park.osm
