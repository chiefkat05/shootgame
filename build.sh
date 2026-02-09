echo 'building for DEBUG'

gcc -DDEBUG -Wall -Wno-unused-function -Werror shoot_main.c -o shoot -lglfw -lGLEW -lGL -lportaudio