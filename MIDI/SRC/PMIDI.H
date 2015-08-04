#ifndef PMIDI_H
#define PMIDI_H


;   /*\
;---|*|----====< Play MIDI >====----
;---|*|
;---|*| play/record blocks of MIDI
;---|*|
;---|*| Copyright (c) 1993,1994  V.E.S.A, Inc. All Rights Reserved.
;---|*|
;---|*| VBE/AI 1.0 Specification
;---|*|    February 2, 1994. 1.00 release
;---|*|
;---|*| Additional Changes:
;---|*|   04/05 - Changed the handling of the delta and track processing
;---|*|           because the cheap quantization threw things out of order.
;---|*|   04/05 - Program didn't exit since the break was nested 2 deep.
;---|*|   03/10 - Added cheap quantization of the tempo to reduce the
;---|*|           interrupt rate to drive the tempo.
;---|*|   02/27 - Removed a bunch of unused code to clean up the pgm
;---|*|   02/16 - Added a switch to allow sending Active Sensing as an option.
;---|*|   02/16 - Added a routine to send All Notes Off before exiting.
;---|*|
;   \*/

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <conio.h>
#include <ctype.h>

#include "src/sound/vbeai.h"
#include "src/sound/vesa.h"

#define TRUE    -1
#define FALSE    0
#define ON      TRUE
#define OFF     FALSE

#if DEBUG
#define BREAKPOINT _asm{int 3};
#else
#define BREAKPOINT _asm{nop};
#endif

        void SetupMIDI(void);
        int  UpdateMIDI(void);
        void MidiFFWD(void);
        void MidiPause(void);
        void MIDIEndFrame(void);


        void GiveHelps(void);
        void DoExit(int cc);
        void CommandLine(int argc, char *argv[]);
        void KeepActive(void);
        void AllNotesOff(void);
        void ReportCallback(void);
        void DumpSomeInfo(void);
        
 #endif