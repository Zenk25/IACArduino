#include <Arduino.h>
#include <Ethernet.h>
#include <SPI.h>
#include <Regexp.h>
#include <MemoryFree.h>
#include "mediciones\Channel.h"

byte mac[] = {
  0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02
};


void setup(){
  Serial.begin(9600);
  Serial1.begin(9600,SERIAL_7O1);
  Serial2.begin(9600,SERIAL_7O1);

  if (Ethernet.begin(mac) == 0) {
    // Check for Ethernet hardware present
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {

      while (true) {
        delay(1); // do nothing, no point running without Ethernet hardware
      }
    }
    if (Ethernet.linkStatus() == LinkOFF) {
    }
  }
  Serial.print(Ethernet.localIP());
  //Serial.println("waiting for sync");
  //setSyncProvider(getNtpTime());

}

void loop() {

  Channel prueba(1,1),prueba2(2,1),prueba3(3,1),prueba4(4,1),prueba5(5,1),prueba6(6,1),prueba7(7,1),prueba8(8,1);
  Serial.print("\nFreeMemory() = ");
  Serial.println(freeMemory());
  /*int numCanal = 1;
  Serial2.write("KRDG? ");
  Serial2.print(numCanal);
  Serial2.write("\r\n");
  Serial2.println();
  delay(2);
  Serial.write("\n");
  while(Serial2.available()){
    Serial.write(Serial2.read());
  }*/

    MatchState ms;
    char* temp;
    temp= prueba.leer();

    ms.Target(temp);
    char result = ms.Match("[+-][0-9]*[.][0-9]*");
      if (result == REGEXP_MATCHED){
        Serial.write(temp);
      }
      delay(1000);
     temp= prueba2.leer();

    ms.Target(temp);
    result = ms.Match("[+-][0-9]*[.][0-9]*");
      if (result == REGEXP_MATCHED){
        Serial.write(temp);
      }

    delay(1000);
    temp= prueba3.leer();

    ms.Target(temp);
    result = ms.Match("[+-][0-9]*[.][0-9]*");
      if (result == REGEXP_MATCHED){
        Serial.write(temp);
      }
      delay(1000);
    temp= prueba4.leer();

    ms.Target(temp);
    result = ms.Match("[+-][0-9]*[.][0-9]*");
      if (result == REGEXP_MATCHED){
        Serial.write(temp);
      }
      delay(1000);
    temp= prueba5.leer();

    ms.Target(temp);
    result = ms.Match("[+-][0-9]*[.][0-9]*");
      if (result == REGEXP_MATCHED){
        Serial.write(temp);
      }
      delay(1000);
    temp= prueba6.leer();

    ms.Target(temp);
    result = ms.Match("[+-][0-9]*[.][0-9]*");
      if (result == REGEXP_MATCHED){
        Serial.write(temp);
      }
      delay(1000);
    temp= prueba7.leer();

    ms.Target(temp);
    result = ms.Match("[+-][0-9]*[.][0-9]*");
      if (result == REGEXP_MATCHED){
        Serial.write(temp);
      }
      delay(1000);
    temp= prueba8.leer();

    ms.Target(temp);
    result = ms.Match("[+-][0-9]*[.][0-9]*");
      if (result == REGEXP_MATCHED){
        Serial.write(temp);
      }
      delay(1000);
      
    Serial.print("\nFreeMemory() = ");
    Serial.println(freeMemory());
}
