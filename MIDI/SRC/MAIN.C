#include "src/pmidi.h"

int main(int argc, char *argv[])
{
int loop = TRUE;
int exitcode = 0;
int midiFinished = 0;

    // process the command line

        CommandLine (argc,argv);

        SetupMIDI();

    // process the tracks

        while (loop) {

        // see if we need an active sensing message to keep the device active
         KeepActive();
       
        midiFinished = UpdateMIDI();

        if(midiFinished)
        {
            loop = FALSE;
            exitcode = TRUE;
            break;
        }
        
        // see if the user wishes to exit

            if (kbhit()) {
                switch (getch()) {

                    // quit the program

                    case 's':   // do a fast forward in the song
                    case 'S':
                        MidiFFWD();
                        break;

                    case 0x20:
                        MidiPause();
                        break;

                    case 0x1b:
                        AllNotesOff();
                        loop = FALSE;
                        exitcode = TRUE;

                    default:
                        break;

                }
            }

        MIDIEndFrame();
        }

    // exit now...

        DoExit(exitcode);
        return 0;
}