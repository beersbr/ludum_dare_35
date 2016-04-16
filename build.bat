::call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" x64

pushd src

:: 4530 - not all control paths return a value

cl net/main_server.cpp /I "../build/src" /MT /wd4530 /link /SUBSYSTEM:CONSOLE /LIBPATH:../build/lib ws2_32.lib /out:../binary/ld_35_server.exe
cl main.cpp /I "../build/src" /MT /wd4530 /link /SUBSYSTEM:CONSOLE /LIBPATH:../build/lib opengl32.lib SDL2.lib SDL2main.lib glew32.lib /out:../binary/ludum_dare_35.exe


popd