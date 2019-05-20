#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>
#include <SD.h>
#include <MemoryFree.h>
#include <Regexp.h>

//Se podrá poner comprobadores de espacios y demás errores a la hora de crear el config.ini,
//por ahora no estan implementados.
unsigned long oldtime = 0;
unsigned long epoch;
//Declaración variables para el Header, en el setup se inicializarán.
String subsystem = "", cicle = "",responsible = "", objective = "", baseDirectory = "", backupBaseDirectory = "", tempPeriod = "", pressurePeriod = "";
//Declaración variables para el Monitor 1, en el setup se inicializarán.
String port1 = "", magnitude = "", type = "";
const int CANALESSIZE = 8;
String canales1[8] = {"Temp1","Temp2","Temp3","Temp4","Temp5","Temp6","Temp7","Temp8"};
String temperaturas = "";
// telnet defaults to port 23
EthernetServer server(80);
boolean puede = false, puede2= false, puede3= false, stop = false, stop2 = false, stop3 = false;
int secs = 0;

unsigned int localPort = 8888;
const int NTP_PACKET_SIZE = 48;
char timeServer[] = "time.nist.gov";
byte packetBuffer[NTP_PACKET_SIZE];

EthernetUDP Udp;

//Esta clase será necesaria para obtener la fecha desde un servidor NTP
void sendNTPpacket(){
  Serial.println("Enviando...");
    // Pone todos los bytes del buffer a 0
    memset(packetBuffer, 0, NTP_PACKET_SIZE);
    // Inicializa las variables necesarias para un paquete NTP
    packetBuffer[0] = 0b11100011;   // LI, Version, Mode
    packetBuffer[1] = 0;     // Stratum, or type of clock
    packetBuffer[2] = 6;     // Polling Interval
    packetBuffer[3] = 0xEC;  // Peer Clock Precision
    // 8 bytes a cero para Root Delay & Root Dispersion
    packetBuffer[12]  = 49;
    packetBuffer[13]  = 0x4E;
    packetBuffer[14]  = 49;
    packetBuffer[15]  = 52;
    Serial.println(freeMemory());

    SD.end();
    // todos los campos del NTP tienen valores, asíque ahora
    // puedes enviar un mensaje pidiendo la fecha y hora
    if((Udp.beginPacket(timeServer, 123))!=1){
      delay(1000);
      Serial.println(Udp.beginPacket(timeServer,123));
      //Si falla que no escriba la coma.
    }
    //los pedidos NTP se hacen a través del puerto 123

    Serial.println(Udp.write(packetBuffer, NTP_PACKET_SIZE));

    Serial.println(Udp.endPacket());

    SD.begin(4);
}

//Esta clase se encargará de recoger las mediciones de los canales.
void mediciones(int puerto);
unsigned long getTimeEpoch();

