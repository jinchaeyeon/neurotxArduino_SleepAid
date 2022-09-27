/*
  neuro TX v0.1 : MAX30102, MPU6050

  Timer0 : main Loop
  Timer1 : 16bit timer for servo library ==> use this timer
  Timer2 : 8bit timer for tone()

  ch1 : pls 6,7    charge a0
  ch2 : pls 8,9    charge a1
  ch3 : pls 10,11  charge a2
  ch4 : pls 12,13  charge a3

  charge pls control by 20us Timer
  other pls control at main loop, by microseconddelay()

*/
#define mySerial Serial2

#ifndef STM32F103CNeuroTXClass_H
#define STM32F103CNeuroTXClass_H

#include <Wire.h>
#include <Adafruit_MCP4725.h>

//channel #1
#define CH1_T_EN      PA5   //PD5
#define CH1_P_EN      PA6   //PD4
#define CH1_N_EN      PA7   //PD5

//channel #2
#define CH2_T_EN      PA8   //
#define CH2_P_EN      PA9   //
#define CH2_N_EN      PA10   //

#define HIV_EN        7   //PD7

/**************** Serial Command ****************************/
#define GET_PLS_DUTY_CH1 101
#define GET_PLS_DUTY_CH2 201

#define SET_PLS_DUTY_CH1 102
#define SET_PLS_DUTY_CH2 202

#define GET_PLS_DURATION_CH1 103
#define GET_PLS_DURATION_CH2 203

#define SET_PLS_DURATION_CH1 104
#define SET_PLS_DURATION_CH2 204

#define GET_PLS_AMP_CH1 105
#define GET_PLS_AMP_CH2 205

#define SET_PLS_AMP_CH1 106
#define SET_PLS_AMP_CH2 206

#define GET_PLS_POLAR_CH1 107
#define GET_PLS_POLAR_CH2 207

#define SET_PLS_POLAR_CH1 108
#define SET_PLS_POLAR_CH2 208

#define GET_PLS_LONG_CH1 111
#define GET_PLS_LONG_CH2 211

#define SET_PLS_LONG_CH1 110
#define SET_PLS_LONG_CH2 210

#define SET_PLS_OUTPUT_CH1 109
#define SET_PLS_OUTPUT_CH2 209

#define SET_PLS_EN 909
#define SET_DATA_OUTPUT 910
#define GET_STATUS 911

class STM32F103CNeuroTXClass {

  private:
    /**************** Serial Command ****************************/
    unsigned long PlsDuty1, PlsDuty2;
    unsigned long PlsDuration1, PlsDuration2;
    unsigned long PlsLONG1, PlsLONG2;
    uint16_t      PlsAmp1, PlsAmp2;
    int PlsPolar1, PlsPolar2;

    int PlsOut1, PlsOut2;
    int PlsOn1, PlsOn2;
    int DataOut, PlsEn;
    int count, count2;
    int sleeptime;

    unsigned long currentMillis, ch1_startMillis, ch2_startMillis;

    //String dataValue;
    unsigned long receiveData[2];

    int16_t Bat_Val;
    int16_t sequence;

    Adafruit_MCP4725 dac, dac1;

    String dataValue;

  public:

