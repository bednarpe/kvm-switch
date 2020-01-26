/**
 * Custom KVM
 * 
 * Switching HDMI and USB between multiple computers using keypad to allow work with multiple computers simultaneously.
 * TODO: Programatic change over usb console
 * 
 * Featuring:
 *  - Arduino pro micro (5V, 16Mhz, ATMEGA32u4)
 *  - 2x PremiumCord HDMI switch 3:1 (connected with standard RS-232 interface using MAX232 or compatible chip to implement 2 ports with only tx,rx and ground connected)
 *  - 1x ATEN USB switch 4:1 (directly connected to 2 pins, desoldered wired remote button click RS-latch counter, exchanged with NOT gates to simulate all states)
 *  - 1x 4x4 keyboard (directly connected to 8 pins)
 * 
 */
#include <arduino.h>
#include <MultitapKeypad.h>
#include <CustomSoftwareSerial.h>

// Keyboard connection
const byte ROW0 = 9;
const byte ROW1 = 8;
const byte ROW2 = 7;
const byte ROW3 = 6;
const byte COL0 = 5;
const byte COL1 = 4;
const byte COL2 = 3;
const byte COL3 = 2;

// 2 pins connected to usb hub
const byte USB_P0 = 0;
const byte USB_P1 = 1;

// Selected device
const int DEVICE_1 = 1;
const int DEVICE_2 = 2;
const int DEVICE_3 = 3;
const int DEVICE_OFF = 0;

// For matrix 4 x 3 keypad
// MultitapKeypad kpd(ROW0, ROW1, ROW2, ROW3, COL0, COL1, COL2);
// For matrix 4 x 4 keypad
MultitapKeypad kpd(ROW0, ROW1, ROW2, ROW3, COL0, COL1, COL2, COL3);
Key key;

// HDMI switch messages
const char* DISPLAY_MSG_SELECT_1 = "port0R";
const char* DISPLAY_MSG_SELECT_2 = "port1R";
const char* DISPLAY_MSG_SELECT_3 = "port2R";
const char* DISPLAY_MSG_ON = "poweronR";
const char* DISPLAY_MSG_OFF = "poweroffR";

const int DISPLAY_1 = 0;
const int DISPLAY_2 = 1;
CustomSoftwareSerial* displaySerial[2];

void setUSB(int selected) {
  switch (selected) {
    case DEVICE_1:
      digitalWrite(USB_P0, LOW);
      digitalWrite(USB_P1, LOW);
      break;
    case DEVICE_2:
      digitalWrite(USB_P0, HIGH);
      digitalWrite(USB_P1, LOW);
      break;
    case DEVICE_3:
      digitalWrite(USB_P0, LOW);
      digitalWrite(USB_P1, HIGH);
      break;
    case DEVICE_OFF:
      digitalWrite(USB_P0, HIGH);
      digitalWrite(USB_P1, HIGH);
      break;
  }
}

void setDisplay(int display, int state) {
  CustomSoftwareSerial* interface = displaySerial[display];
  switch (state) {
    case DEVICE_1:
      interface->write(DISPLAY_MSG_SELECT_1);
      interface->write(DISPLAY_MSG_ON);
      break;
    case DEVICE_2:
      interface->write(DISPLAY_MSG_SELECT_2);
      interface->write(DISPLAY_MSG_ON);
      break;
    case DEVICE_3:
      interface->write(DISPLAY_MSG_SELECT_3);
      interface->write(DISPLAY_MSG_ON);
      break;
    case DEVICE_OFF:
      interface->write(DISPLAY_MSG_OFF);
      break;
  }
}

void setInitState() {
  setUSB(DEVICE_1);
  setDisplay(DISPLAY_1, DEVICE_1);
  setDisplay(DISPLAY_2, DEVICE_1);
}

void setup() {
  // Console
  Serial.begin(9600);
  // USB
  pinMode(USB_P0, OUTPUT);
  pinMode(USB_P1, OUTPUT);
  // Display 1
  displaySerial[DISPLAY_1] = new CustomSoftwareSerial(19, 18); // rx, tx
  displaySerial[DISPLAY_1]->begin(9600, CSERIAL_8N1);
  // Display 2
  displaySerial[DISPLAY_2] = new CustomSoftwareSerial(21, 20); // rx, tx
  displaySerial[DISPLAY_2]->begin(9600, CSERIAL_8N1);

  setInitState();
}

void loop() {
  key = kpd.getKey();
  if (key.state == KEY_DOWN && key.character > 0) {
    Serial.println( char( key.character ) ); // NOTE: Debug&notification message
    switch (key.character)
    {
      case '1':
        setUSB(DEVICE_1);
        setDisplay(DISPLAY_1, DEVICE_1);
        setDisplay(DISPLAY_2, DEVICE_1);
        break;
      case '2':
        setUSB(DEVICE_2);
        setDisplay(DISPLAY_1, DEVICE_2);
        setDisplay(DISPLAY_2, DEVICE_2);
        break;
      case '3':
        setUSB(DEVICE_3);
        setDisplay(DISPLAY_1, DEVICE_3);
        setDisplay(DISPLAY_2, DEVICE_3);
        break;
      case 'A':
        setUSB(DEVICE_OFF);
        setDisplay(DISPLAY_1, DEVICE_OFF);
        setDisplay(DISPLAY_2, DEVICE_OFF);
        break;
      case '4':
        setDisplay(DISPLAY_1, DEVICE_1);
        break;
      case '5':
        setDisplay(DISPLAY_1, DEVICE_2);
        break;
      case '6':
        setDisplay(DISPLAY_1, DEVICE_3);
        break;
      case 'B':
        setDisplay(DISPLAY_1, DEVICE_OFF);
        break;
      case '7':
        setDisplay(DISPLAY_2, DEVICE_1);
        break;
      case '8':
        setDisplay(DISPLAY_2, DEVICE_2);
        break;
      case '9':
        setDisplay(DISPLAY_2, DEVICE_3);
        break;
      case 'C':
        setDisplay(DISPLAY_2, DEVICE_OFF);
        break;
      case '*':
        setUSB(DEVICE_1);
        break;
      case '0':
        setUSB(DEVICE_2);
        break;
      case '#':
        setUSB(DEVICE_3);
        break;
      case 'D':
        setUSB(DEVICE_OFF);
        break;
    }
  }
}
