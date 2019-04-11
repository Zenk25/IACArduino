#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>
#include <SD.h>
#include <CustomSoftwareSerial.h>



// telnet defaults to port 23
EthernetServer server(80);
CustomSoftwareSerial* lakeshore;

void setup() {
  byte mac[] = {
    0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02
  };
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  lakeshore = new CustomSoftwareSerial(2,3);
  lakeshore->begin(9600,711);
  //Iniciada la SDCard
   if (!SD.begin(4)) {
       return;    // init failed
   }
   // check for index.htm file
   if (!SD.exists("lineas/index.htm")) {
       return;  // can't find index file
   }
   if (SD.exists("datosF.txt")){
     SD.remove("datosF.txt");
   }

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
  lakeshore->write("KRDG?\r\n");
  delay(1000);
  if(SD.exists("datos.jso")){
    SD.remove("datos.jso");
  }
  File datos = SD.open("datos.jso",FILE_WRITE);
  File datosT = SD.open("datosF.txt", FILE_WRITE);
  int j = 1;
  datos.print("{");
  while(lakeshore->available()){
    delay(2);
    datos.print("\"C");
    datos.print(j);
    datos.print("\":");
    for (int i = 0; i < 7; i++) {
      char c;
      if ((c= lakeshore->read())==',') {
        c= lakeshore->read();
      }
      datos.write(c);
      datosT.write(c);
    }
    datos.print(",");
    datosT.print(",");
    ++j;
    datos.println();

  }
  datos.print("}");
  datos.close();
  datosT.close();

  delay(5000);
  // wait for a new client:
  EthernetClient client = server.available();
  if (client) {
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);

        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK\nContent-Type: text/html\nConnection: close");  // the connection will be closed after completion of the response
          //client.println("Refresh: 5");  // refresh the page automatically every 5 sec
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

        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    Serial.println("se cierra");
    client.stop();
  }
}