    void setup()   {

      //dac =  new Adafruit_MCP4725();
      //dac1 =  new Adafruit_MCP4725();

      //dataValue = "";
      PlsDuty1 = 200;      PlsDuty2 = 200;
      PlsDuration1 = 20;   PlsDuration2 = 20;
      PlsAmp1 = 80;        PlsAmp2 = 80;
      PlsPolar1 = 1;       PlsPolar2 = 1;
      PlsOut1 = 0;         PlsOut2 = 0;
      PlsLONG1 = 80;       PlsLONG2 = 80;
      DataOut = 0;

      sequence = 0;
      count = 1;
      count2 = 1;
      sleeptime = 0;

      // configure Arduino pin for output
      pinMode (CH1_P_EN, OUTPUT);
      pinMode (CH1_N_EN, OUTPUT);
      pinMode (CH1_T_EN, OUTPUT);
      pinMode (CH2_P_EN, OUTPUT);
      pinMode (CH2_N_EN, OUTPUT);
      pinMode (CH2_T_EN, OUTPUT);
      pinMode (HIV_EN ,  OUTPUT);

      digitalWrite (CH1_P_EN, LOW);
      digitalWrite (CH1_N_EN, LOW);
      digitalWrite (CH1_T_EN, HIGH);
      digitalWrite (CH2_P_EN, LOW);
      digitalWrite (CH2_N_EN, LOW);
      digitalWrite (CH2_T_EN, HIGH);
      digitalWrite (HIV_EN,   HIGH);

      // For Adafruit MCP4725A1 the address is 0x62 (default) or 0x63 (ADDR pin tied to VCC)
      // For MCP4725A0 the address is 0x60 or 0x61
      // For MCP4725A2 the address is 0x64 or 0x65

      Wire.setSDA(PB11);
      Wire.setSCL(PB10);

      dac.begin(0x60);
      delay(1);
      dac1.begin(0x61);
      delay(1);
    }

    void loop()
    {
      serialEvent();
      if (DataOut < 2) {
        digitalWrite(CH1_P_EN, HIGH);
        digitalWrite(CH1_N_EN, HIGH);
        digitalWrite(CH2_P_EN, HIGH);
        digitalWrite(CH2_N_EN, HIGH);
      }
      else {
        currentMillis = millis();
        if (count > 4) {
          if (currentMillis - ch1_startMillis >= PlsLONG1) {
            count = 1;
          }
        } else {
          if (currentMillis - ch1_startMillis >= PlsDuration1) {
            PlsOn1 = 1;
            ch1_startMillis = currentMillis;
          }

          if (PlsOn1 == 1) {
            digitalWrite(CH1_P_EN, PlsPolar1);
            digitalWrite(CH1_N_EN, !PlsPolar1);
            delayMicroseconds (PlsDuty1 - 2);
            //digitalWrite(CH1_P_EN, !PlsPolar1);
            //digitalWrite(CH1_N_EN, PlsPolar1);
            digitalWrite(CH1_P_EN, HIGH);
            digitalWrite(CH1_N_EN, HIGH);
            PlsOn1 = 0;
            count++;
          }
        }
        if (count2 > 4) {
          if (currentMillis - ch2_startMillis >= PlsLONG2) {
            count2 = 1;
            ch2_startMillis = currentMillis;
          }
        } else {
          if (currentMillis - ch2_startMillis >= PlsDuration2) {
            PlsOn2 = 1;
          }

          if (PlsOn2 == 1) {
            digitalWrite(CH2_P_EN, PlsPolar2);
            digitalWrite(CH2_N_EN, !PlsPolar2);
            delayMicroseconds (PlsDuty2 - 2);
            //digitalWrite(CH2_P_EN, !PlsPolar2);
            //digitalWrite(CH2_N_EN, PlsPolar2);
            digitalWrite(CH2_P_EN, HIGH);
            digitalWrite(CH2_N_EN, HIGH);
            PlsOn2 = 0;
            count2++;
          }
        }
        currentMillis = millis();
      }
    }

