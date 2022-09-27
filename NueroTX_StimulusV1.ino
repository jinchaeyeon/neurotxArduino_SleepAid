/*
product : NeuroTx_Stimulus module
board : STM32F103C 
method : jllink

check jlink upload application in stm32f hardware
2021.07.31
*/

#include "STM32F103CNeuroTXClass.h"


void setup() {
  mySerial.begin(9600);
  mySerial.println("Initializing...");

  STM32F103CNeuroTX.setup();
  STM32F103CNeuroTX.test();
  
}

void loop() {
  STM32F103CNeuroTX.loop();
}
