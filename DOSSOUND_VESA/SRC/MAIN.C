#include "src/input.h"
#include "src/sound/midi.h"
#include "src/sound/wave.h"
#include <stdio.h>
#include <stdlib.h>

void Shutdown(int exitCode)
{
    ShutdownWAV();
    ShutdownMIDI();
    exit(exitCode);
}

int main()
{
    unsigned short *keysPressed = translateInput();

    printf("\n\n*** Press keys 1-5 to play sound ***\n\n");

    // setup drivers
    if (!SetupWAVDriver())
        Shutdown(1);     
    
    if (!SetupMIDIDriver())
        Shutdown(1);        

    // load files
    if (!LoadMIDI("sfx/doom.mid"))
        Shutdown(1);        
        
    if (!LoadWAV("sfx/sfx1.wav"))
        Shutdown(1);

    if (!LoadWAV("sfx/sfx2.wav"))
        Shutdown(1);

    if (!LoadWAV("sfx/sfx3.wav"))
        Shutdown(1);        
        
    if (!LoadWAV("sfx/sfx4.wav"))
        Shutdown(1);   

    if (!LoadWAV("sfx/sfx5.wav"))
        Shutdown(1);   

       
       

    // main loop
    while (!keysPressed[KEY_ESC])
    {

        keysPressed = translateInput();

        if (keysPressed[KEY_1])
            PlayWAV(0);

        if (keysPressed[KEY_2])
            PlayWAV(1);

        if (keysPressed[KEY_3])
            PlayWAV(2);            
            
        if (keysPressed[KEY_4])
            PlayWAV(3);   

        if (keysPressed[KEY_5])
            PlayWAV(4);               
            
        UpdateMIDI();
    }

    Shutdown(0);    
    return 0;
}