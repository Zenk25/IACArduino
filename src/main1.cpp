#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>
#include <SD.h>
#include <MemoryFree.h>
#include <Regexp.h>

//Canales totales, manejables por la aplicación
const int CANALESSIZE = 8;
const int NTP_PACKET_SIZE = 48;

//Variables en las que se guardarán milisegundos de tiempo por necesidad.
unsigned long oldtime = 0, extendTime = 0, cambioFichero = 0, tiempoMedicion = 0;
//Se guardará el tiempo actual en segundos
unsigned long epoch;
//Declaración variables para el Header
String subsystem = "",responsible = "", objective = "";
//Declaración variables para el Monitor 1
String port1 = "", type = "", magnitude = "";
//Variable que mandará el comando al equipo en cuestión, por ahora solo está implementado para Temperatura
char magn[7];
//Variable que guardará el formato desea a la hora de imprimir los datos en el csv
String formatNum;
String canales1[8] = {"","","","","","","",""};
String temperaturas = "", nomFichero = "lake";
// telnet defaults to port 23
EthernetServer server(80);
boolean puede = false, stop = false, dia = true, holi= true, holi2 = false;
int interacciones = 0, cicle = 0,tempPeriod = 0, pressurePeriod = 0,numFichero= 0, secsInicio = 0;
//Variables necesarias para el envio de paquetes al servidor NTP
unsigned int localPort = 8888;
char timeServer[] = "pool.ntp.org";
byte packetBuffer[NTP_PACKET_SIZE];

EthernetUDP Udp;
void printUploadForm(EthernetClient client);
void mediciones(int puerto);
unsigned long getTimeEpoch();
void sendNTPpacket();
void iniciarConfig(String clientRequest, boolean inicio);

//Esta clase se encargará de recoger las mediciones de los canales.
//poner codigo para la ip fija
void setup() {
  byte mac[] = {
    0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02
  };
  /*Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }*/
  Serial1.begin(9600,SERIAL_7O1);
  //Iniciada la SDCard
   if (!SD.begin(4)) {
       return;    // init failed
   }
   // check for index.htm file
   if (!SD.exists("lineas/index.htm")) {
       return;  // can't find index file
   }
   File dirJson = SD.open("json/");
   File file;
   while(true){
     file = dirJson.openNextFile();
     if(!file){
       //Serial.println("--Done--");
       break;
     }
     else{
       String borrar = "json/";
       borrar += file.name();
       SD.remove(borrar);
     }
   }
   file.close();
   dirJson.close();
   interacciones = 0;
  //Para que la conexion este fija descomente el siguiente codigo,  introduciendo los datos necesarios.
  /*if (Ethernet.begin(mac,ip,dns,gateway,subnet) == 0) {
    // Check for Ethernet hardware present
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      while (true) {
        delay(1); // do nothing, no point running without Ethernet hardware
      }
    }
    if (Ethernet.linkStatus() == LinkOFF) {
    }
  }*/
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
  //Serial.print(Ethernet.localIP());
  server.begin();
  Udp.begin(localPort);
  //Se recoge el tiempo desde el arranque del arduino.
  oldtime = millis();
  //Se recoge el tiempo actual.
  while(epoch == 0){
    epoch = getTimeEpoch();
  }
  if(SD.exists("configs/config.ini")){
    File configFile= SD.open("configs/config.ini");
    String config = "";
    while(configFile.available()){
      char ltr = configFile.read();
      config += ltr;
    }
    iniciarConfig(config, true);
    config = "";
  }
}

