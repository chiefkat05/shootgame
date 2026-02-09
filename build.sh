echo 'building for DEBUG'
set -e
gcc -DDEBUG -Wall -Wno-unused-function -Werror shoot_main.c -o shoot -lglfw -lGLEW -lGL -lportaudio
./shoot