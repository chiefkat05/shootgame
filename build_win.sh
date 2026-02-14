echo 'building for DEBUG WINDOWS'
set -e
x86_64-w64-mingw32-gcc -DDEBUG shoot_main.c -o shoot.exe -L../glad \
        -l:libglfw3.a -l:libgladwin.a -l:libopengl32.a -l:libgdi32.a -l:libportaudio.a -lws2_32 -Ofast
wine shoot.exe