//Este método crea el webServer que contendrá las gráficas.
void webServer(){
  delay(2);

  boolean html = false, json = false ,upload = false, csv = false;
  EthernetClient client = server.available();
  if (client) {
    boolean currentLineIsBlank = true;
    String clientRequest = "";
    while (client.connected()) {
        char d;
        while(client.available()){
          d = client.read();
          clientRequest += d;
          if(clientRequest.indexOf("Connection: keep-alive")>0){
            if(clientRequest.indexOf("boundary")>0){
              if(clientRequest.indexOf("[END]")>0){
                iniciarConfig(clientRequest.substring(clientRequest.indexOf("[Header]"),clientRequest.lastIndexOf("[END]")+5),false);
                clientRequest = "";
                html = true;
              }
            }
            else if(clientRequest.indexOf(" / HTTP/1.1")>0){
              html = true;
              clientRequest = "";
            }
            else if(clientRequest.indexOf(" /upload.html HTTP/1.1")>0){
              upload= true;
              clientRequest= "";
            }

            else if(clientRequest.indexOf("/datos.json") > 0){
              json = true;
              clientRequest = "";
            }
            else if(clientRequest.indexOf("/datosFinal.csv")> 0){
              csv = true;
              clientRequest = "";
            }
        }
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (d =='\n' && currentLineIsBlank && upload){
          //Serial.println("Mande el upload");
          client.println("HTTP/1.1 200 OK");  // the connection will be closed after completion of the response
          client.println("Content-Type: text/html");  // refresh the page automatically every 5 sec
          client.println("Connection:close");
          client.println();
          printUploadForm(client);
          upload = false;
          break;
        }
        if (d == '\n' && currentLineIsBlank && html) {
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
          //Serial.println("Termine de mandarlo");
          break;
        }
        if(json && currentLineIsBlank && d== '\n'){

          //Serial.println("Mande el json");
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: application/json");
          client.println("Connection: close");
          client.println();

          client.println("{\"data\":[");
          File webFile = SD.open("json/"+nomFichero+".jso",FILE_READ);
          if(webFile){
            extendTime = millis();
            while(webFile.available()){
              client.write(webFile.read());
            }
            client.println("]}");
            client.println();
            extendTime = millis() - extendTime;
            webFile.close();
          }
          //Serial.println("Termine de mandarlo");
          json = false;
        }
        if (d == '\n' && currentLineIsBlank && csv) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");  // the connection will be closed after completion of the response
          client.println("Content-Type: application/CSV");  // refresh the page automatically every 5 sec
          client.println("Connection:close");
          client.println();
          //Enviar pagina WebServer
          File webFile = SD.open("datos/final.csv", FILE_READ);
          if(webFile){
            while(webFile.available()){
              client.write(webFile.read());
            }
          webFile.close();
        }
          csv = false;
          //Serial.println("Termine de mandarlo");
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
      // close the connection:
      client.stop();
      break;
      }
    }
}

void loop() {
  //Cambio de fichero cada 12 horas ahora mismo
  if(millis()-cambioFichero > 43200000UL){
    numFichero++;
    nomFichero = "lake";
    nomFichero+= numFichero;
    cambioFichero = millis();
    //Serial.print(nomFichero);
    puede = false;
  }

  if(millis() > 86400000UL ){
    epoch = getTimeEpoch();
  }
  Ethernet.maintain();
  unsigned long necesario = tempPeriod*1000;
  if(millis()-oldtime > necesario && tempPeriod>0){
    oldtime = 0;
    epoch += tempPeriod+(extendTime/1000);
    extendTime= 0;
    mediciones(1);

  }

  if(interacciones%10 == 0 && interacciones != 0){
    SD.end();
    Serial1.end();
    //Serial.println("Cerro la conexion");
    Serial1.begin(9600,SERIAL_7O1);
    SD.begin(4);
    interacciones++;
    stop = false;
  }
    webServer();
}
void mediciones(int puerto){
  //Como por ahora solo estará disponible un puerto solo existirá una caso en el switch, pero a futuro se podrá extender.
  //Se recoge el tiempo al empezar la medición;
  oldtime = millis();
  temperaturas = "";
  switch(puerto){
    case 1:
    {
      Serial1.write("KRDG?\r\n");
      Serial1.println();
      //Con este bucle generaremos el numero de canales necesarios y podremos indicar después que canales queremos mostrar
      //Así aunque recojamos todos los canales se podrán elegir el muestre y que nombre quieren.
      MatchState ms;
      char patron[7];
      int desplazamiento = 0, aux = 0;
      //Serial.print(freeMemory());
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
      if(SD.exists("json/"+nomFichero+".jso") && puede && stop){
       File paraComa = SD.open("json/"+nomFichero+".jso",FILE_WRITE);
       paraComa.println(",");
       paraComa.close();
      }
      File lakeshoreData = SD.open("json/"+nomFichero+".jso",FILE_WRITE);
      File modeloFinal = SD.open("datos/final.csv",FILE_WRITE);

      if(holi){
        tiempoMedicion = millis();
        holi = false;
      }

      if(lakeshoreData){
        //Serial.println("SD");
        if(stop){
          //Serial.println("Hola");
          if(holi2){
            modeloFinal.println();
          }
          holi2=true;
          lakeshoreData.print("{");
          lakeshoreData.print("\"secs\":\"");
          /*lakeshoreData.print(((epoch+3600) % 2629743UL) / 86400L);
          //Serial.print(((epoch+3600) % 2629743UL) / 86400L);
          lakeshoreData.print("-");
          //Serial.print("-");
          lakeshoreData.print(((epoch+3600) % 31556926UL) / 2629743UL);
          //Serial.print(((epoch+3600) % 31556926UL) / 2629743UL);
          lakeshoreData.print("-");
          //Serial.print("-");
          lakeshoreData.print((epoch+3600) % 31556926UL);
          //Serial.print((epoch+3600) % 31556926UL);
          lakeshoreData.print("\t");
          //Serial.print("\t");*/
          lakeshoreData.print(((epoch+3600)  % 86400L) / 3600); // print the hour (86400 equals secs per day)
          lakeshoreData.print(':');
          if((millis()-tiempoMedicion)<1000){
            modeloFinal.print(0);
          }else{
            modeloFinal.print(((millis()-tiempoMedicion)/1000)+secsInicio);
          }
          modeloFinal.print("; ");
          modeloFinal.print(((epoch+3600)  % 86400L) / 3600); // print the hour (86400 equals secs per day)
          modeloFinal.print(':');

          //Serial.print(((epoch+3600)  % 86400L) / 3600); // print the hour (86400 equals secs per day)
          //Serial.print(':');
          if (((epoch % 3600) / 60) < 10) {
            // In the first 10 minutes of each hour, we'll want a leading '0'
            lakeshoreData.print('0');
            modeloFinal.print('0');
            //Serial.print('0');
          }
          lakeshoreData.print((epoch  % 3600) / 60); // print the minute (3600 equals secs per minute)
          lakeshoreData.print(':');
          modeloFinal.print((epoch  % 3600) / 60); // print the minute (3600 equals secs per minute)
          modeloFinal.print(':');
          //Serial.print((epoch  % 3600) / 60); // print the minute (3600 equals secs per minute)
          //Serial.print(':');
          if ((epoch % 60) < 10) {
            // In the first 10 seconds of each minute, we'll want a leading '0'
            lakeshoreData.print('0');
            //Serial.print('0');
            modeloFinal.print('0');

          }
          lakeshoreData.print(epoch % 60);
          //Serial.print(epoch % 60);// print the second
          //Añadir fecha a la hora mostrada.
          lakeshoreData.print("\",");
          modeloFinal.print(epoch % 60);
          modeloFinal.print(";");
        }
        //Este bucle se encargará de crear
        desplazamiento = 0;
        interacciones += 1;
        for (int i = 0; i < 8; i++){
          if(stop && !canales1[i].equals("")){
            lakeshoreData.print("\"");
            lakeshoreData.print(canales1[i]);
            lakeshoreData.print("\":\"");
            String aux = "";
            for (int j = 0; j < 7; j++) {
              lakeshoreData.print(temperaturas[j+desplazamiento]);
              aux += temperaturas[j+desplazamiento];
            }
            if(formatNum.equals(",")){
              aux.replace(".",",");
            }
            modeloFinal.print(aux);
            if (i < CANALESSIZE-1){
              if(canales1[i+1].equals("")){
                lakeshoreData.print("\"");
              }else{
                lakeshoreData.print("\",");
                modeloFinal.print("; ");
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
      modeloFinal.close();
      break;
    }

    default:
    ;
  }

}
//Metodo que recibe mediante NTP la fecha actual en segundos desde el 1 de Enero de 1970
unsigned long getTimeEpoch(){
  sendNTPpacket(); // send an NTP packet to a time server
  delay(1000);
  // wait to see if a reply is available
  if(Udp.parsePacket()){
    Udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer

    //Serial.println(freeMemory());
    // the timestamp starts at byte 40 of the received packet and is four bytes,
    // or two words, long. First, extract the two words:
    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
    // combine the four bytes (two words) into a long integer
    // this is NTP time (seconds since Jan 1 1900):
    unsigned long secsSince1900 = highWord << 16 | lowWord;
    const unsigned long seventyYears = 2208988800UL;
    // subtract seventy years:
    unsigned long secsNow = secsSince1900 - seventyYears;
    return secsNow;
  }else{
    return 0;
  }
}
void printUploadForm(EthernetClient client) {
  client.println("<!DOCTYPE html>");
  client.println("<html lang=\"es\">");
  client.println("<head>");
  client.println("<meta charset= \"utf-8\"></meta>");
  client.println("<link rel=\"shortcut icon\" href=\"http://www.example.com/my_empty_resource\" />");
  client.println("<title>Upload Config</title>");
  client.println("</head>");
  client.println("<body>");
  client.println();
  client.println("<form action=\"/\" method=\"post\" enctype=\"multipart/form-data\">");
  client.println("Select image to upload:");
  client.println("<input type=\"file\" name=\"fileToUpload\" id=\"fileToUpload\">");
  client.println("<input type=\"submit\" value=\"Upload Config\" name=\"submit\">");
  client.println("</form>");
  client.println();
  client.println("</body>");
  client.println("</html>");

  client.println();

}
//Esta clase será necesaria para obtener la fecha desde un servidor NTP
void sendNTPpacket(){
  //Serial.println("Enviando...");
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
    // todos los campos del NTP tienen valores, asíque ahora
    // puedes enviar un mensaje pidiendo la fecha y hora
    if((Udp.beginPacket(timeServer, 123))!=1){
      delay(1000);
      Udp.beginPacket(timeServer,123);
      //Si falla que no escriba la coma.
    }
    //los pedidos NTP se hacen a través del puerto 123
    Udp.write(packetBuffer, NTP_PACKET_SIZE);
    Udp.endPacket();

}

void iniciarConfig(String clientRequest, boolean inicio){
  if(!inicio){
    File dirConfig = SD.open("configs/");
    File dirDatos = SD.open("datos/");
    File prueba;
    while(true){
      prueba = dirDatos.openNextFile();
      if(!prueba){
        //Serial.println("--Done--");
        break;
      }
      else{
        String borrar = "datos/";
        borrar += prueba.name();
        SD.remove(borrar);
      }
    }
    while(true){
      prueba = dirConfig.openNextFile();
      if(!prueba){
        //Serial.println("--Done--");
        break;
      }
      else{
        String borrar = "configs/";
        borrar += prueba.name();
        SD.remove(borrar);
      }
    }
    File dirJson = SD.open("json/");
    File file;
    while(true){
      file = dirJson.openNextFile();
      if(!file){
        //Serial.println("--Done--");
        break;
      }
      else{
        String borrar = "json/";
        borrar += file.name();
        SD.remove(borrar);
      }
    }
    file.close();
    dirJson.close();
    prueba.close();
    prueba = SD.open("configs/config.ini",FILE_WRITE);
    prueba.print(clientRequest);
    dirConfig.close();
    dirDatos.close();
    prueba.close();
  }
  int index;
  if ((index = clientRequest.lastIndexOf("Subsystem= "))>0) {
    int lastindex = clientRequest.indexOf("\nCicle");
    subsystem = clientRequest.substring(index+11, lastindex-1);
  }
  if((index = clientRequest.lastIndexOf("Cicle= "))>0){
    int lastindex = clientRequest.indexOf("\nResponsible");
    cicle = clientRequest.substring((index+7),lastindex-1).toInt();
  }
  if((index = clientRequest.lastIndexOf("Responsible= "))>0){
    int lastindex = clientRequest.indexOf("\nObjective");
    responsible = clientRequest.substring(index+13,lastindex-1);
  }
  if((index = clientRequest.lastIndexOf("Objective= "))>0){
    int lastindex = clientRequest.indexOf("\nFormatNum");
    objective = clientRequest.substring(index+11,lastindex-1);
  }
  if((index = clientRequest.lastIndexOf("FormatNum= "))>0){
    int lastindex = clientRequest.indexOf("\nTemperaturePeriod");
    formatNum = clientRequest.substring(index+11,lastindex-1);
  }
  if((index = clientRequest.lastIndexOf("TemperaturePeriod= "))>0){
    int lastindex = clientRequest.indexOf("\nPressurePeriod");
    tempPeriod = clientRequest.substring(index+19,lastindex-1).toInt();
  }

  if ((index = clientRequest.lastIndexOf("PressurePeriod= "))>0){
    int lastindex = clientRequest.indexOf("\n[MONITOR1]");
    pressurePeriod = clientRequest.substring(index+16,lastindex-1).toInt();
  }

  //Cuando se alcance el index del monitor, se empezarán a recoger los datos correspondientes al monitor.
    index=0;
    //Empezando por el puerto de entrada.
    if((index = clientRequest.lastIndexOf("PORT= "))>0){
      int lastindex = clientRequest.indexOf("\nMagnitude");
      port1 = clientRequest.substring(index+6,lastindex-1);
    }
    //Seguido de la magnitud que se desea recoger.
    if((index = clientRequest.lastIndexOf("Magnitude= "))>0){
      int lastindex = clientRequest.indexOf("\nType");
      magnitude = clientRequest.substring(index+11,lastindex-1);
      if(magnitude.equals("temperature")){
        magn[0] = 'K';
        magn[1] = 'R';
        magn[2] = 'D';
        magn[3] = 'G';
        magn[4] = '?';
        magn[5] = '\r';
        magn[6] = '\n';
      }else if(magnitude.equals("pressure")){
        //Aqui se añadirá el codigo para medir la presión.
        //magn =
      }
    }
    //El tipo de aparato de medición, por defecto Lakeshore218
    if((index = clientRequest.lastIndexOf("Type= "))>0){
      int lastindex = clientRequest.indexOf("\nCHANNEL1");
      type = clientRequest.substring(index+6,lastindex-1);
    }
    //Y por último los nombres de los canales, a los que no se les haya puesto un nombre no saldrán en la gráfica.
    //Los nombres tienen que tener 9 caracteres de máx. y no pueden ser iguales.
    if((index = clientRequest.lastIndexOf("CHANNEL1= "))>0){
      int lastindex = clientRequest.indexOf("\nCHANNEL2");
      if(!clientRequest.substring(index+10,lastindex-1).equals("")){
        canales1[0] = clientRequest.substring(index+10,lastindex-1);
      }
    }
    if((index = clientRequest.lastIndexOf("CHANNEL2= "))>0){
      int lastindex = clientRequest.indexOf("\nCHANNEL3");
      if(!clientRequest.substring(index+10,lastindex).equals("")){
        canales1[1] = clientRequest.substring(index+10,lastindex-1);
      }
    }
    if((index = clientRequest.lastIndexOf("CHANNEL3= "))>0){
      int lastindex = clientRequest.indexOf("\nCHANNEL4");
      if(!clientRequest.substring(index+10,lastindex).equals("")){
        canales1[2] = clientRequest.substring(index+10,lastindex-1);
      }
    }
    if((index = clientRequest.lastIndexOf("CHANNEL4= "))>0){
      int lastindex = clientRequest.indexOf("\nCHANNEL5");
      if(!clientRequest.substring(index+10,lastindex).equals("")){
        canales1[3] = clientRequest.substring(index+10,lastindex-1);
      }
    }
    if((index = clientRequest.lastIndexOf("CHANNEL5= "))>0){
      int lastindex = clientRequest.indexOf("\nCHANNEL6");
      if(!clientRequest.substring(index+10,lastindex).equals("")){
        canales1[4] = clientRequest.substring(index+10,lastindex-1);
      }
    }
    if((index = clientRequest.lastIndexOf("CHANNEL6= "))>0){
      int lastindex = clientRequest.indexOf("\nCHANNEL7");
      if(!clientRequest.substring(index+10,lastindex).equals("")){
        canales1[5] = clientRequest.substring(index+10,lastindex-1);
      }
    }
    if((index = clientRequest.lastIndexOf("CHANNEL7= "))>0){
      int lastindex = clientRequest.indexOf("\nCHANNEL8");
      if(!clientRequest.substring(index+10,lastindex).equals("")){
        canales1[6] = clientRequest.substring(index+10,lastindex-1);
      }
    }
    if((index = clientRequest.lastIndexOf("CHANNEL8= "))>0){
      int lastindex = clientRequest.indexOf("\n[END]");
      if(!clientRequest.substring(index+10,lastindex).equals("")){
        canales1[7] = clientRequest.substring(index+10,lastindex-1);
      }
    }


  clientRequest = "";
  if(!SD.exists("datos/final.csv")){
    File modeloFinal = SD.open("datos/final.csv", FILE_WRITE);
    modeloFinal.println("Subsistema o prototipo; Ciclado; Responsable; Objetivo de la prueba; Inicio; Final; Muestreo; COM1; Tipo");
    modeloFinal.print(subsystem);
    modeloFinal.print("; ");
    //Serial.println(cicle);
    modeloFinal.print(cicle);
    modeloFinal.print("; ");
    modeloFinal.print(responsible);
    modeloFinal.print("; ");
    modeloFinal.print(objective);
    modeloFinal.print("; ");
    modeloFinal.print(((epoch+3600)  % 86400L) / 3600); // print the hour (86400 equals secs per day)
    modeloFinal.print(":");
    if (((epoch % 3600) / 60) < 10) {
      // In the first 10 minutes of each hour, we'll want a leading '0'
      modeloFinal.print("0");
    }
    modeloFinal.print((epoch  % 3600) / 60); // print the minute (3600 equals secs per minute)
    modeloFinal.print(":");
    if ((epoch % 60) < 10) {
      // In the first 10 seconds of each minute, we'll want a leading '0'
      modeloFinal.print("0");
    }
    modeloFinal.print(epoch % 60);
    modeloFinal.print("; ");
    modeloFinal.print(((epoch+3600)  % 86400L) / 3600); // print the hour (86400 equals secs per day)
    modeloFinal.print(":");
    if (((epoch % 3600) / 60) < 10) {
      // In the first 10 minutes of each hour, we'll want a leading '0'
      modeloFinal.print("0");
    }
    modeloFinal.print((epoch  % 3600) / 60); // print the minute (3600 equals secs per minute)
    modeloFinal.print(":");
    if ((epoch % 60) < 10) {
      // In the first 10 seconds of each minute, we'll want a leading '0'
      modeloFinal.print('0');
    }
    modeloFinal.print(epoch % 60);
    modeloFinal.print("; ");
    modeloFinal.print(tempPeriod);
    modeloFinal.print("; ");
    modeloFinal.print(port1);
    modeloFinal.print("; ");
    modeloFinal.println(type);
    modeloFinal.println();
    modeloFinal.print("t(seg); Hora PC; ");
    for (size_t i = 0; i < 8; i++) {
      if(!canales1[i].equals("")){
        modeloFinal.print(canales1[i]);
        if(i<7){
          if(!canales1[i+1].equals("")){
            modeloFinal.print("; ");
          }
        }
      }

    }
    modeloFinal.println();
    modeloFinal.close();
  }else{
    File final = SD.open("datos/final.csv", FILE_READ);
    String config = "";
    while(final.available()){
      char ltr = final.read();
      config += ltr;
    }
    String tts = config.substring(config.lastIndexOf('\n'));
    tts = tts.substring(0, tts.indexOf(','));
    secsInicio = tts.toInt();
  }
}
