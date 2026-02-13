echo 'building for DEBUG'
set -e
gcc -DDEBUG -Wall -Wno-unused-function -Werror -L../glad shoot_main.c -o shoot -l:libglfw3.a -l:libglad.a -lGL -lm
./shoot