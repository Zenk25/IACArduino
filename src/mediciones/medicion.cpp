#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>
#include <SD.h>
#include <TimerOne.h>
#include <MemoryFree.h>
#include <Regexp.h>
/*
void medicionCanal1(int canales []){
  //canales = {1,2,4,5};

  for
  int aux = 0;
  Serial.write("KRDG?\r\n");
  Serial.println();
  delay(2);
  char temp [8][7];
  char lakeshore;
  int j = 1, z = 0;
  while(Serial.available()){
    MatchState ms;
    for (int i = 0; i < 8; i++) {
      z=0;
      while(z < 7) {
        if ((lakeshore = Serial.read())==',') {
          lakeshore = Serial.read();
        }
        temp[i][z] = lakeshore;
        z++;
      }
      ms.Target(temp[i]);

      char result = ms.Match("[+-][0-9]*%.[0-9]*", 0);
      if (result == REGEXP_MATCHED){
        aux++;
      }else if (result == REGEXP_NOMATCH){
      }
    }
    if(aux == 8){
      stop = true;
    }else{
      stop = false;
    }
  }

  if(SD.exists("lake.jso") && puede3 && stop){
   File paraComa = SD.open("lake.jso", FILE_WRITE);
   paraComa.println(",");
   paraComa.close();
  }


  File lakeshoreData = SD.open("lake.jso",FILE_WRITE);

    if(stop){
      lakeshoreData.print("{");
      lakeshoreData.print("\"secs\":\"");
      lakeshoreData.print(secs);
      lakeshoreData.print("\",");
    }
    secs += 5;
    z=0;
    while(z < 8){
      if(stop){
        lakeshoreData.print("\"Temp");
        lakeshoreData.print(j);
        lakeshoreData.print("\":\"");
        for (size_t i = 0; i < 7; i++) {
          lakeshoreData.print(temp[z][i]);
        }
      }
      if(stop){
        if (z < 7){
          lakeshoreData.print("\",");
        }else{
          lakeshoreData.print("\"");
        }
      }
      ++j;
      ++z;
    }
    if(stop){
      lakeshoreData.print("}");
      puede = true;
    }

    lakeshoreData.close();

  break;

}
void mediciones(int canal){
  switch(canal){
    case 1:
    {

    }
    case 2:
    {
      int aux = 0;
      Serial2.write("KRDG?\r\n");
      Serial2.println();
      delay(2);
      char temp [8][7];
      char lakeshore;
      int j = 1, z = 0;

      while(Serial2.available()){
        MatchState ms;
        for (int i = 0; i < 8; i++) {
          z=0;
          while(z < 7) {
            if ((lakeshore = Serial2.read())==',') {
              lakeshore = Serial2.read();
            }
            temp[i][z] = lakeshore;
            z++;
          }
          ms.Target(temp[i]);

          char result = ms.Match("[+-][0-9]*%.[0-9]*", 0);
          if (result == REGEXP_MATCHED){
            aux++;
          }else if (result == REGEXP_NOMATCH){
          }
        }
        if(aux == 8){
          stop2 = true;
        }else{
          stop2 = false;
        }
      }

      if(SD.exists("lake2.jso") && puede2 && stop2){
       File paraComa = SD.open("lake2.jso", FILE_WRITE);
       paraComa.println(",");
       paraComa.close();
      }


      File lakeshoreData = SD.open("lake2.jso",FILE_WRITE);

        if(stop2){
          lakeshoreData.print("{");
          lakeshoreData.print("\"secs\":\"");
          lakeshoreData.print(secs);
          lakeshoreData.print("\",");
        }
        secs += 5;
        z=0;
        while(z < 8){
          if(stop2){
            lakeshoreData.print("\"Temp");
            lakeshoreData.print(j);
            lakeshoreData.print("\":\"");
            for (size_t i = 0; i < 7; i++) {
              lakeshoreData.print(temp[z][i]);
            }
          }
          if(stop2){
            if (z < 7){
              lakeshoreData.print("\",");
            }else{
              lakeshoreData.print("\"");
            }
          }
          ++j;
          ++z;
        }
        if(stop2){
          lakeshoreData.print("}");
          puede2 = true;
        }

        lakeshoreData.close();

      break;
    }
    case 3:
    {
      int aux = 0;
      Serial3.write("KRDG?\r\n");
      Serial3.println();
      delay(2);
      char temp [8][7];
      char lakeshore;
      int j = 1, z = 0;

      while(Serial3.available()){
        MatchState ms;
        for (int i = 0; i < 8; i++) {
          z=0;
          while(z < 7) {
            if ((lakeshore = Serial3.read())==',') {
              lakeshore = Serial3.read();
            }
            temp[i][z] = lakeshore;
            z++;
          }
          ms.Target(temp[i]);

          char result = ms.Match("[+-][0-9]*%.[0-9]*", 0);
          if (result == REGEXP_MATCHED){
            aux++;
          }else if (result == REGEXP_NOMATCH){
          }
        }
        if(aux == 8){
          stop3 = true;
        }else{
          stop3 = false;
        }
      }

      if(SD.exists("lake3.jso") && puede3 && stop3){
       File paraComa = SD.open("lake3.jso", FILE_WRITE);
       paraComa.println(",");
       paraComa.close();
      }


      File lakeshoreData = SD.open("lake3.jso",FILE_WRITE);

        if(stop3){
          lakeshoreData.print("{");
          lakeshoreData.print("\"secs\":\"");
          lakeshoreData.print(secs);
          lakeshoreData.print("\",");
        }
        secs += 5;
        z=0;
        while(z < 8){
          if(stop3){
            lakeshoreData.print("\"Temp");
            lakeshoreData.print(j);
            lakeshoreData.print("\":\"");
            for (size_t i = 0; i < 7; i++) {
              lakeshoreData.print(temp[z][i]);
            }
          }
          if(stop3){
            if (z < 7){
              lakeshoreData.print("\",");
            }else{
              lakeshoreData.print("\"");
            }
          }
          ++j;
          ++z;
        }
        if(stop3){
          lakeshoreData.print("}");
          puede3 = true;
        }

        lakeshoreData.close();

      break;
    }
    default:
      Serial.write("Uys algo fui mal.");
  }
  Serial.print("FreeMemory() = ");
  Serial.println(freeMemory());
}
*/
