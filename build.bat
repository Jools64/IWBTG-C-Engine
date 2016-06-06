del iwbtg.exe

IF "%1"=="" goto SDL

IF "%1" == "gl" goto OPENGL

IF "%1" == "sdl" goto SDL

IF "%1" == "release" goto RELEASE

:OPENGL
cls
gcc -std=c99 -D OPENGL -g main.c -Wall -o iwbtg.exe -lm -lSDL2 -lSDL2_image -lSDL2_mixer -lGlew32 -lopengl32 -fmax-errors=5
@echo off
goto DONE

:SDL
cls
gcc -std=c99 -g main.c -Wall -o iwbtg.exe -lm -lSDL2 -lSDL2_image -lSDL2_mixer -fmax-errors=5
@echo off
goto DONE

:RELEASE
cls
gcc -std=c99 -D OPENGL -O3 main.c -Wall -o iwbtg.exe -lm -lSDL2 -lSDL2_image -lSDL2_mixer -lGlew32 -lopengl32 -fmax-errors=5
@echo off
goto DONE

:DONE
echo on
iwbtg.exe