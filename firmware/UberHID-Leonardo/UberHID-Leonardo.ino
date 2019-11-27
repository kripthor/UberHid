#include "Keyb.h"
#include "Mouse.h" 

//char USBKeyboardPage[] = {0,0,0,0,'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z','1','2','3','4','5','6','7','8','9','0','\n',0x1b,0xb2,'\t',' ','-','=','[',']','\\','#',';','\'','`',',','.','/',0,0xc2,0xc3,0xc4,0xc5,0xc6,0xc7,0xc8,0xc9,0xcA,0xcb,0xcc,0xcd,0,0,0,0xd1,0xd2,0xd3,0xd4,0xd5,0xd6,0xd7,0xd8,0xd9,0xda};

void setup() {
 Serial.begin(115200); 
 Serial1.begin(9600); 
 Serial.println("Init done");
}

void loop() {
  if (Serial1.available() >= 3) {
    char ctrlByte = Serial1.read();
    
    if (ctrlByte != 'U' && ctrlByte != 'D' && ctrlByte != 'M'  ) {
      return;
    }
    
    char bytesAscii[3];
    bytesAscii[0] =  Serial1.read();
    bytesAscii[1] =  Serial1.read();
    bytesAscii[2] =  '\0';

    // Keyboard UP or DOWN events
    if (ctrlByte == 'U' || ctrlByte == 'D' ) {
    char pressed = (unsigned char) strtoul(bytesAscii,NULL,16);

/*    if (ctrlByte == 'U') Keyboard.release(pressed);
    if (ctrlByte == 'D') Keyboard.press(pressed);
*/
    if (ctrlByte == 'U') {
      if (bytesAscii[0] == 'L') {
          if (bytesAscii[1] == 'C') Keyboard.releaseMod(0);
          if (bytesAscii[1] == 'S') Keyboard.releaseMod(1);
          if (bytesAscii[1] == 'A') Keyboard.releaseMod(2);
          if (bytesAscii[1] == 'G') Keyboard.releaseMod(3);
      }
      else if (bytesAscii[0] == 'R') {
          if (bytesAscii[1] == 'C') Keyboard.releaseMod(4);
          if (bytesAscii[1] == 'S') Keyboard.releaseMod(5);
          if (bytesAscii[1] == 'A') Keyboard.releaseMod(6);
          if (bytesAscii[1] == 'G') Keyboard.releaseMod(7);
      }
      else Keyboard.releaseRaw(pressed);
    }

    if (ctrlByte == 'D') {
      if (bytesAscii[0] == 'L') {
          if (bytesAscii[1] == 'C') Keyboard.pressMod(0);
          if (bytesAscii[1] == 'S') Keyboard.pressMod(1);
          if (bytesAscii[1] == 'A') Keyboard.pressMod(2);
          if (bytesAscii[1] == 'G') Keyboard.pressMod(3);
      }
      else if (bytesAscii[0] == 'R') {
          if (bytesAscii[1] == 'C') Keyboard.pressMod(4);
          if (bytesAscii[1] == 'S') Keyboard.pressMod(5);
          if (bytesAscii[1] == 'A') Keyboard.pressMod(6);
          if (bytesAscii[1] == 'G') Keyboard.pressMod(7);
      }
      else Keyboard.pressRaw(pressed);
    }
    
    Serial.print(ctrlByte);
    Serial.println(pressed,HEX);

    }

    
    // Mouse events
    // TODO press and release. Drop M: X,Y,W is enough to encode and we have 2 bytes for offset
    if (ctrlByte == 'M') {
      signed int val = (signed int) bytesAscii[1];
      switch(bytesAscii[0]) {
        case 'C': if (bytesAscii[1] == '1') Mouse.click(MOUSE_LEFT);
                  if (bytesAscii[1] == '2') Mouse.click(MOUSE_RIGHT);
                  if (bytesAscii[1] == '3') Mouse.click(MOUSE_MIDDLE);
                  break;
        case 'X': Mouse.move(val,0,0);
                  break;
        case 'Y': Mouse.move(0,val,0);
                  break;
        case 'W': Mouse.move(0,0,val);
                  break;
        default:  break;
        
      }
    }

    // Reset events
    if (ctrlByte == 'R') {
        Keyboard.releaseAll();
    }
    
    
  }
  
 
}
