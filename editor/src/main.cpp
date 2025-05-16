#include "star/core/common.hpp"

#ifdef STAR_PLATFORM_WINDOWS
#include <Windows.h>

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
#ifdef STAR_DEBUG
    AllocConsole();
#endif

    return 0;
}

#else

int main(int argc, char** argv) {
    return 0;
}

#endif