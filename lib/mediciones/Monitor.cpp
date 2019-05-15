/*#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>
#include <Array.h>
#include <Regexp.h>
#include "Monitor.h"

using namespace std;
//Constructor de la clase monitor
Monitor::Monitor(Array<int> numCanales,int port){
    Channel final[numCanales.size()];
    for (int i = 0; i < numCanales.size(); i++) {

      switch(numCanales[i]){
        case 1:
        {
          Channel channel1(numCanales[i], port);
          final[i] = channel1;
          break;
        }
        case 2:
        {
          Channel channel2(numCanales[i], port);
          final[i] = channel2;
          break;
        }
        case 3:
        {
          Channel channel3(numCanales[i], port);
          final[i] = channel3;
          break;
        }
        case 4:
        {
          Channel channel4(numCanales[i], port);
          final[i] = channel4;
          break;
        }
        case 5:
        {
          Channel channel5(numCanales[i], port);
          final[i] = channel5;
          break;
        }
        case 6:
        {
          Channel channel6(numCanales[i], port);
          final[i] = channel6;
          break;
        }
        case 7:
        {
          Channel channel7(numCanales[i], port);
          final[i] = channel7;
          break;
        }
        case 8:
        {
          Channel channel8(numCanales[i], port);
          final[i] = channel8;
          break;
        }
      }
    }
    setCanales(final, numCanales.size());
}

//Asignarás los canales
void Monitor::setCanales(Channel* aux, int length){

  for (int i = 0; i < length; i++) {
    Monitor::canales[i] = aux[i];
  }
}
//Devolverá los canales
Channel Monitor::getCanal(int numCanal){
  return Monitor::canales[numCanal-1];
}

Array<char> Monitor::getTempCanales(){
    return temperaturas;
}

void Monitor::setTempCanales(){
  char aux[canalesLength][7];
  for (int i = 0; i < canalesLength; i++) {
    for (int j = 0; j < 7; j++) {
      aux[i][j] = Monitor::canales[i].leer()[j];
    }
  }
  temperaturas = Array<char>(*aux,canalesLength);
}


boolean Monitor::regexComparator(char temp[]){
  MatchState ms;
  ms.Target(temp);
  char result = ms.Match("[+-][0-9]*[.][0-9]*");
  if (result == REGEXP_MATCHED){
    return true;
  }else{
    return false;
  }
}

String Monitor::tempsToJson(boolean comprobado){
  String Json ="";
  Json += "{secs = \"";
  Json += Monitor::horaFecha();
  Json += ",";
  for (int i = 0; i < temperaturas.size(); i++) {
    Json += "\"Temp";
    Json += i;
    Json += "\"= \"";
    Json += temperaturas[i];
    Json += "\"";
    if(i<temperaturas.size()){
      Json += ",";
    }
  }
  return Json;
}

String Monitor::horaFecha(){
    String fecha;
    sendNTPpacket("1.europe.pool.ntp.org"); // send an NTP packet to a time server

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
    fecha = (((epoch+3600)  % 86400L) / 3600); // print the hour (86400 equals secs per day)
    fecha+=':';
    if (((epoch % 3600) / 60) < 10) {
      // In the first 10 minutes of each hour, we'll want a leading '0'
      fecha+='0';
    }
    fecha+=((epoch  % 3600) / 60); // print the minute (3600 equals secs per minute)
    fecha+=':';
    if ((epoch % 60) < 10) {
      // In the first 10 seconds of each minute, we'll want a leading '0'
      fecha+=('0');
    }
    fecha+=(epoch % 60); // print the second
    //Añadir fecha a la hora mostrada.
    Ethernet.maintain();
    return fecha;
}

void Monitor::sendNTPpacket(const char* address){
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
*/
