#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>
#include <SD.h>
#include <TimerOne.h>


// telnet defaults to port 23
EthernetServer server(80);
int secs;

void interrupcion5secs(){
  Serial.println("Han pasado 5 secs");
}

void setup() {
  byte mac[] = {
    0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02
  };
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Timer1.initialize(5000000);
  Timer1.attachInterrupt(interrupcion5secs);

  Serial2.begin(9600,SERIAL_7O1);
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




void loop() {

  Serial2.write("KRDG?\r\n");
  delay(1000);
  if (!SD.exists("lake.jso")) {
    File paraComa = SD.open("lake.jso",FILE_WRITE);
    paraComa.println("{\"data\":[");
    paraComa.close();
  }else{
    File paraComa = SD.open("lake.jso", FILE_WRITE);
    paraComa.println(",");
    paraComa.close();
  }

  File lakeshoreData = SD.open("lake.jso",FILE_WRITE);
  int j = 1, z = 0;
  while(Serial2.available()){
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
        if ((lakeshore = Serial2.read())==',') {
          lakeshore = Serial2.read();
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
  delay(2);
  // wait for a new client:


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
            Serial.write(d);
            clientRequest += d;
          }
          // if you've gotten to the end of the line (received a newline
          // character) and the line is blank, the http request has ended,
          // so you can send a reply

          if (d == '\n' && currentLineIsBlank && clientRequest.indexOf("/ HTTP/1.1")>0) {
            Serial.println("Mande el HTML");
            // send a standard http response header
            client.println("HTTP/1.1 200 OK\nContent-Type: text/html\nConnection:close");  // the connection will be closed after completion of the response
            //client.println("Refresh: 15");  // refresh the page automatically every 5 sec
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
          if(clientRequest.indexOf("/datos.json") > 0 && currentLineIsBlank && d== '\n'){
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

}
