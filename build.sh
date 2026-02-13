echo 'building for DEBUG'
set -e
gcc -DDEBUG -Wall -Wno-unused-function -Werror shoot_main.c -o shoot -DGLEW_STATIC -l:libglfw3.a -lGLEW -lGL -lm
./shoot