#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include "Cube.h"
#include <Wire.h>
#include <Centipede.h>

// I'm wondering why I made layers zero-indexed and everything else one-indexed
// TODO: Fix that^ ? And maybe improve the way letters are printed ? Use less for loops possibly?

Centipede C;

int time = 100;

int row = 32;

boolean toggle0 = 0;

int count = 0;

int ledStateInts[10];

boolean ledState[125];

Cube::Cube() {

}

void Cube::initialize() {

  Wire.begin();
  TWBR = 12;
  C.initialize();

  pinMode(13, OUTPUT);
  C.portMode(0, 0);
  C.portMode(1, 0);
  C.portMode(2, 0);
  C.portMode(3, 0);
  allOff();
  cli();//stop interrupts

  //set timer1 interrupt at 1Hz
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 1hz increments
  OCR1A = 50;// = (16*10^6) / (1*1024) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS10 and CS12 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);

  sei();//allow interrupts

}

ISR(TIMER1_COMPA_vect){

  sei();
  if (count == 100){
    if (toggle0){
      digitalWrite(13,HIGH);
      toggle0 = 0;
    } else {
      digitalWrite(13,LOW);
      toggle0 = 1;
    }
    count = 0;
  }
  count++;

  row++;
  if (row >= 37) {
    row = 32;
  }

  C.portWrite(2, 0);

  int zeroIndexRow = (row - 32);
  int ledArrIndex = zeroIndexRow * 2;

  C.portWrite(0, ledStateInts[ledArrIndex]);
  C.portWrite(1, ledStateInts[ledArrIndex + 1]);
  C.portWrite(2, 1 << zeroIndexRow);

}

void Cube::allOff() {

    for (int i = 0; i < 10; i++) {
        ledStateInts[i] = 0;
    }

    for (int a = 0; a < 125; a++){
        ledState[a] = 0;
    }

}

void Cube::allOn() {

    for (int i = 0 ; i < 10; i++) {
        ledStateInts[i] = 0b1111111111111111;
    }

    for (int a = 0; a < 125; a++){
        ledState[a] = 1;
    }

}

void Cube::ledOn(int level, int pos) {
    led(1, level, pos);
}

void Cube::ledOff(int level, int pos) {
    led(0, level, pos);
}

void Cube::led(boolean state, int level, int pos) {

    int y;
    int x = 25 * level;

    int num;

    int real_pos = pos;

    int state_pos = (pos - 1) + x;

    if (pos > 15){
      y = (2 * level) + 1;
      pos -= 15;
    }
    else {
      y = 2 * level;
    }

    num = getLedCode(pos);

    if (ledState[state_pos] == !state) {
        ledStateInts[y] = state ? ledStateInts[y] + num : ledStateInts[y] - num;
    }

    ledState[state_pos] = state;
}

int Cube::getLedCode(int position) {
    return position > 0 ? 1 << (position - 1) : 0;
}

void Cube::row(boolean state, char orientation, int plane, int row, int leds) {
    int x;
    int y;

    for (int i = 0; i < 5; i++) {

        switch (orientation) {
            case 'X':
                x = plane - 1;
                y = 1 + ((row - 1) * 5) + i;
                break;
            case 'Y':
                x = i;
                y = row + ((plane - 1) * 5);
                break;
            case 'Z':
                x = row - 1;
                y = plane + (5 * i);
                break;
        }
        bool present = ledPresent(i, leds);
        if (present) {
            led(state, x, y);
        }
    }
}

void Cube::plane(boolean state, char orientation, int pos) {
    plane(state, orientation, pos, 0b11111, 0b11111);
}

void Cube::plane(boolean state, char orientation, int pos, int leds1, int leds2) {

    int x = (pos - 1) * 5;

    for (int i = 1; i < 6; i++){
        bool present = ledPresent(i-1, leds2);
        if (present){
            row(state, orientation, pos, i, leds1);
        }
    }
}

void Cube::orientation(boolean state, char orientation, int perpenLeds, int leds1, int leds2) {
    for (int i = 1; i < 6; i++) {
        bool present = ledPresent(i-1, perpenLeds);
        if (present) {
            plane(state, orientation, i, leds1, leds2);
        }
    }
}

