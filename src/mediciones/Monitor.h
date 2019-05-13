using namespace std;
const int NTP_PACKET_SIZE = 48;

class Monitor {
   private:
      // Datos miembro de la clase Monitor
      int numCanales,numMonitor;
      Channel canales[];
      String nombreCanales[];
      unsigned int localPort = 8888;       // local port to listen for UDP packets
      const char timeServer[] = "1.europe.pool.ntp.org"; // time.nist.gov NTP server
       // NTP time stamp is in the first 48 bytes of the message
      byte packetBuffer[NTP_PACKET_SIZE];
      //  byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets
      EthernetUDP Udp;

   public:
      // Funciones miembro de la clase Monitor
      Monitor(Channel* canales);
      void setCanales(int numCanales[]);
      char* getCanales();
      boolean regexComparator(char temp[]);
      String tempsToJson(boolean comprobado);
      String horaFecha();
      void sendNTPpacket(const char* address);

};
