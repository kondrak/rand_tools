#ifndef PMIDI_H
#define PMIDI_H

#ifdef __cplusplus
extern "C" {
#endif
    int  SetupMIDIDriver(void);
    int  LoadMIDI(const char *);
    int  UpdateMIDI(void);
    void FastFwdMIDI(void);
    void PauseMIDI(void);

    void AllNotesOff(void);
    void ShutdownMIDI(void);
#ifdef __cplusplus
}
#endif        
 #endif