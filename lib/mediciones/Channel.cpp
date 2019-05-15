#include <Arduino.h>
#include <SPI.h>
#include <MemoryFree.h>
#include <Regexp.h>
#include <Array.h>
#include "Channel.h"



//Clase canal, se encargará de guardar la medición del canal, correspondiente.
Channel::Channel(){
}


Channel::Channel(int numCanal2, int port2){

  setNumCanal(numCanal2);
  setPort(port2);
}

Channel::~Channel(){
  valor.liberar();
  nulo.liberar();
  delete sadfaf;
}
void Channel::destruir(){
  valor.liberar();
  nulo.liberar();
  delete sadfaf;
}

void Channel::setNumCanal(int numCanal2){

  numCanal = numCanal2;
}

void Channel::setPort(int port2){

  port = port2;
}

Array<char> Channel::leer() {
      char aux[7];
      switch(port){
        case 1:
        {
          //Esta sección envía el código para medir el canal específicado.
          Serial1.write("KRDG? ");
          Serial1.print(numCanal);
          Serial1.write("\r\n");
          Serial1.println();
          //Esta sección recibe el valor de la temperatura y la devuelve como un
          //array de chars.
          if(Serial1.available()){
            int z= 0;
            char lakeshore;
            while((lakeshore = Serial1.read()) != '\r'){
              aux[z] = lakeshore;
              z++;
            }
            valor = Array<char>(aux, 7);
            return valor;
          }else{
            return nulo;
          }

        }
        break;
        case 2:
        {
          //Esta sección envía el código para medir el canal específicado.
          Serial2.write("KRDG? ");
          Serial2.print(numCanal);
          Serial2.write("\r\n");
          Serial2.println();
          delay(2);
          //Esta sección recibe el valor de la temperatura y la devuelve como un
          //array de chars.
          if(Serial2.available()){
            int z= 0;
            char lakeshore;
            while((lakeshore = Serial2.read()) != '\r'){
              aux[z] = lakeshore;
              z++;
            }
            valor = Array<char>(aux, 7);
            return valor;
          }else{

            return nulo;
          }

        }
        break;
        case 3:
        {
          //Esta sección envía el código para medir el canal específicado.
          Serial3.write("KRDG? ");
          Serial3.print(numCanal);
          Serial3.write("\r\n");
          Serial3.println();
          delay(2);
          //Esta sección recibe el valor de la temperatura y la devuelve como un
          //array de chars.
          if(Serial3.available()){
            int z= 0;
            char lakeshore;
            while((lakeshore = Serial3.read()) != '\r'){
              aux[z] = lakeshore;
              z++;
            }
            valor = Array<char>(aux, 7);
            return valor;
          }else{

            return nulo;
          }
        }
        default:

          return nulo;
      }

};
