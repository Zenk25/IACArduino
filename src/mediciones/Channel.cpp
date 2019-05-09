#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>
#include <SD.h>
#include <TimerOne.h>
#include <MemoryFree.h>
#include <Regexp.h>

using namespace std;
//Clase canal, se encargará de guardar la medición del canal, correspondiente.
class Channel {
   private:
      // Datos de la medición.
      char [7] valor;
      //Numero del canal actual.
      int numCanal;
      int port;

   public:
      // Funcion que devuelve el codigo para la lectura
      void codigoPeticion();
      char* leer();

};

char* Channel::leer() {

  switch(port){
    case 1:
    {
      //Esta sección envía el código para medir el canal específicado.
      Serial1.write("KRDG ");
      Serial1.write(numCanal);
      Serial1.write("\r\n");
      Serial1.println();
      delay(2);

      if(Serial1.available()){
        MatchState ms;
        for (int i = 0; i < 7; i++) {
          valor[i] = Serial1.read();
        }
        ms.Target(valor);
        char result = ms.Match("^[+-][0-9]*[%.][0-9]*", 0);
        if (result != REGEXP_MATCHED){
          return -1;
        }
        return valor;
      }

    }
    case 2:
    {
      //Esta sección envía el código para medir el canal específicado.
      Serial2.write("KRDG ");
      Serial2.write(numCanal);
      Serial2.write("\r\n");
      Serial2.println();
      delay(2);

      if(Serial2.available()){
        MatchState ms;
        for (int i = 0; i < 7; i++) {
          valor[i] = Serial2.read();
        }
        ms.Target(valor);
        char result = ms.Match("^[+-][0-9]*[%.][0-9]*", 0);
        if (result != REGEXP_MATCHED){
          return -1;
        }
        return valor;
      }

    }
    case 3:
    {
      //Esta sección envía el código para medir el canal específicado.
      Serial3.write("KRDG ");
      Serial3.write(numCanal);
      Serial3.write("\r\n");
      Serial3.println();
      delay(2);

      if(Serial3.available()){
        MatchState ms;
        for (int i = 0; i < 7; i++) {
          valor[i] = Serial3.read();
        }
        ms.Target(valor);
        char result = ms.Match("^[+-][0-9]*[%.][0-9]*", 0);
        if (result != REGEXP_MATCHED){
          return -1;
        }
        return valor;
      }

    }
  }
}
