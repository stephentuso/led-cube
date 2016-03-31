#ifndef Cube_h
#define Cube_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif


class Cube {
    public:
        Cube();
        void initialize();
        void allOff();
        void allOn();
        void led(boolean state, int level, int pos);
        void ledOn(int level, int pos);
        void ledOff(int level, int pos);
        void row(boolean state, char orientation, int plane, int row, int leds);
        void plane(boolean state, char orientation, int pos);
        void plane(boolean state, char orientation, int pos, int arr[]);
        void plane(boolean state, char orientation, int plane, int leds1, int leds2);
        void orientation(boolean state, char orientation, int perpenLeds, int leds1, int leds2);
        void printString(String word, String dir, int _speed);
        void fillArrayForCharacter(int array[], char character);
        void printCharacter(char character, String dir, int _speed);

    private:
        int getLedCode(int position);
        boolean ledPresent(int ledStateInt, int index);
        byte getDirFromString(String dir);
        int getPositionForDirection(byte dir, int index);
};

#endif
