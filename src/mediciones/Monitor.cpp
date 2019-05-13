#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>
#include <SD.h>
#include <TimerOne.h>
#include <MemoryFree.h>
#include <Regexp.h>
#include "Channel.h"
#include "Monitor.h"

Monitor::Monitor(Channel* canales){

}


void Monitor::setCanales(int numCanales[]){

}

char* Monitor::getCanales(){

  return {'a','b','c'};
}

boolean Monitor::regexComparator(char temp[]){

  return false;
}

String Monitor::tempsToJson(boolean comprobado){
  return "hola";
}

String Monitor::horaFecha(){

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
    return "Hola";
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