    void Control(String dataValue) {
      String command = dataValue.substring(0, dataValue.length() - 1);
      Split(command, '|');
      if ( receiveData[0] <= 0) return;

      switch (receiveData[0]) {
        case GET_PLS_DUTY_CH1:
          mySerial.println(String(GET_PLS_DUTY_CH1) + '|' + String(PlsDuty1));
          break;
        case SET_PLS_DUTY_CH1:
          PlsDuty1 = receiveData[1];
          if (PlsDuty1 < 10 ) PlsDuty1 = 10;
          mySerial.println(String(SET_PLS_DUTY_CH1) + '|' + String(PlsDuty1));
          //EEPROM.write(0,auto_off);
          break;
        case GET_PLS_DUTY_CH2:
          mySerial.println(String(GET_PLS_DUTY_CH2) + '|' + String(PlsDuty2));
          break;
        case SET_PLS_DUTY_CH2:
          PlsDuty2 = receiveData[1];
          if (PlsDuty2 < 10 ) PlsDuty2 = 10;
          mySerial.println(String(SET_PLS_DUTY_CH2) + '|' + String(PlsDuty2));
          break;

        case GET_PLS_DURATION_CH1:
          mySerial.println(String(GET_PLS_DURATION_CH1) + '|' + String(PlsDuration1));
          break;
        case SET_PLS_DURATION_CH1:
          PlsDuration1 = receiveData[1];
          mySerial.println(String(SET_PLS_DURATION_CH1) + '|' + String(PlsDuration1));
          //EEPROM.write(0,auto_off);
          break;
        case GET_PLS_DURATION_CH2:
          mySerial.println(String(GET_PLS_DURATION_CH2) + '|' + String(PlsDuration2));
          break;
        case SET_PLS_DURATION_CH2:
          PlsDuration2 = receiveData[1];
          mySerial.println(String(SET_PLS_DURATION_CH2) + '|' + String(PlsDuration2));

        case GET_PLS_AMP_CH1:
          mySerial.println(String(GET_PLS_AMP_CH1) + '|' + String(PlsAmp1));
          break;
        case SET_PLS_AMP_CH1:
          PlsAmp1 = receiveData[1];
          mySerial.println(String(SET_PLS_AMP_CH1) + '|' + String(PlsAmp1));
          dac.setVoltage(PlsAmp1, false);
          //EEPROM.write(0,auto_off);
          break;
        case GET_PLS_AMP_CH2:
          mySerial.println(String(GET_PLS_AMP_CH2) + '|' + String(PlsAmp2));
          break;
        case SET_PLS_AMP_CH2:
          PlsAmp2 = receiveData[1];
          mySerial.println(String(SET_PLS_AMP_CH2) + '|' + String(PlsAmp2));
          dac1.setVoltage(PlsAmp2, false);
          break;

        case GET_PLS_POLAR_CH1:
          mySerial.println(String(GET_PLS_POLAR_CH1) + '|' + String(PlsPolar1));
          break;
        case SET_PLS_POLAR_CH1:
          PlsPolar1 = receiveData[1];
          //CH1_OUTPIN, CH2_OUTPIN  // CH1 5 or 15, CH2 10 or 16
          mySerial.println(String(SET_PLS_POLAR_CH1) + '|' + String(PlsPolar1));
          //EEPROM.write(0,auto_off);
          break;
        case GET_PLS_POLAR_CH2:
          mySerial.println(String(GET_PLS_POLAR_CH2) + '|' + String(PlsPolar2));
          break;
        case SET_PLS_POLAR_CH2:
          PlsPolar2 = receiveData[1];
          //CH1_OUTPIN, CH2_OUTPIN  // CH1 5 or 15, CH2 10 or 16
          mySerial.println(String(SET_PLS_POLAR_CH2) + '|' + String(PlsPolar2));
          break;

        case GET_PLS_LONG_CH1:
          mySerial.println(String(GET_PLS_LONG_CH1) + '|' + String(PlsLONG1));
          break;
        case SET_PLS_LONG_CH1:
          PlsLONG1 = receiveData[1];
          mySerial.println(String(SET_PLS_LONG_CH1) + '|' + String(PlsLONG1));
          //EEPROM.write(0,auto_off);
          break;
        case GET_PLS_LONG_CH2:
          mySerial.println(String(GET_PLS_LONG_CH2) + '|' + String(PlsLONG2));
          break;
        case SET_PLS_LONG_CH2:
          PlsLONG2 = receiveData[1];
          mySerial.println(String(SET_PLS_LONG_CH2) + '|' + String(PlsLONG2));
          
        case SET_PLS_OUTPUT_CH1:
          PlsOut1 = receiveData[1];
          mySerial.println(String(SET_PLS_OUTPUT_CH1) + '|' + String(PlsOut1));
          //EEPROM.write(0,auto_off);
          break;
        case SET_PLS_OUTPUT_CH2:
          PlsOut2 = receiveData[1];
          mySerial.println(String(SET_PLS_OUTPUT_CH2) + '|' + String(PlsOut2));
          break;

        case SET_PLS_EN:
          PlsEn = receiveData[1];
          mySerial.println(String(SET_PLS_EN) + '|' + String(PlsEn));
          break;

        case SET_DATA_OUTPUT:
          DataOut = receiveData[1];
          mySerial.println(String(SET_DATA_OUTPUT) + '|' + String(DataOut));
          break;

        case GET_STATUS:
          mySerial.print("Mode  : "); mySerial.print(DataOut); mySerial.print("   Pls En : "); mySerial.print(PlsEn); mySerial.print("   Pls Out1 : "); mySerial.print(PlsOut1); mySerial.print("   Pls Out2 : "); mySerial.print(PlsOut2);
          mySerial.print("   Amplitude : "); mySerial.println(PlsAmp1);

          break;

        default:
          break;

      }
      return;
    }

