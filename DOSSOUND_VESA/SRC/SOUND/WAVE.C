;   /*\
;---|*|
;---|*| Wave file definition and playback
;---|*|
;---|*| Copyright (c) 1993,1994  V.E.S.A, Inc. All Rights Reserved.
;---|*| Borland Turbo C++ adaptation (c) 2015 Krzysztof Kondrak
;---|*|
;---|*| VBE/AI 1.0 Specification
;---|*|    April 6, 1994. 1.00 release
;---|*|
;   \*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>

#include "src/sound/wave.h"
#include "src/sound/vbeai.h"
#include "src/sound/vesa.h"


;   /*\
;---|*| Global Variables
;   \*/

int  CallBackOccuredWAV = 0;
int  ErrorBackOccured   = 0;

VESAHANDLE  hWAVE = 0;
FILE       *rfileWAV;
char far   *memptrWAV = 0;

GeneralDeviceClass gdcWAV; // receives a copy of the VESA driver info block
fpWAVServ wfnWAV;          // pointer to wave functions


;   /*\
;---|*| Local Variables
;   \*/

typedef struct {
    char *fname;        // file name
    char huge *ptr;     // pointer to the buffer
    long len;           // file length
    int  han;           // registered handle
    long sr;            // sample rate
    int  ch;            // channels
    int  sz;            // data size (8 or 16)
    int  cp;            // compression
} wave;

int MaxWaves   = 0;       // maximum number of registered waves
wave Waves[10] = { 0 };   // up to 10 registered waves


// 4. Wave format control block
typedef struct {
    int  formatTag;         // format category
    int  nChannels;         // stereo/mono
    long nSamplesPerSec;    // sample rate
    long nAvgBytesPerSec;   // stereo * sample rate
    int  nBlockAlign;       // block alignment (1=byte)
    int  nBitsPerSample;    // # byte bits per sample
} WaveInfo;


// 3. Wave detailed information Block
typedef struct {
    char name[4];   // "fmt "
    long length;
    WaveInfo info;
} WaveFormat;


// 3. Data header which follows a WaveFormat Block
typedef struct {
    char name[4];   // "data"
    unsigned long length;
} DataHeader;


// 2. Total Wave Header data in a wave file
typedef struct {
    char name[4];   // "WAVE"
    WaveFormat fmt;
    DataHeader data;
} WaveHeader;


// 2. Riff wrapper around the WaveFormat Block (optional)
typedef struct {
    char name[4];   // "RIFF"
    long length;
} RiffHeader;


// 1. Riff wrapped WaveFormat Block
typedef struct {
    RiffHeader riff;
    WaveHeader wave;
} RiffWave;

#define VALIDWAVE           0
#define UNKNOWNFILETYPE     1

;   /*\
;---|*| prototypes
;   \*/

void far pascal   WAVEndCallback(int, void far *, long, long);
void far pascal    ErrorCallback(int, void far *, long, long);
int                 ReadBlockWAV(int, char huge *, int);
VESAHANDLE      OpenTheWAVDriver(int);
int             ProcessWAVHeader(FILE *, int);

// only allows us to run on this version of the VBE interface. This
// will be removed after the interface is ratified. Changes may be made
// that would cause this code to crash if run on other version. Again,
// this will be removed in the final software.

static int  VersionControl = 0x0100;



int SetupWAVDriver()
{
    int n;
    // do all the work to get a VESA driver
    if (!OpenTheWAVDriver(0)) {    // open the highest driver
        printf("Cannot find any installed VAI devices! (Did you run SBWAVE.COM?)\n");
        return 0;
    }

    // Register all the blocks
    for (n = 0; n < MaxWaves; n++)
        Waves[n].han = (wfnWAV->wsWaveRegister) (Waves[n].ptr, Waves[n].len);

    return 1;
}


void PlayWAV(int idx)
{
    //      CallBackOccuredWAV = 0;    // no callbacks yet

    // set the sample rate, etc.
    (wfnWAV->wsPCMInfo)(Waves[idx].ch, Waves[idx].sr, Waves[idx].cp, 0, Waves[idx].sz);

    // start output
    (wfnWAV->wsPlayBlock)(Waves[idx].han, 0); 

    /*            if (!DriverErrorWAV()) {
    if (!CallBackOccuredWAV) {

    if (kbhit()) {

    switch (getch()) {

    case 0x1b:  // escape
    //CallBackOccuredWAV++;
    //(wfnWAV->wsStopIO)(Waves[idx].han);
    break;

    case 0x20:  // space
    (wfnWAV->wsPauseIO)(Waves[idx].han);
    printf ("\nType SPACE to continue ");
    while (getch() != 0x20) ;
    (wfnWAV->wsResumeIO)(Waves[idx].han);
    break;

    default:
    break;
    }
    }
    }
    }
    */
}


