@echo off

if not exist build mkdir build
pushd build

del /Q *.*

cl /nologo /Zi /FC /WX /W4 /wd4505 /wd4201 /wd4090 /wd4324 /wd4189 /wd4100 /wd4101 /I..\source /I..\dependencies\includes ..\source\wombat_main.cpp /link /libpath:..\dependencies\libraries\ user32.lib SDL3.lib

popd

copy ..\build\wombat_main.exe ..\output\
pushd output
wombat_main.exe
popd