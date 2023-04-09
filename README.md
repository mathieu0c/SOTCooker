# SOTCooker

## ---- Build ----

<br/><br/>

### Setup

<br/>

#### 1- SDL

Setup SDL by downloading the `SDL2-devel-{version}-mingw` package from [github/SDL](https://github.com/libsdl-org/SDL/releases)
Then extract the folder `x86_64-w64-mingw32` into `C:/Libs/` so that the final include directory for the SDL is `C:/Libs/x86_64-w64-mingw32/include`

<br/>

#### 2- Protobuf

1. Install *MSys2*
2. Install the [Protobuf package](https://packages.msys2.org/package/mingw-w64-x86_64-protobuf) (version **21.12-1**) in *MSys2*
    ```bash
    pacman -S mingw-w64-x86_64-protobuf
    ```

<br/><br/>

### CMake configuration

Useful variables:
- `SDL2_BASE_INSTALLATION_PATH`: base SDL2 installation path. This folder shall contains the following subfolders: `include`, `lib`, `share`, `bin`
    > ***Note***: default is `C:\\Libs\\x86_64-w64-mingw32`
- `BASE_PROTO_PATH`: base protobuf installation path. This folder shall contains the following subfolders: `include`, `lib`, `bin`
    > ***Note***: default is `C:\\msys64\\mingw64`