void setup() {
  byte mac[] = {
    0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02
  };
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial1.begin(9600,SERIAL_7O1);
  Serial2.begin(9600,SERIAL_7O1);
  Serial3.begin(9600,SERIAL_7O1);
  //Iniciada la SDCard
   if (!SD.begin(4)) {
       return;    // init failed
   }
   // check for index.htm file
   /*if (!SD.exists("lineas/index.htm")) {
     Serial.write("NANI");
       return;  // can't find index file
   }*/

   if(SD.exists("lake.jso")){
     SD.remove("lake.jso");
   }
   if(SD.exists("lake2.jso")){
     SD.remove("lake2.jso");
   }
   if(SD.exists("lake3.jso")){
     SD.remove("lake3.jso");
   }
   secs = 0;

  // You can use Ethernet.init(pin) to configure the CS pin
  //Ethernet.init(10);  // Most Arduino shields
  //Ethernet.init(5);   // MKR ETH shield
  //Ethernet.init(0);   // Teensy 2.0
  //Ethernet.init(20);  // Teensy++ 2.0
  //Ethernet.init(15);  // ESP8266 with Adafruit Featherwing Ethernet
  //Ethernet.init(33);  // ESP32 with Adafruit Featherwing Ethernet

  // start the Ethernet connection:
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
  server.begin();
  Udp.begin(localPort);
  EthernetClient client = server.available();
  if (client) {
    boolean currentLineIsBlank = true;
    String clientRequest = "";

    while (client.connected()) {
        char d;
        while(client.available()){
          d = client.read();
          clientRequest += d;
          if(clientRequest.indexOf("/ajax-upload")>0 && currentLineIsBlank && d== '\n'){
            if(clientRequest.indexOf("[Header]")>0){
              int index;
              if ((index = clientRequest.lastIndexOf("Subsystem= "))>0) {
                int lastindex = clientRequest.indexOf("\nCicle");
                subsystem = clientRequest.substring(index, lastindex);
              }
              if((index = clientRequest.lastIndexOf("Cicle= "))>0){
                int lastindex = clientRequest.indexOf("\nResponsible");
                cicle = clientRequest.substring(index,lastindex);
              }
              if((index = clientRequest.lastIndexOf("Responsible= "))>0){
                int lastindex = clientRequest.indexOf("\nObjective");
                responsible = clientRequest.substring(index,lastindex);
              }
              if((index = clientRequest.lastIndexOf("Objective= "))>0){
                int lastindex = clientRequest.indexOf("\nBaseDirectory");
                objective = clientRequest.substring(index,lastindex);
              }
              if((index = clientRequest.lastIndexOf("BaseDirectory= "))>0){
                int lastindex = clientRequest.indexOf("\nbackupBaseDirectory");
                baseDirectory = clientRequest.substring(index,lastindex);
              }
              if((index = clientRequest.lastIndexOf("backupBaseDirectory= "))>0){
                int lastindex = clientRequest.indexOf("\nTemperaturePeriod");
                backupBaseDirectory = clientRequest.substring(index,lastindex);
              }
              if((index = clientRequest.lastIndexOf("TemperaturePeriod= "))>0){
                int lastindex = clientRequest.indexOf("\nPressurePeriod");
                tempPeriod = clientRequest.substring(index,lastindex);
              }
              if ((index = clientRequest.lastIndexOf("PressurePeriod= "))>0){
                int lastindex = clientRequest.indexOf("\n[MONITOR1]");
                pressurePeriod = clientRequest.substring(index,lastindex);
                clientRequest = "[MONITOR1]\n";
              }
            }
            if(clientRequest.indexOf("[MONITOR1]")>0){
              int index;
              if((index = clientRequest.lastIndexOf("PORT= "))>0){
                int lastindex = clientRequest.indexOf("\nMagnitude");
                port1 = clientRequest.substring(index,lastindex);
              }
              if((index = clientRequest.lastIndexOf("Magnitude= "))>0){
                int lastindex = clientRequest.indexOf("\nType");
                magnitude = clientRequest.substring(index,lastindex);
              }
              if((index = clientRequest.lastIndexOf("Type= "))>0){
                int lastindex = clientRequest.indexOf("\nCHANNEL1");
                type = clientRequest.substring(index,lastindex);
              }
              if((index = clientRequest.lastIndexOf("CHANNEL1= "))>0){
                int lastindex = clientRequest.indexOf("\nCHANNEL2");
                if(!clientRequest.substring(index,lastindex).equals("")){
                  //canales1[0] = clientRequest.substring(index,lastindex);
                }
              }
              if((index = clientRequest.lastIndexOf("CHANNEL2= "))>0){
                int lastindex = clientRequest.indexOf("\nCHANNEL3");
                if(!clientRequest.substring(index,lastindex).equals("")){
                  //canales1[1] = clientRequest.substring(index,lastindex);
                }
              }
              if((index = clientRequest.lastIndexOf("CHANNEL3= "))>0){
                int lastindex = clientRequest.indexOf("\nCHANNEL4");
                if(!clientRequest.substring(index,lastindex).equals("")){
                  //canales1[2] = clientRequest.substring(index,lastindex);
                }
              }
              if((index = clientRequest.lastIndexOf("CHANNEL4= "))>0){
                int lastindex = clientRequest.indexOf("\nCHANNEL5");
                if(!clientRequest.substring(index,lastindex).equals("")){
                  //canales1[3] = clientRequest.substring(index,lastindex);
                }
              }
              if((index = clientRequest.lastIndexOf("CHANNEL5= "))>0){
                int lastindex = clientRequest.indexOf("\nCHANNEL6");
                if(!clientRequest.substring(index,lastindex).equals("")){
                  //canales1[4] = clientRequest.substring(index,lastindex);
                }
              }
              if((index = clientRequest.lastIndexOf("CHANNEL6= "))>0){
                int lastindex = clientRequest.indexOf("\nCHANNEL7");
                if(!clientRequest.substring(index,lastindex).equals("")){
                  //canales1[5] = clientRequest.substring(index,lastindex);
                }
              }
              if((index = clientRequest.lastIndexOf("CHANNEL7= "))>0){
                int lastindex = clientRequest.indexOf("\nCHANNEL8");
                if(!clientRequest.substring(index,lastindex).equals("")){
                  //canales1[6] = clientRequest.substring(index,lastindex);
                }
              }
              if((index = clientRequest.lastIndexOf("CHANNEL8= "))>0){
                int lastindex = clientRequest.indexOf("\n[MONITOR2]");
                if(!clientRequest.substring(index,lastindex).equals("")){
                  //canales1[7] = clientRequest.substring(index,lastindex);
                }
              }
            }
          }
        }
      Serial.println("Todo bien.");
      client.stop();
      }
    }
  client.stop();
  //Esto generará el array de canales, que posteriormente a la hora de grabarse en la SD, será cuando se decidirá cuales serán escritos y cuales no.
  //Se decidirá cuales se escriben usando las variables boolean iniciadas globalmente para ello.
  oldtime = millis();
  epoch = getTimeEpoch();
  Serial.println("Todo bien.");
}


