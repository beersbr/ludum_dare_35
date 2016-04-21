
pushd src

:: 4530 - not all control paths return a value

cl net/main_server.cpp /Zi /I "../build/src" /Fo..\binary\ /Fd..\binary\ /MT /wd4530 /link /SUBSYSTEM:CONSOLE /LIBPATH:../build/lib ws2_32.lib SDL2.lib SDL2main.lib /out:../binary/ld_35_server.exe
cl main.cpp /I "../build/src" /Fo..\binary\ /Fd..\binary\ /Zi /MT /wd4530 /link /SUBSYSTEM:CONSOLE /LIBPATH:../build/lib opengl32.lib ws2_32.lib SDL2.lib SDL2main.lib glew32.lib /out:../binary/ludum_dare_35.exe


popd