;   /*\
;---|*|----====< LoadWAV >====----
;---|*|
;---|*| Load the wave file
;---|*|
;   \*/
int LoadWAV(const char *filename)
{
    int n, fhan;

    // process all the switches...
    Waves[MaxWaves].fname = filename;

    if ((rfileWAV = fopen(Waves[MaxWaves].fname, "rb")) == 0) {

        if ((rfileWAV = fopen(filename, "rb")) == 0) {
            printf("cannot open .WAV file: \"%s\"\n", Waves[MaxWaves].fname);
            return 0;
        }
    }
    fseek(rfileWAV, 0, SEEK_SET);   // point to the start

    if (ProcessWAVHeader(rfileWAV, MaxWaves) != VALIDWAVE) {
        printf("Invalid .WAV file: \"%s\"\n", filename);
        return 0;
    }

    Waves[MaxWaves].ptr = AllocateBuffer(Waves[MaxWaves].len);

    if (Waves[MaxWaves].ptr == 0) {
        printf("Ran out of memory loading WAV files!\n");
        return 0;
    }

    ReadBlockWAV(fileno(rfileWAV), Waves[MaxWaves].ptr, (int)Waves[MaxWaves].len);

    fclose(rfileWAV);
    MaxWaves++;
    
    return 1;
}

;
;   /*\
;---|*|----====< ShutdowNWAV >====----
;---|*|
;---|*| Shut down WAV driver
;---|*|
;   \*/
void ShutdownWAV()
{
    // close the device if already opened
    if (wfnWAV)
        VESACloseDevice(hWAVE);     // close the device
}


;   /*\
;---|*|----====< DriverErrorWAV >====----
;---|*|
;---|*| Report any errors by the driver
;---|*|
;   \*/
int DriverErrorWAV()
{
    int err = (wfnWAV->wsGetLastError)();

    if (err) {
        printf("Driver reported an error! (code=%02X)\n", err);
        return (err);
    }
    return(0);
}


;   /*\
;---|*|----====< ErrorCallback >====----
;---|*|
;---|*| If we get this, we received the wrong callback!
;---|*|
;   \*/
void far pascal ErrorCallback(han, fptr, len, filler)
int han;        // device handle
void far *fptr; // buffer that just played
long len;       // length of completed transfer
long filler;    // reserved...
{

    // setup our data segment
    _asm {

        push    ds
            mov     ax, seg ErrorBackOccured
            mov     ds, ax
            inc[ErrorBackOccured]
            pop     ds
    }
}


;   /*\
;---|*|----====< OpenTheDriver >====----
;---|*|
;---|*| Find the driver with the highest user preference, and return it to
;---|*| the caller
;---|*|
;   \*/
int OpenTheWAVDriver(pref)
int pref;
{
    int driverpref = 256;   // real low preference
    long l;

    // find a matching driver

    do {
        // Find one DAC device, else bail if non found

        if ((hWAVE = VESAFindADevice(WAVDEVICE)) == 0)
            return(0);

        // get the device information

        if (VESAQueryDevice(hWAVE, VESAQUERY2, &gdcWAV) == 0) {
            printf("Cannot query the installed VAI devices!\n");
            return -1;
        }

        // make sure its a wave device

        if (gdcWAV.gdclassid != WAVDEVICE) {
            printf("The VESA find device query returned a NON DAC device!\n");
            return -1;
        }

        // make sure it's matches the beta version #

        if (gdcWAV.gdvbever != VersionControl) {
            printf("The VESA device version # does not match, cannot continue!\n");
            return -1;
        }

        // get the drivers user preference level

        driverpref = gdcWAV.u.gdwi.widevpref;

        // if the caller is not expressing a preference, then use this one

        if (pref == -1)
            break;

    } while (driverpref != pref);

    // get the memory needed by the device

    if (!(memptrWAV = AllocateBuffer(gdcWAV.u.gdwi.wimemreq))) {
        printf("We don't have memory for the device!\n");
        return -1;
    }

    // if the DAC device doesn't open, bomb out...

    if ((wfnWAV = (fpWAVServ)VESAOpenADevice(hWAVE, 0, memptrWAV)) == 0) {
        printf("Cannot Open the installed WAV devices!\n");
        return -1;
    }

    // setup the record and playback callbacks

    wfnWAV->wsApplPSyncCB = &WAVEndCallback;
    wfnWAV->wsApplRSyncCB = &ErrorCallback;

    // return the handle

    return(hWAVE);
}


