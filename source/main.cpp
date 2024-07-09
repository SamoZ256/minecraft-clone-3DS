#include <stdio.h>
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

	printf("TEST GAME!");
	printf("Woah!");

	printf("\x1b[20;15HPress Start to exit.");

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
