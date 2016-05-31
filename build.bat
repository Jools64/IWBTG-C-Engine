del iwbtg.exe
cls

if "%1"=="" goto SDL

IF "%1" == "gl" goto OPENGL

:OPENGL
gcc -std=c99 -D OPENGL -g main.c -Wall -o iwbtg.exe -lm -lSDL2 -lSDL2_image -lSDL2_mixer -lGlew32 -lopengl32 -fmax-errors=5
goto DONE

:SDL
gcc -std=c99 -g main.c -Wall -o iwbtg.exe -lm -lSDL2 -lSDL2_image -lSDL2_mixer -fmax-errors=5
goto DONE

:DONE
iwbtg.exe