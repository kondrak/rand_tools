#ifndef PLAY_H
#define PLAY_H

#ifdef __cplusplus
extern "C" {
#endif
    int  SetupWAVDriver(void);
    
    void PlayWAV(int);
    int  LoadWAV(const char*);
    void ShutdownWAV(void);
#ifdef __cplusplus
}
#endif

#endif