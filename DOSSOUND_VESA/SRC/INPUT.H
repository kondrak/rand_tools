#ifndef INPUT_H
#define INPUT_H

enum KeyCode
{
    KEY_ESC = 1,
    KEY_1 = 2,
    KEY_2 = 3,
    KEY_3 = 4,
    KEY_4 = 5,
    KEY_5 = 6,
    KEY_6 = 7,
    KEY_7 = 8,
    KEY_8 = 9,
    KEY_9 = 10,
    KEY_0 = 11,
    KEY_ENTER = 28,
    KEY_UP = 72,
    KEY_LEFT = 75,
    KEY_RIGHT = 77,
    KEY_DOWN = 80    
};

#ifdef __cplusplus
extern "C" {
#endif

    unsigned short *translateInput(void);
    
#ifdef __cplusplus
}
#endif

#endif