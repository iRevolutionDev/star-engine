include(FetchContent)

set(SDL_STATIC ON CACHE BOOL "Build SDL as a static library")

FetchContent_Declare(
        sdl3
        GIT_REPOSITORY https://github.com/libsdl-org/SDL
        GIT_TAG release-3.2.10
)

FetchContent_MakeAvailable(sdl3)