;   /*\
;---|*|----====< WAVEndCallback >====----
;---|*|
;---|*| Block End callback. NOTE: No assumptions can be made about
;---|*| the segment registers! (DS,ES,GS,FS)
;---|*|
;   \*/
void far pascal WAVEndCallback(han, fptr, len, filler)
VESAHANDLE han; // device handle
void far *fptr; // buffer that just played
long len;       // length of completed transfer
long filler;    // reserved...
{
    // setup our data segment

    _asm {

        push    ds
            mov     ax, seg CallBackOccuredWAV
            mov     ds, ax
            inc[CallBackOccuredWAV]
            pop     ds
    }
}

;
;   /*\
;---|*|----====< ProcessWAVHeader >====----
;---|*|
;---|*| load the header from our WAV file format
;---|*|
;   \*/
int ProcessWAVHeader(f, idx)
FILE *f;
int idx;
{
    int n;
    RiffWave rw;

    // eat the RIFF portion of the header

    ReadBlockWAV(fileno(f), (void far *)&rw, sizeof(RiffWave));

    // make sure its says RIFF

    n = rw.riff.name[0] - 'R';
    n += rw.riff.name[1] - 'I';
    n += rw.riff.name[2] - 'F';
    n += rw.riff.name[3] - 'F';

    if (n)
        return(UNKNOWNFILETYPE);

    // make sure it says WAVE

    n = rw.wave.name[0] - 'W';
    n += rw.wave.name[1] - 'A';
    n += rw.wave.name[2] - 'V';
    n += rw.wave.name[3] - 'E';

    if (n)
        return(UNKNOWNFILETYPE);

    // make sure it says 'fmt '

    n = rw.wave.fmt.name[0] - 'f';
    n = rw.wave.fmt.name[1] - 'm';
    n = rw.wave.fmt.name[2] - 't';
    n = rw.wave.fmt.name[3] - ' ';

    if (n)
        return(UNKNOWNFILETYPE);

    Waves[idx].ch = rw.wave.fmt.info.nChannels;
    Waves[idx].sr = rw.wave.fmt.info.nSamplesPerSec;
    Waves[idx].sz = rw.wave.fmt.info.nBitsPerSample;
    Waves[idx].cp = 0;

    // make sure it says 'data'

    n = rw.wave.data.name[0] - 'd';
    n = rw.wave.data.name[1] - 'a';
    n = rw.wave.data.name[2] - 't';
    n = rw.wave.data.name[3] - 'a';

    if (n)
        return(UNKNOWNFILETYPE);

    Waves[idx].len = rw.wave.data.length;

    // return OKAY

    return(VALIDWAVE);

}


;   /*\
;---|*|----====< ReadBlockWAV >====----
;---|*|
;---|*| read a chunk of the PCM file into the huge buffer
;---|*|
;   \*/
int ReadBlockWAV(han, tptr, len)
int han;
char huge *tptr;
int len;
{
    int siz = 0;

    // go get it...
    _asm {
        push    ds

            mov     cx, [len]
            mov     ax, cx
            add     cx, word ptr[tptr]  // wrap?
            jnc     rdbl05              // no, go get the size

            sub     ax, cx               // ax holds the # of bytes to read
            mov     cx, ax
            sub[len], ax

            mov     ah, 03fh             // cx holds the length
            mov     bx, [han]
            lds     dx, [tptr]
            int     21h

            mov[siz], ax            // we moved this much
            add     word ptr[tptr + 2], 0x1000

            cmp     ax, cx               // same size?
            jnz     rdbldone            // no, exit...
    }
rdbl05:
    _asm {
        mov     ah, 03fh
            mov     bx, [han]
            mov     cx, [len]

            jcxz    rdbldone

            lds     dx, [tptr]
            int     21h

            add[siz], ax            // we moved this much
    }
rdbldone:
    _asm {
        pop     ds

    }

    // return the amount read

    return(siz);
}
