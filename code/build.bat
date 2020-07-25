@echo off

REM June 14, 2020
REM (CK): Learning how to build outside of visual studio. I know this is probably not best practices 
REM       but it is working for me right now and I will continue to make it better as I learn.

REM MTd = static link debug  (MT release)
REM MDd = dynamic link debug  (MD release) Need to include assimp-vc141-mtd.lib 
REM -W4 for warnings
set commonCompilerFlags=-MD -nologo -Gm- -GR- -EHa- -EHsc- -Od -Oi -WX -wd4201 -wd4100 -wd4189 -wd4505 -std:c++17 -DHANDMADE_INTERNAL=1 -DHANDMADE_SLOW=1 -DHANDMADE_WIN32=1 -FC -Z7 -Fmwin32_handmade.map
set CommonLinkerFlags=-opt:ref opengl32.lib glfw3.lib assimp-vc141-mt.lib kernel32.lib user32.lib gdi32.lib winspool.lib winmm.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib

REM assimp-vc141-mt.lib = x64 release  assimp-vc141-mtd.lib

set IncludePaths=-I "W:/finnsie/include"
set LibraryPaths=/LIBPATH:"W:/finnsie/lib/x64"
REM TODO - can we just build both one exe?

IF NOT EXIST ..\..\finnbuild mkdir ..\..\finnbuild
pushd ..\..\finnbuild

REM 32-bit build
REM cl #commonCompilerFlags% ..\handmade\code\win32_handmade.cpp /link -subsystem:windows,5.1 %commonLinkerFlags%

REM 64-bit build
set mainFiles="\finnsie\code\main.cpp" "\finnsie\code\glad.c" "\finnsie\code\stb_image.cpp" "\finnsie\code\game.cpp" "\finnsie\code\utils.cpp" "\finnsie\code\texture_manager.cpp" "\finnsie\code\shader_manager.cpp" 
set moreFiles="\finnsie\code\resource_manager.cpp" "\finnsie\code\renderer.cpp" "\finnsie\code\log.cpp" "\finnsie\code\gui.cpp" "\finnsie\code\camera.cpp" "\finnsie\code\rigid_body.cpp"
set imGUIFiles="\finnsie\include\imgui\imgui.cpp" "\finnsie\include\imgui\imgui_demo.cpp" "\finnsie\include\imgui\imgui_impl_glfw.cpp" "\finnsie\include\imgui\imgui_impl_opengl3.cpp" "\finnsie\include\imgui\imgui_widgets.cpp" "\finnsie\include\imgui\imgui_draw.cpp"
cl %commonCompilerFlags% %IncludePaths% %mainFiles% %moreFiles% %imGUIFiles% /link %LibraryPaths% %commonLinkerFlags%

popd