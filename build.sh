echo 'building for DEBUG'
set -e
gcc -DDEBUG -Wall -Wno-unused-function -Werror shoot_main.c -o shoot `pkg-config --static --libs glfw3 glew`
./shoot