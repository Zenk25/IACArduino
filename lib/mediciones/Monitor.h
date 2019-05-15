/*#include "Channel.h"
#include <Array.h>

using namespace std;

class Monitor {
   private:
      // Datos miembro de la clase Monitor
      int numCanales[];
      int numMonitor;
      int canalesLength;
      char sda[1]= {'1'};
      Array<char> temperaturas = Array<char>(sda, 1);
      Channel canales[];
      unsigned int localPort = 8888;       // local port to listen for UDP packetss
       // NTP time stamp is in the first 48 bytes of the message
      byte packetBuffer[NTP_PACKET_SIZE];
      //  byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets
      EthernetUDP Udp;

   public:
      // Funciones miembro de la clase Monitor
      Monitor(Array<int> numCanales, int port);
      void setCanales(Channel* aux, int length);
      Channel getCanal(int numCanal);
      Array<char> getTempCanales();
      void setTempCanales();
      boolean regexComparator(char temp[]);
      String tempsToJson(boolean comprobado);
      String horaFecha();
      void sendNTPpacket(const char* address);

};
*/