void webServer(){
  delay(2);

  boolean html = false, json = false, json2 = false, json3 = false;
  EthernetClient client = server.available();
  if (client) {
    boolean currentLineIsBlank = true;
    String clientRequest = "";
    while (client.connected()) {
        char d;
        while(client.available()){
          d = client.read();
          clientRequest += d;
          if(clientRequest.indexOf("/ HTTP/1.1")>0 || html){
            html = true;
            clientRequest = "";
          }if(clientRequest.indexOf("/datos.json") > 0 || json){
            json = true;
            clientRequest = "";
          }/*if(clientRequest.indexOf("/datos2.json") > 0 || html){
            json2 = true;
            clientRequest = "";
          }if(clientRequest.indexOf("/datos3.json") > 0 || html){
            json3 = true;
            clientRequest = "";
          }*/
        }
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply

        if (d == '\n' && currentLineIsBlank && html) {
          Serial.println("Mande el html");
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");  // the connection will be closed after completion of the response
          client.println("Content-Type: text/html");  // refresh the page automatically every 5 sec
          client.println("Connection:close");
          client.println();
          //Enviar pagina WebServer
          File webFile = SD.open("lineas/index.htm");
          if(webFile){
            while(webFile.available()){
              client.write(webFile.read());
            }
          webFile.close();
        }
          html = false;
          Serial.println("Termine de mandarlo");
          break;
        }
        if(json && currentLineIsBlank && d== '\n'){

          Serial.println("Mande el json");
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: application/json");
          client.println("Connection: close");
          client.println();

          client.println("{\"data\":[");
          File webFile = SD.open("lake.jso",FILE_READ);
          if(webFile){
            while(webFile.available()){
              client.write(webFile.read());
            }
            client.println("]}");
            client.println();

            webFile.close();
          }
          Serial.println("Termine de mandarlo");
          json = false;
          break;
        }
        if(json2 && currentLineIsBlank && d== '\n'){
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: application/json");
          client.println("Connection: close");
          client.println();
          File webFile = SD.open("lake2.jso",FILE_READ);
          if(webFile){
            while(webFile.available()){
              client.write(webFile.read());
            }
            client.println("]}");
            client.println();

            webFile.close();
          }
          json2 = false;
          break;
        }
        if(json3 && currentLineIsBlank && d== '\n'){
          Serial.println("Mande el json");
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: application/json");
          client.println("Connection: close");
          client.println();
          File webFile = SD.open("lake3.jso",FILE_READ);
          if(webFile){
            while(webFile.available()){
              client.write(webFile.read());
            }
            client.println("]}");
            client.println();

            webFile.close();
          }
          json3 = false;
          break;
        }
        if (d == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } else if (d != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      // give the web browser secs to receive the data
    }
    delay(1);
    Serial.println("Se cerro");
    // close the connection:
    client.stop();
  }
}

void loop() {
  Ethernet.maintain();

  if(millis()-oldtime > 5000 ){
    epoch += 5;
    Serial.println("A medir");
    mediciones(1);

  }
  if(secs%10 == 0 && secs != 0){
    SD.end();
    Serial1.end();
    Serial2.end();
    Serial3.end();
    Serial.println("Cerro la conexion");
    Serial1.begin(9600,SERIAL_7O1);
    Serial2.begin(9600,SERIAL_7O1);
    Serial3.begin(9600,SERIAL_7O1);
    SD.begin(4);
    secs++;
    stop = false;
  }
    webServer();
}
void mediciones(int puerto){
  //Seleccionara cada puerto
  oldtime = millis();
  temperaturas = "";
  switch(puerto){
    case 1:
    {
      Serial1.write("KRDG?\r\n");
      Serial1.println();
      //Con este bucle generaremos el numero de canales necesarios y podremos indicar después que canales queremos mostrar
      //Así aunque recojamos todos los canales se podrán elegir el muestre y que nombre quieren, usando condicionales de NULL, en las
      //variables creadas e inicializadas en el setup

      MatchState ms;
      char patron[7];
      int desplazamiento = 0, aux = 0;
      while(Serial1.available()){
        temperaturas += (char) Serial1.read();
      }
      delayMicroseconds(20);
      for (size_t i = 0; i < 8; i++) {
        for (size_t j = 0; j < 7; j++) {
          patron[j] = temperaturas[j+desplazamiento];
        }
        desplazamiento += 8;
        ms.Target(patron);
        char result = ms.Match("^[+-][0-9]*[%.][0-9]*", 0);
        if (result == REGEXP_MATCHED){
          aux++;
        }
      }
      if(aux == 8){
        stop = true;
      }else{
        stop = false;
      }


      if(SD.exists("lake.jso") && puede && stop){
       File paraComa = SD.open("lake.jso", FILE_WRITE);
       paraComa.println(",");
       paraComa.close();
      }
      File lakeshoreData = SD.open("lake.jso",FILE_WRITE);

      if(lakeshoreData){
        Serial.println("SD");
        if(stop){
          Serial.println("Hola");
          //sendNTPpacket(); // send an NTP packet to a time server
          //delay(1000);
          // wait to see if a reply is available
          //if(Udp.parsePacket()){
            lakeshoreData.print("{");
            lakeshoreData.print("\"secs\":\"");
            /*Udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer

            Serial.println(freeMemory());
            // the timestamp starts at byte 40 of the received packet and is four bytes,
            // or two words, long. First, extract the two words:
            Serial.write("Bien");
            unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
            unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
            // combine the four bytes (two words) into a long integer
            // this is NTP time (seconds since Jan 1 1900):
            unsigned long secsSince1900 = highWord << 16 | lowWord;
            // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
            const unsigned long seventyYears = 2208988800UL;
            // subtract seventy years:
            unsigned long epoch = secsSince1900 - seventyYears;
            */
            /*lakeshoreData.print(((epoch+3600) % 2629743UL) / 86400L);
            Serial.print(((epoch+3600) % 2629743UL) / 86400L);
            lakeshoreData.print("-");
            Serial.print("-");
            lakeshoreData.print(((epoch+3600) % 31556926UL) / 2629743UL);
            Serial.print(((epoch+3600) % 31556926UL) / 2629743UL);
            lakeshoreData.print("-");
            Serial.print("-");
            lakeshoreData.print((epoch+3600) % 31556926UL);
            Serial.print((epoch+3600) % 31556926UL);
            lakeshoreData.print("\t");
            Serial.print("\t");*/
            lakeshoreData.print(((epoch+3600)  % 86400L) / 3600); // print the hour (86400 equals secs per day)
            lakeshoreData.print(':');
            Serial.print(((epoch+3600)  % 86400L) / 3600); // print the hour (86400 equals secs per day)
            Serial.print(':');
            if (((epoch % 3600) / 60) < 10) {
              // In the first 10 minutes of each hour, we'll want a leading '0'
              lakeshoreData.print('0');
              Serial.print('0');
            }
            lakeshoreData.print((epoch  % 3600) / 60); // print the minute (3600 equals secs per minute)
            lakeshoreData.print(':');
            Serial.print((epoch  % 3600) / 60); // print the minute (3600 equals secs per minute)
            Serial.print(':');
            if ((epoch % 60) < 10) {
              // In the first 10 seconds of each minute, we'll want a leading '0'
              lakeshoreData.print('0');
              Serial.print('0');
            }
            lakeshoreData.print(epoch % 60);
            Serial.print(epoch % 60);// print the second
            //Añadir fecha a la hora mostrada.
            lakeshoreData.print("\",");
          /*}else{
            Serial.println("No funciono");
            stop = false;
          }*/
        }

        desplazamiento = 0;
        secs += 1;
        for (int i = 0; i < 8; i++){
          if(stop && !canales1[i].equals("")){
            lakeshoreData.print("\"");
            lakeshoreData.print(canales1[i]);
            lakeshoreData.print("\":\"");
            for (int j = 0; j < 7; j++) {
              lakeshoreData.print(temperaturas[j+desplazamiento]);
            }if (i < CANALESSIZE-1){
              if(canales1[i+1].equals("")){
                lakeshoreData.print("\"");
              }else{
                lakeshoreData.print("\",");
              }
            }else{
              lakeshoreData.print("\"");
            }
            desplazamiento += 8;
          }

        }
        if(stop){
          lakeshoreData.print("}");
          puede = true;
        }
      }
      lakeshoreData.close();
      break;
    }

    default:
      Serial.write("Uys algo fui mal.");
  }

}

unsigned long getTimeEpoch(){
  sendNTPpacket(); // send an NTP packet to a time server
  delay(1000);
  // wait to see if a reply is available
  if(Udp.parsePacket()){
    Udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer

    Serial.println(freeMemory());
    // the timestamp starts at byte 40 of the received packet and is four bytes,
    // or two words, long. First, extract the two words:
    Serial.write("Bien");
    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
    // combine the four bytes (two words) into a long integer
    // this is NTP time (seconds since Jan 1 1900):
    unsigned long secsSince1900 = highWord << 16 | lowWord;
    const unsigned long seventyYears = 2208988800UL;
    // subtract seventy years:
    unsigned long secsNow = secsSince1900 - seventyYears;
    return secsNow;
  }
}
