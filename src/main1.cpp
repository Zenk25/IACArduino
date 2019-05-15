#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>
#include <SD.h>
#include <TimerOne.h>
#include <MemoryFree.h>
#include <Regexp.h>


// telnet defaults to port 23
EthernetServer server(80);
boolean puede = false, puede2= false, puede3= false, stop = false, stop2 = false, stop3 = false;

unsigned int localPort = 8888;
const int NTP_PACKET_SIZE = 48;
char timeServer[] = "1.europe.pool.ntp.org";
byte packetBuffer[NTP_PACKET_SIZE];

EthernetUDP Udp;


void sendNTPpacket(const char* address){
    // set all bytes in the buffer to 0
    memset(packetBuffer, 0, NTP_PACKET_SIZE);
    // Initialize values needed to form NTP request
    // (see URL above for details on the packets)
    packetBuffer[0] = 0b11100011;   // LI, Version, Mode
    packetBuffer[1] = 0;     // Stratum, or type of clock
    packetBuffer[2] = 6;     // Polling Interval
    packetBuffer[3] = 0xEC;  // Peer Clock Precision
    // 8 bytes of zero for Root Delay & Root Dispersion
    packetBuffer[12]  = 49;
    packetBuffer[13]  = 0x4E;
    packetBuffer[14]  = 49;
    packetBuffer[15]  = 52;

    // all NTP fields have been given values, now
    // you can send a packet requesting a timestamp:
    Udp.beginPacket(address, 123); // NTP requests are to port 123
    Udp.write(packetBuffer, NTP_PACKET_SIZE);
    Udp.endPacket();
}

