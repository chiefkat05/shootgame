echo 'building for RELEASE'

gcc shoot_main.c -o BUILD_LINUX/shoot -DGLEW_STATIC -l:libglfw3.a -lGLEW -lGL -lm -Ofast
cp -r gfx BUILD_LINUX/gfx
cp -r lvl BUILD_LINUX/lvl
cp -r sav BUILD_LINUX/sav
cp -r sfx BUILD_LINUX/sfx
cp -r shaders BUILD_LINUX/shaders

x86_64-w64-mingw32-gcc shoot_main.c -o BUILD_WINDOWS/shoot.exe -DGLEW_STATIC \
        -l:libglfw3.a -l:libglew32.a -l:libopengl32.a -l:libgdi32.a -l:libportaudio.a -Ofast
cp -r gfx BUILD_WINDOWS/gfx
cp -r lvl BUILD_WINDOWS/lvl
cp -r sav BUILD_WINDOWS/sav
cp -r sfx BUILD_WINDOWS/sfx
cp -r shaders BUILD_WINDOWS/shaders

emcc --use-port=contrib.glfw3 shoot_main.c -o BUILD_BROWSER/index.html -lglfw -lGLEW -lGL -O3 -ffast-math \
        --preload-file gfx --preload-file lvl --preload-file sav --preload-file sfx --preload-file shaders \
        -sINITIAL_HEAP=1gb