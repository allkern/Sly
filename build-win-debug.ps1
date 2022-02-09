$LGW_INCLUDE_DIR    = "C:\Users\Lisandro\Desktop\Development\Libraries\lgw"
$SDL2_DIR           = "C:\Users\Lisandro\Desktop\Development\Libraries\SDL2"

md -Force -Path bin > $null

c++ -I"`"$($LGW_INCLUDE_DIR)`"" `
    -I"`"$($SDL2_DIR)\include\SDL2`"" `
    -I"`"$($SDL2_DIR)\include`"" `
    -I"sly/" `
    "frontend/main.cpp" `
    -o "bin/sly.exe" `
    -L"`"$($SDL2_DIR)\lib\x64`"" `
    -mbmi2 -m64 -lSDL2main -lSDL2 -lopengl32 -g -std=c++2a