void mediciones(int canal){
  switch(canal){
    case 1:
    {
      int aux = 0;
      Serial1.write("KRDG?\r\n");
      Serial1.println();
      delay(2);
      char temp [8][7];
      char lakeshore;
      int j = 1, z = 0;
      while(Serial1.available()){
        MatchState ms;
        for (int i = 0; i < 8; i++) {
          z=0;
          while(z < 7) {
            if ((lakeshore = Serial1.read())==',') {
              lakeshore = Serial1.read();
            }
            temp[i][z] = lakeshore;
            z++;
          }
          ms.Target(temp[i]);

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
      }

      if(SD.exists("lake.jso") && puede && stop){
       File paraComa = SD.open("lake.jso", FILE_WRITE);
       paraComa.println(",");
       paraComa.close();
      }


      File lakeshoreData = SD.open("lake.jso",FILE_WRITE);

        if(stop){
          lakeshoreData.print("{");
          lakeshoreData.print("\"secs\":\"");
          sendNTPpacket(timeServer); // send an NTP packet to a time server

          // wait to see if a reply is available
          while(!Udp.parsePacket()){
            delay(1);
          }
          Udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer

          // the timestamp starts at byte 40 of the received packet and is four bytes,
          // or two words, long. First, extract the two words:

          unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
          unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
          // combine the four bytes (two words) into a long integer
          // this is NTP time (seconds since Jan 1 1900):
          unsigned long secsSince1900 = highWord << 16 | lowWord;
          // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
          const unsigned long seventyYears = 2208988800UL;
          // subtract seventy years:
          unsigned long epoch = secsSince1900 - seventyYears;
          lakeshoreData.print(((epoch+3600)  % 86400L) / 3600); // print the hour (86400 equals secs per day)
          lakeshoreData.print(':');
          if (((epoch % 3600) / 60) < 10) {
            // In the first 10 minutes of each hour, we'll want a leading '0'
            lakeshoreData.print('0');
          }
          lakeshoreData.print((epoch  % 3600) / 60); // print the minute (3600 equals secs per minute)
          lakeshoreData.print(':');
          if ((epoch % 60) < 10) {
            // In the first 10 seconds of each minute, we'll want a leading '0'
            lakeshoreData.print('0');
          }
          lakeshoreData.print(epoch % 60); // print the second
          //Añadir fecha a la hora mostrada.
          Ethernet.maintain();
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

void interrupcion(){
  mediciones(1);

}




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

   Timer1.initialize(30000000);
   Timer1.attachInterrupt(interrupcion);
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
}


void webServer(){
  delay(2);

  noInterrupts();
  for(int i= 0; i < 2; i++){
    EthernetClient client = server.available();
    if (client) {
      boolean currentLineIsBlank = true;
      String clientRequest = "";
      while (client.connected()) {
          char d;
          while(client.available()){
            d = client.read();
            clientRequest += d;
          }
          // if you've gotten to the end of the line (received a newline
          // character) and the line is blank, the http request has ended,
          // so you can send a reply
          //Serial.print("FreeMemory= ");
          //Serial.println(freeMemory());
          if (d == '\n' && currentLineIsBlank && clientRequest.indexOf("/ HTTP/1.1")>0) {
            noInterrupts();
            // send a standard http response header
            client.println("HTTP/1.1 200 OK");  // the connection will be closed after completion of the response
            client.println("Content-Type: text/html");  // refresh the page automatically every 5 sec
            client.println("Connection:close");
            client.println();
            //Enviar pagina WebServer
            /*client.print("<!DOCTYPE html> <html> <head> <meta charset= \"utf-8\"></meta> <meta name =\"viewport\" content=\"width=device-width,initial-scale=1.0\"></meta> <link rel=\"stylesheet\" href=\"http://cdnjs.cloudflare.com/ajax/libs/morris.js/0.5.1/morris.css\"></link> <script src=\"http://ajax.googleapis.com/ajax/libs/jquery/1.9.0/jquery.min.js\"></script> <script src=\"http://cdnjs.cloudflare.com/ajax/libs/raphael/2.1.0/raphael-min.js\"></script> <script src=\"http://cdnjs.cloudflare.com/ajax/libs/morris.js/0.5.1/morris.min.js\"></script> <link rel=\"stylesheet\" href=\"https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/css/bootstrap.min.css\" integrity=\"sha384-ggOyR0iXCbMQv3Xipma34MD+dH/1fQ784/j6cY/iJTQUOhcWr7x9JvoRxT2MZw1T\" crossorigin=\"anonymous\"></link> <title>Arduino SD, Grafica de lineas</title> </head> <body> <script> var chart; $.getJSON(\"lake.json\", function (json) { var data = json.data; chart = Morris.Line({ element: 'primerGrafico', data: data, xkey: 'secs', xLabels: 'Segundos',");
            client.print(" // A list of names of data record attributes that contain y-values. //Los 8 canales irán en las y. ykeys: ['Temp1','Temp2','Temp3','Temp4','Temp5','Temp6','Temp7','Temp8'], // Labels for the ykeys -- will be displayed when you hover over the // chart. labels: ['Temp1','Temp2','Temp3','Temp4','Temp5','Temp6','Temp7','Temp8'], resize: true, pointSize: 0, hideHover : true, smooth: false, parseTime: false, xLabelAngle: 45 }); }); var chart2; $.getJSON(\"lake2.json\", function (json) { var data = json.data; chart = Morris.Line({ element: 'segundoGrafico', data: data, xkey: 'secs', xLabels: 'Segundos', // A list of names of data record attributes that contain y-values. //Los 8 canales irán en las y. ykeys: ['Temp1','Temp2','Temp3','Temp4','Temp5','Temp6','Temp7','Temp8'], // Labels for the ykeys -- will be displayed when you hover over the // chart. labels: ['Temp1','Temp2','Temp3','Temp4','Temp5','Temp6','Temp7','Temp8'],");
            client.println(" resize: true, pointSize: 0, hideHover : true, smooth: false, parseTime: false, xLabelAngle: 45 }); }); var chart3; $.getJSON(\"lake3.json\", function (json) { var data = json.data; chart = Morris.Line({ element: 'TercerGrafico', data: data, xkey: 'secs', xLabels: 'Segundos', // A list of names of data record attributes that contain y-values. //Los 8 canales irán en las y. ykeys: ['Temp1','Temp2','Temp3','Temp4','Temp5','Temp6','Temp7','Temp8'], // Labels for the ykeys -- will be displayed when you hover over the // chart. labels: ['Temp1','Temp2','Temp3','Temp4','Temp5','Temp6','Temp7','Temp8'], resize: true, pointSize: 0, hideHover : true, smooth: false, parseTime: false, xLabelAngle: 45 }); }); function update() { $.getJSON(\"lake.json\", function (json){ chart.setData(json.data); }) }; function update2() { $.getJSON(\"lake2.json\", function (json){ chart.setData(json.data); }) }; function update3() { $.getJSON(\"lake3.json\", function (json){ chart.setData(json.data); }) }; setInterval(update, 60000); setInterval(update2, 60000); setInterval(update3, 60000); </script> <div class=\"container\"> <h1>Gráficas</h1> <div class=\"row\"> <div class=\"col-md-12\"> <h2>Primer Grupo</h2> <div id=\"primerGrafico\"></div> </div> </div> <div class=\"row\"> <div class=\"col-md-12\"> <h2>Segundo Grupo</h2> <div id=\"segundoGrafico\"></div> </div> </div> <div class=\"row\"> <div class=\"col-md-12\"> <h2>Tercer Grupo</h2> <div id=\"tercerGrafico\"></div> </div> </div> </div> </body> </html>");*/
            File webFile = SD.open("lineas/index.htm");
            if(webFile){
              while(webFile.available()){
                client.write(webFile.read());
              }
            webFile.close();
          }
            interrupts();
            break;
          }
          if(clientRequest.indexOf("/lake.json") > 0 && currentLineIsBlank && d== '\n'){
            noInterrupts();
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: application/json");
            client.println("Connection: close");
            client.println();

            client.println("{\"data\":[");
            File webFile = SD.open("lake.jso",FILE_READ);
            if(webFile){
              while(webFile.available()){
                //Añadir este patron de comprobacion aqui [\d\w"}{:+.,\n]
                client.write(webFile.read());
              }
              client.println("]}");
              client.println();

              webFile.close();
            }
            interrupts();
            break;
          }
          if(clientRequest.indexOf("/lake2.json") > 0 && currentLineIsBlank && d== '\n'){
            Serial.println("Mande el json");
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
            break;
          }
          if(clientRequest.indexOf("/lake3.json") > 0 && currentLineIsBlank && d== '\n'){
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
            break;
          }
          if (d == '\n') {
            // you're starting a new line
            currentLineIsBlank = true;
          } else if (d != '\r') {
            // you've gotten a character on the current line
            currentLineIsBlank = false;
          }
      }
      // give the web browser secs to receive the data
      delay(1);
      // close the connection:

      client.stop();
    }
  }
  interrupts();
}

void loop() {
if(secs%50 == 0 && secs != 0){
  Serial1.end();
  SD.end();
  Serial.println("Cerro la conexion");
  Serial1.begin(9600,SERIAL_7O1);
  SD.begin(4);
  secs+=5;
  stop = false;
}

webServer();



}