    void test() {
      Control("102|200\r\n");
      Control("104|200\r\n");
      Control("106|4095\r\n");
      Control("108|1\r\n");
      Control("910|1\r\n");
    }

    void test2() {
      currentMillis = millis();
      if (currentMillis - ch1_startMillis >= PlsDuration1) {
        PlsOn1 = 1;
        ch1_startMillis = currentMillis;
      }
      if (currentMillis - ch2_startMillis >= PlsDuration2) {
        PlsOn2 = 1;
        ch2_startMillis = currentMillis;
      }

      if (PlsOn1 == 1) {
        digitalWrite(CH1_P_EN, PlsPolar1);
        digitalWrite(CH1_N_EN, !PlsPolar1);
        delayMicroseconds (PlsDuty1 - 2);
        digitalWrite(CH1_P_EN, !PlsPolar1);
        digitalWrite(CH1_N_EN, PlsPolar1);
        PlsOn1 = 0;
      }

      if (PlsOn2 == 1) {
        digitalWrite(CH2_P_EN, PlsPolar2);
        digitalWrite(CH2_N_EN, !PlsPolar2);
        delayMicroseconds (PlsDuty2 - 2);
        digitalWrite(CH2_P_EN, !PlsPolar2);
        digitalWrite(CH2_N_EN, PlsPolar2);
        PlsOn2 = 0;
      }
      currentMillis = millis();
    }


    /*
      void Split(String sData, char cSeparator) {
        receiveData[0] = -1;
        receiveData[1] = -1;
        int nCount = 0;
        int nGetIndex = 0 ;
        String sTemp = "";
        String sCopy = sData;
        nGetIndex = sCopy.indexOf(cSeparator);
        sTemp = sCopy.substring(0, nGetIndex);
        receiveData[0] = sTemp.toInt();
        sCopy = sCopy.substring(nGetIndex + 1);

        nGetIndex = sCopy.indexOf(cSeparator);
        sTemp = sCopy.substring(0, nGetIndex);
        receiveData[1] = sTemp.toInt();
      }
    */

    void Split(String sData, char cSeparator) {
      receiveData[0] = -1;
      receiveData[1] = -1;
      int nCount = 0;
      int nGetIndex = 0 ;
      String sTemp = "";
      String sCopy = sData;
      nGetIndex = sCopy.indexOf(cSeparator);
      if (nGetIndex >= 0) {
        sTemp = sCopy.substring(0, nGetIndex);
        receiveData[0] = sTemp.toInt();
        sCopy = sCopy.substring(nGetIndex + 1);
        receiveData[1] = sCopy.toInt();
      }
    }

    void serialEvent() {
      int inChar;
      while (mySerial.available() > 0) {
        inChar = mySerial.read();
        dataValue += (char)inChar;
        mySerial.print((char)inChar);   //Echo char

        // if you get a newline, print the string, then the string's value:
        if (inChar == '\n') {
          if (dataValue.length() > 1) {
            Control(dataValue);
            dataValue = "";
          }
        }
      }
    }

};

STM32F103CNeuroTXClass STM32F103CNeuroTX;

#endif //STM32F103CNeuroTXClass_H
