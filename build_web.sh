echo 'building for web'

emcc --use-port=contrib.glfw3 shoot_main.c -o BUILD_BROWSER/index.html -lglfw -lGLEW -lGL \
        --preload-file gfx --preload-file lvl --preload-file sav --preload-file sfx --preload-file shaders \
        -sINITIAL_HEAP=1gb