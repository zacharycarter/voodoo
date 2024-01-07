#!/bin/bash 2>nul || goto :windows

if [ "$1" = "" ]; then
	sh make.bat src/main.c \
	./thirdparty/janet/janet.c \
	./thirdparty/sx/src/allocator.c \
	./thirdparty/sx/src/handle.c \
	./thirdparty/sx/src/hash.c \
	./thirdparty/sx/src/io.c \
	./thirdparty/sx/src/jobs.c \
	./thirdparty/sx/src/lockless.c \
	./thirdparty/sx/src/os.c \
	./thirdparty/sx/src/string.c \
	./thirdparty/sx/src/sx.c \
	./thirdparty/sx/src/threads.c \
	./thirdparty/sx/src/vmem.c
	exit
fi

## clone emscripten sdk
if ! [ -x "$(command -v emsdk)" ]; then
	git clone https://github.com/emscripten-core/emsdk ../../../../../emsdk
	pushd ../../../../../emsdk
	./emsdk install 3.0.0  ## latest
	./emsdk activate 3.0.0 ## latest
	source ./emsdk_env.sh
	popd
fi

## cook art
# if [ "$(uname)" = "Darwin" ]; then
#     chmod +x ../../tools/cook.osx
#     ../../tools/cook.osx   --cook-jobs=1 --cook-ini=../../tools/cook.ini
#     cp .art[00].zip index.zip
# else
#     chmod +x ../../tools/cook.linux
#     ../../tools/cook.linux --cook-jobs=1 --cook-ini=../../tools/cook.ini
#     cp .art[00].zip index.zip
# fi
# cp template.js index.coi.js

## host webserver, compile and launch
if [ "$(uname)" = "Darwin" ]; then
	SRV_CMD=("open")
else
	SRV_CMD=("xdg-open")
fi

# python3 -m http.server --bind 127.0.0.1 8888 1>/dev/null 2>/dev/null &
# server.py 8888 1>/dev/null 2>/dev/null &
emcc $@ -g -gsource-map -I./src/voodoo -I./thirdparty/c89atomic -I./thirdparty/janet -I./thirdparty/sokol -I./thirdparty/sx/include -I./thirdparty/stackwalkerc -I./thirdparty/cj5 -I./thirdparty/sort -I./thirdparty/hmm -I./thirdparty/dds-ktx -I./thirdparty/stb -o index.html -s USE_WEBGPU=1 -s PRECISE_F32=1 -s STACK_SIZE=5MB -s TOTAL_MEMORY=256mb -s ENVIRONMENT=worker,web --shell-file shell.html -Wfatal-errors --preload-file ./data -s ALLOW_MEMORY_GROWTH=1 -s USE_PTHREADS=1 -s PTHREAD_POOL_SIZE=8 -s ASSERTIONS=1 -s ASYNCIFY=1 && "${SRV_CMD[@]}" http://localhost:9999

exit

:windows

if "%1"=="" MAKE.bat .\main.c ^
.\thirdparty\janet\janet.c 

@REM if "%1"=="tidy" del index.* & del *.zip & del temp_* & exit /b && rem rd /q /s emsdk

rem clone emscripten sdk
@REM if not exist "emsdk" (
@REM 	git clone https://github.com/emscripten-core/emsdk emsdk
@REM 	pushd emsdk
@REM 		call emsdk install  3.0.0 && rem latest
@REM 		call emsdk activate 3.0.0 && rem latest
@REM 	popd
@REM )
@REM if "%EMSDK%"=="" call emsdk\emsdk_env.bat --system

rem cook art
@REM ..\..\tools\cook.exe --cook-jobs=1 --cook-ini=..\..\tools\cook.ini
@REM copy /y .art[00].zip index.zip
@REM copy /y template.js index.coi.js

rem host webserver:8000 if not open
netstat /ano | find /i "listening" | find ":8000" >nul 2>nul && (
	rem start python -m http.server --bind 127.0.0.1 8000
) || (
	@REM start python -m http.server --bind 127.0.0.1 8000
	start python .\server.py 9999
)

rem compile shaders
.\tools\bin\win32\sokol-shdc.exe -i .\assets\shaders\src\shapes.glsl -o .\assets\shaders\wgsl\shapes.glsl.h -l wgsl -r

rem compile and launch
@REM emcc %* -g -O0 -I.\shaders\wgsl -I.\thirdparty -I.\thirdparty\c89atomic -I.\thirdparty\janet -I.\thirdparty\sokol -I.\thirdparty\flecs -I.\thirdparty\sx\include -I.\thirdparty\stackwalkerc -I.\thirdparty\cj5 -I.\thirdparty\sort -I.\thirdparty\hmm -I.\thirdparty\dds-ktx -I.\thirdparty\stb -I.\thirdparty\cdbgui -o index.html -s USE_WEBGPU=1 -s PRECISE_F32=1 -s STACK_SIZE=5MB -s TOTAL_MEMORY=256mb -s ENVIRONMENT=worker,web --shell-file shell.html -Wfatal-errors --preload-file .\data -s ALLOW_MEMORY_GROWTH=1 -s USE_PTHREADS=1 -s PTHREAD_POOL_SIZE=8 -s ASSERTIONS=1 -s ASYNCIFY=1 && start "" http://localhost:9999
@REM emcc %* -g -O0 -I.\shaders\wgsl -I.\thirdparty -I.\thirdparty\c89atomic -I.\thirdparty\janet -I.\thirdparty\sokol -I.\thirdparty\flecs -I.\thirdparty\sx\include -I.\thirdparty\stackwalkerc -I.\thirdparty\cj5 -I.\thirdparty\sort -I.\thirdparty\hmm -I.\thirdparty\dds-ktx -I.\thirdparty\stb -I.\thirdparty\cdbgui -o index.html -s USE_WEBGPU=1 -s PRECISE_F32=1 -s STACK_SIZE=5MB -s TOTAL_MEMORY=256mb -s ENVIRONMENT=worker,web --shell-file shell.html -Wfatal-errors -s ALLOW_MEMORY_GROWTH=1 -s USE_PTHREADS=1 -s PTHREAD_POOL_SIZE=8 -s ASSERTIONS=1 -s ASYNCIFY=1 && start "" http://localhost:9999
emcc %* -g -O0 -DUSE_DBG_UI -I.\assets\shaders\wgsl -I.\thirdparty -I.\thirdparty\hmm -I.\thirdparty\janet -I.\thirdparty\sokol -I.\thirdparty\tinyfiledialogs -I.\thirdparty\zpl\code -o index.html -s USE_WEBGPU=1 -s STACK_SIZE=5MB -s TOTAL_MEMORY=256mb --shell-file shell.html -Wfatal-errors --preload-file .\assets -s ALLOW_MEMORY_GROWTH=1 -s ASSERTIONS=1 && start "" http://localhost:9999
