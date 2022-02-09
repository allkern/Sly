$LGW_INCLUDE_DIR    = "C:\Users\Lisandro\Desktop\Development\Libraries\lgw"
$SDL2_DIR           = "C:\Users\Lisandro\Desktop\Development\Libraries\SDL2"

c++ -I"`"$($LGW_INCLUDE_DIR)`"" `
    -I"`"$($SDL2_DIR)\include\SDL2`"" `
    -I"`"$($SDL2_DIR)\include`"" `
    main.cpp `
    -o main.exe `
    -L"`"$($SDL2_DIR)\lib\x64`"" `
    -mbmi2 -m64 -lSDL2main -lSDL2 -lopengl32 -g -std=c++2a