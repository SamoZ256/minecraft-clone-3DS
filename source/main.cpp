#include <iostream>

#include <3ds.h>
#include <citro3d.h>

int main(int argc, char **argv) {
    srvInit();
    aptInit();
    hidInit();
    gfxInitDefault();
    gfxSet3D(false);

    // Init console for debugging
    consoleInit(GFX_BOTTOM, NULL);

	std::cout << "Hello, World!" << std::endl;

    while(aptMainLoop()) {
        gspWaitForVBlank();
        hidScanInput();

        if(hidKeysDown() & KEY_START)
            break;

        gfxFlushBuffers();
        gfxSwapBuffers();
    }

    gfxExit();
    hidExit();
    // TODO: find out why this crashes
    //aptExit();
    srvExit();

    return 0;
}