boolean Cube::ledPresent(int index, int ledStateInt) {
    int shifted = ledStateInt >> index;
    return (shifted & 1) == 1;
}

void Cube::printString(String word, String dir, int _speed) {
  for (int i = 0; i < word.length; i++) {
    printCharacter(word[i], dir, _speed);
  }
}

void Cube::printCharacter(char character, String dir, int _speed){

  int int1 = 0;
  int int2 = 0;
  int int3 = 0;
  int int4 = 0;
  int int5 = 0;

  int num1;
  int num2;
  int num3;

  byte _dir;

  if (dir == "forward"){
    _dir = 1;
  } else if (dir == "backward"){
    _dir = 0;
  } else if (dir == "static"){
    _dir = 2;
  }

  switch(character){
    case 'A':
      int1 = 0b00111;
      int2 = 0b01010;
      int3 = 0b10010;
      int4 = 0b01010;
      int5 = 0b00111;
      break;
    case 'B':
      int1 = 0b01010;
      int2 = 0b10101;
      int3 = 0b10101;
      int4 = 0b10101;
      int5 = 0b11111;
      break;
    case 'C':
      int1 = 0b00000;
      int2 = 0b10001;
      int3 = 0b10001;
      int4 = 0b10001;
      int5 = 0b01110;
      break;
    case 'D':
      int1 = 0b01110;
      int2 = 0b10001;
      int3 = 0b10001;
      int4 = 0b10001;
      int5 = 0b11111;
      break;
    case 'E':
      int1 = 0b10001;
      int2 = 0b10101;
      int3 = 0b10101;
      int4 = 0b10101;
      int5 = 0b11111;
      break;
    case 'F':
      int1 = 0b10000;
      int2 = 0b10100;
      int3 = 0b10100;
      int4 = 0b10100;
      int5 = 0b11111;
      break;
    case 'G':
      int1 = 0b00110;
      int2 = 0b10101;
      int3 = 0b10101;
      int4 = 0b10001;
      int5 = 0b01110;
      break;
    case 'H':
      int1 = 0b11111;
      int2 = 0b00100;
      int3 = 0b00100;
      int4 = 0b00100;
      int5 = 0b11111;
      break;
    case 'I':
      int1 = 0b10001;
      int2 = 0b10001;
      int3 = 0b11111;
      int4 = 0b10001;
      int5 = 0b10001;
      break;
    case 'J':
      int1 = 0b11110;
      int2 = 0b00001;
      int3 = 0b00001;
      int4 = 0b00001;
      int5 = 0b00010;
      break;
    case 'K':
      int1 = 0b10001;
      int2 = 0b01010;
      int3 = 0b00100;
      int4 = 0b00100;
      int5 = 0b11111;
      break;
    case 'L':
      int1 = 0b00001;
      int2 = 0b00001;
      int3 = 0b00001;
      int4 = 0b00001;
      int5 = 0b11111;
      break;
    case 'M':
      int1 = 0b11111;
      int2 = 0b01000;
      int3 = 0b00100;
      int4 = 0b01000;
      int5 = 0b11111;
      break;
    case 'N':
      int1 = 0b11111;
      int2 = 0b00010;
      int3 = 0b00100;
      int4 = 0b01000;
      int5 = 0b11111;
      break;
    case 'O':
      int1 = 0b01110;
      int2 = 0b10001;
      int3 = 0b10001;
      int4 = 0b10001;
      int5 = 0b01110;
      break;
    case 'P':
      int1 = 0b11100;
      int2 = 0b10100;
      int3 = 0b10100;
      int4 = 0b10100;
      int5 = 0b11111;
      break;
    case 'Q':
      int1 = 0b01111;
      int2 = 0b10011;
      int3 = 0b10101;
      int4 = 0b10001;
      int5 = 0b01110;
      break;
    case 'R':
      int1 = 0b11101;
      int2 = 0b10110;
      int3 = 0b10100;
      int4 = 0b10100;
      int5 = 0b11111;
      break;
    case 'S':
      int1 = 0b10111;
      int2 = 0b10101;
      int3 = 0b10101;
      int4 = 0b10101;
      int5 = 0b11101;
      break;
    case 'T':
      int1 = 0b10000;
      int2 = 0b10000;
      int3 = 0b11111;
      int4 = 0b10000;
      int5 = 0b10000;
      break;
    case 'U':
      int1 = 0b11110;
      int2 = 0b00001;
      int3 = 0b00001;
      int4 = 0b00001;
      int5 = 0b11110;
      break;
    case 'V':
      int1 = 0b11100;
      int2 = 0b00010;
      int3 = 0b00001;
      int4 = 0b00010;
      int5 = 0b11100;
      break;
    case 'W':
      int1 = 0b11110;
      int2 = 0b00001;
      int3 = 0b11110;
      int4 = 0b00001;
      int5 = 0b11110;
      break;
    case 'X':
      int1 = 0b10001;
      int2 = 0b01010;
      int3 = 0b00100;
      int4 = 0b01010;
      int5 = 0b10001;
      break;
    case 'Y':
      int1 = 0b10000;
      int2 = 0b01000;
      int3 = 0b00111;
      int4 = 0b01000;
      int5 = 0b10000;
      break;
    case 'Z':
      int1 = 0b10001;
      int2 = 0b11001;
      int3 = 0b10101;
      int4 = 0b10011;
      int5 = 0b10001;
      break;
    case '0':
      int1 = 0b00000;
      int2 = 0b11111;
      int3 = 0b10001;
      int4 = 0b11111;
      int5 = 0b00000;
      break;
    case '1':
      int1 = 0b00000;
      int2 = 0b00001;
      int3 = 0b11111;
      int4 = 0b00001;
      int5 = 0b00000;
      break;
    case '2':
      int1 = 0b00000;
      int2 = 0b01001;
      int3 = 0b10101;
      int4 = 0b10011;
      int5 = 0b10001;
      break;
    case '3':
      int1 = 0b11111;
      int2 = 0b10101;
      int3 = 0b10101;
      int4 = 0b10101;
      int5 = 0b10001;
      break;
    case '4':
      int1 = 0b00000;
      int2 = 0b11111;
      int3 = 0b00100;
      int4 = 0b11100;
      int5 = 0b00000;
      break;
    case '5':
      int1 = 0b10010;
      int2 = 0b10101;
      int3 = 0b10101;
      int4 = 0b10101;
      int5 = 0b11101;
      break;
    case '6':
      int1 = 0b00000;
      int2 = 0b10111;
      int3 = 0b10101;
      int4 = 0b11111;
      int5 = 0b00000;
      break;
    case '7':
      int1 = 0b10000;
      int2 = 0b11000;
      int3 = 0b10100;
      int4 = 0b10011;
      int5 = 0b10000;
      break;
    case '8':
      int1 = 0b00000;
      int2 = 0b11111;
      int3 = 0b10101;
      int4 = 0b11111;
      int5 = 0b00000;
      break;
    case '9':
      int1 = 0b00000;
      int2 = 0b11111;
      int3 = 0b10100;
      int4 = 0b11100;
      int5 = 0b00000;
      break;
    case '°':
      int1 = 0b11000;
      int2 = 0b11000;
      int3 = 0b00000;
      int4 = 0b00000;
      int5 = 0b00000;
      break;
    case ' ':
      break;
    default:
      int1 = 0b01000;
      int2 = 0b10100;
      int3 = 0b10111;
      int4 = 0b10000;
      int5 = 0b10000;
  }

  int intArr[] = {int1, int2, int3, int4, int5};

  for (int a = 1; a < 6; a++){
      int plane;

    switch (_dir){
        case 1:
            plane = a;
            break;
        case 0:
            plane = 6 - a;
            break;
        case 2:
            plane = 5;
            break;
    }

    for (int j = 0; j < 5; j++) {
        row(true, 'Y', plane, j+1, intArr[j]);
    }

    delay(_speed);
    allOff();
  }
}
