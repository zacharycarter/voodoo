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
emcc $@ -g -gsource-map -I./src/voodoo -I./thirdparty/c89atomic -I./thirdparty/janet -I./thirdparty/sokol -I./thirdparty/sx/include -I./thirdparty/stackwalkerc -I./thirdparty/cj5 -I./thirdparty/sort -I./thirdparty/hmm -I./thirdparty/dds-ktx -I./thirdparty/stb -o index.html -s USE_WEBGPU=1 -s PRECISE_F32=1 -s STACK_SIZE=5MB -s TOTAL_MEMORY=256mb -s ENVIRONMENT=worker,web --shell-file web/voodoo.html -Wfatal-errors --preload-file ./data -s ALLOW_MEMORY_GROWTH=1 -s USE_PTHREADS=1 -s PTHREAD_POOL_SIZE=8 -s ASSERTIONS=1 && "${SRV_CMD[@]}" http://localhost:9999

exit

:windows

if "%1"=="" MAKE.bat .\main.c^
	.\thirdparty\janet\janet.c^
	./thirdparty/sx/src/allocator.c^
	./thirdparty/sx/src/fiber.c^
	./thirdparty/sx/src/handle.c^
	./thirdparty/sx/src/hash.c^
	./thirdparty/sx/src/io.c^
	./thirdparty/sx/src/jobs.c^
	./thirdparty/sx/src/lin-alloc.c^
	./thirdparty/sx/src/lockless.c^
	./thirdparty/sx/src/os.c^
	./thirdparty/sx/src/string.c^
	./thirdparty/sx/src/sx.c^
	./thirdparty/sx/src/threads.c^
	./thirdparty/sx/src/vmem.c

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

@REM rem host webserver:8000 if not open
@REM netstat /ano | find /i "listening" | find ":8000" >nul 2>nul && (
@REM 	rem start python -m http.server --bind 127.0.0.1 8000
@REM ) || (
@REM 	@REM start python -m http.server --bind 127.0.0.1 8000
@REM 	start python .\server.py 9999
@REM )

@REM compile shaders
call .\tools\bin\win32\sokol-shdc.exe -i .\assets\shaders\src\shapes.glsl -o .\assets\shaders\wgsl\shapes.glsl.h -l wgsl -r


@REM compile and launch
@REM emcc %* -g -O0 -I.\shaders\wgsl -I.\thirdparty -I.\thirdparty\c89atomic -I.\thirdparty\janet -I.\thirdparty\sokol -I.\thirdparty\flecs -I.\thirdparty\sx\include -I.\thirdparty\stackwalkerc -I.\thirdparty\cj5 -I.\thirdparty\sort -I.\thirdparty\hmm -I.\thirdparty\dds-ktx -I.\thirdparty\stb -I.\thirdparty\cdbgui -o index.html -s USE_WEBGPU=1 -s PRECISE_F32=1 -s STACK_SIZE=5MB -s TOTAL_MEMORY=256mb -s ENVIRONMENT=worker,web --shell-file shell.html -Wfatal-errors --preload-file .\data -s ALLOW_MEMORY_GROWTH=1 -s USE_PTHREADS=1 -s PTHREAD_POOL_SIZE=8 -s ASSERTIONS=1 -s ASYNCIFY=1 && start "" http://localhost:9999
@REM emcc %* -g -O0 -I.\shaders\wgsl -I.\thirdparty -I.\thirdparty\c89atomic -I.\thirdparty\janet -I.\thirdparty\sokol -I.\thirdparty\flecs -I.\thirdparty\sx\include -I.\thirdparty\stackwalkerc -I.\thirdparty\cj5 -I.\thirdparty\sort -I.\thirdparty\hmm -I.\thirdparty\dds-ktx -I.\thirdparty\stb -I.\thirdparty\cdbgui -o index.html -s USE_WEBGPU=1 -s PRECISE_F32=1 -s STACK_SIZE=5MB -s TOTAL_MEMORY=256mb -s ENVIRONMENT=worker,web --shell-file shell.html -Wfatal-errors -s ALLOW_MEMORY_GROWTH=1 -s USE_PTHREADS=1 -s PTHREAD_POOL_SIZE=8 -s ASSERTIONS=1 -s ASYNCIFY=1 && start "" http://localhost:9999

call cd .\thirdparty\ozz-animation^
	& mkdir .build^
	& cd .build^
	& cmake ..^
	& cmake --build ./ --target BUILD_FUSE_ALL^
	& cd ..\..\..

call em++ -emit-llvm -std=c++17 -c -msimd128 -mavx -pthread -I.\thirdparty\ozz-animation\include .\thirdparty\ozz-animation\.build\src_fused\ozz_base.cc  -o.\thirdparty\ozz-animation\.build\src_fused\ozz_base.bc
call em++ -emit-llvm -std=c++17 -c -msimd128 -mavx -pthread -I.\thirdparty\ozz-animation\include .\thirdparty\ozz-animation\.build\src_fused\ozz_animation.cc -o.\thirdparty\ozz-animation\.build\src_fused\ozz_animation.bc
call em++ -emit-llvm -std=c++17 -c -msimd128 -mavx -pthread -I.\thirdparty\ozz-animation\include -I.\thirdparty\ozz-animation\samples\framework .\thirdparty\ozz-animation\samples\framework\mesh.cc -o.\thirdparty\ozz-animation\.build\src_fused\mesh.bc
call em++ -emit-llvm -std=c++17 -c -msimd128 -mavx -pthread -I.\thirdparty\sokol -I.\thirdparty\ozz-animation\include -I.\thirdparty\ozz-animation\samples -I.\thirdparty\ozz-util .\thirdparty\ozz-util\ozz_util.cc -o.\thirdparty\ozz-util\ozz_util.bc
call emar.bat rcu %~dp0\thirdparty\ozz-animation\ozz.a^
  %~dp0thirdparty\ozz-animation\.build\src_fused\ozz_base.bc^
  %~dp0thirdparty\ozz-animation\.build\src_fused\ozz_animation.bc^
  %~dp0thirdparty\ozz-animation\.build\src_fused\mesh.bc^
  %~dp0thirdparty\ozz-util\ozz_util.bc

call emcc %* -g -O0  -msimd128 -mavx -s NO_EXIT_RUNTIME=1 -s USE_PTHREADS=1 -s PTHREAD_POOL_SIZE=4 -DUSE_DBG_UI -I.\assets\shaders\wgsl -I.\thirdparty -I./thirdparty/sx/include -I./thirdparty/stackwalkerc -I.\thirdparty\cj5 -I.\thirdparty\hmm -I.\thirdparty\janet -I.\thirdparty\sokol -I.\thirdparty\ozz-util -o .\web\voodoo.js -s USE_WEBGPU=1 -s STACK_SIZE=5MB -s TOTAL_MEMORY=256mb -s ASYNCIFY=1 -Wfatal-errors --preload-file .\assets\scripts\game.janet --preload-file .\assets\dolls\ozz_skin.doll --preload-file .\assets\dolls\ozz_skin_skeleton.ozz -s ALLOW_MEMORY_GROWTH=1 -s ASSERTIONS=2 -s MODULARIZE=1 -s EXPORT_ES6 -s EXPORT_NAME="'Voodoo'" -s EXPORTED_RUNTIME_METHODS=["FS"] -lidbfs.js .\thirdparty\ozz-animation\ozz.a

call rollup -c .\web\rollup.config.mjs