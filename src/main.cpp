#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>
#include <SD.h>
#include <TimerOne.h>
#include <MemoryFree.h>


// telnet defaults to port 23
EthernetServer server(80);
int secs;
boolean puede = false, puede2= false, puede3= false;

void mediciones(int canal){
  switch(canal){
    case 1:
    {
      Serial1.write("KRDG?\r\n");
      Serial1.println();

        if (!SD.exists("lake.jso")) {
          File paraComa = SD.open("lake.jso",FILE_WRITE);
          paraComa.println("{\"data\":[");
          paraComa.close();
        }
        if(SD.exists("lake.jso") && puede){
         File paraComa = SD.open("lake.jso", FILE_WRITE);
         paraComa.println(",");
         paraComa.close();
        }
        if(Serial1.available()){
          puede = true;
        }
        File lakeshoreData = SD.open("lake.jso",FILE_WRITE);
        int j = 1, z = 0;
        while(Serial1.available()){

          delay(2);

          lakeshoreData.print("{");
          lakeshoreData.print("\"time\":\"");
          lakeshoreData.print(secs);
          lakeshoreData.print("\",");
          secs += 5;

          while(z < 8){
            lakeshoreData.print("\"Temp");
            lakeshoreData.print(j);
            lakeshoreData.print("\":\"");
            for (int i = 0; i < 7; i++) {
              char lakeshore;
              if ((lakeshore = Serial1.read())==',') {
                lakeshore = Serial1.read();
              }
              lakeshoreData.write(lakeshore);
            }
            if (z < 7){
              lakeshoreData.print("\",");
            }else{
              lakeshoreData.print("\"");
            }
            ++j;
            ++z;
          }
          lakeshoreData.print("}");

        }
        lakeshoreData.close();

      break;
    }
    case 2:
    {
      Serial2.write("KRDG?\r\n");
      Serial2.println();


        if (!SD.exists("lake.jso")) {
          File paraComa = SD.open("lake.jso",FILE_WRITE);
          paraComa.println("{\"data\":[");
          paraComa.close();
        }
        if(SD.exists("lake.jso") && puede2){
         File paraComa = SD.open("lake.jso", FILE_WRITE);
         paraComa.println(",");
         paraComa.close();
        }
        if(Serial2.available()){
          puede2 = true;
        }else {
          puede2 = false;
        }
        File lakeshoreData = SD.open("lake.jso",FILE_WRITE);
        int j = 1, z = 0;
        while(Serial2.available()){

          delay(2);

          lakeshoreData.print("{");
          lakeshoreData.print("\"secs\":\"");
          lakeshoreData.print(secs);
          lakeshoreData.print("\",");
          secs += 5;

          while(z < 8){
            lakeshoreData.print("\"Temp");
            lakeshoreData.print(j);
            lakeshoreData.print("\":\"");
            for (int i = 0; i < 7; i++) {
              char lakeshore;
              if ((lakeshore = Serial2.read())==',') {
                lakeshore = Serial2.read();
              }
              lakeshoreData.write(lakeshore);
            }
            /*char lakeshore;
            String temp = "";
            while((lakeshore = Serial2.read())!= ','){
              temp += lakeshore;
            }*/
            if (z < 7){
              lakeshoreData.print("\",");
            }else{
              lakeshoreData.print("\"");
            }
            ++j;
            ++z;
          }
          lakeshoreData.print("}");

        }
        lakeshoreData.close();

      break;
    }
    case 3:
    {
      Serial3.write("KRDG?\r\n");
      Serial3.println();

        if (!SD.exists("lake.jso")) {
          File paraComa = SD.open("lake.jso",FILE_WRITE);
          paraComa.println("{\"data\":[");
          paraComa.close();
        }
        if(SD.exists("lake.jso") && puede3){
         File paraComa = SD.open("lake.jso", FILE_WRITE);
         paraComa.println(",");
         paraComa.close();
        }
        if(Serial3.available()){
          puede3 = true;
        }
        File lakeshoreData = SD.open("lake.jso",FILE_WRITE);
        int j = 1, z = 0;
        while(Serial3.available()){

          delay(2);

          lakeshoreData.print("{");
          lakeshoreData.print("\"time\":\"");
          lakeshoreData.print(secs);
          lakeshoreData.print("\",");
          secs += 5;

          while(z < 8){
            lakeshoreData.print("\"Temp");
            lakeshoreData.print(j);
            lakeshoreData.print("\":\"");
            for (int i = 0; i < 7; i++) {
              char lakeshore;
              if ((lakeshore = Serial3.read())==',') {
                lakeshore = Serial3.read();
              }
              lakeshoreData.write(lakeshore);
            }
            if (z < 7){
              lakeshoreData.print("\",");
            }else{
              lakeshoreData.print("\"");
            }
            ++j;
            ++z;
          }
          lakeshoreData.print("}");

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
  mediciones(2);

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
   if (!SD.exists("lineas/index.htm")) {
       return;  // can't find index file
   }

   if(SD.exists("lake.jso")){
     SD.remove("lake.jso");
   }
   secs = 0;

   Timer1.initialize(2000000);
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
    // initialize the Ethernet device not using DHCP:
   /* Ethernet.begin(mac, ip, myDns, gateway, subnet);*/
  }
  Serial.print(Ethernet.localIP());
  // start listening for clients
  server.begin();
}


void webServer(){
  delay(2);
  // wait for a new client:

  noInterrupts();
  for(int i= 0; i < 2; i++){
    EthernetClient client = server.available();
    if (client) {
      // an http request ends with a blank line
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
          Serial.print("FreeMemory= ");
          Serial.println(freeMemory());
          if (d == '\n' && currentLineIsBlank && clientRequest.indexOf("/ HTTP/1.1")>0) {
            Serial.println("Mande el HTML");
            // send a standard http response header
            client.println("HTTP/1.1 200 OK\nContent-Type: text/html\nConnection:close");  // the connection will be closed after completion of the response
            //client.println("Refresh: 10");  // refresh the page automatically every 5 sec
            client.println();
            //Enviar pagina WebServer
            File webFile = SD.open("lineas/index.htm");
            if(webFile){
              while(webFile.available()){
                client.write(webFile.read());
              }
            webFile.close();
            }
            break;
          }
          if(clientRequest.indexOf("/lake.json") > 0 && currentLineIsBlank && d== '\n'){
            Serial.println("Mande el json");
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: application/json");
            client.println("Connection: close");
            client.println();
            File webFile = SD.open("lake.jso",FILE_READ);
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
  Serial2.end();
  Serial.println("Cerro la conexion");
  Serial2.begin(9600,SERIAL_7O1);
  secs+=5;
}

webServer();



}
