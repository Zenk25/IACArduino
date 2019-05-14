//Clase canal, se encargará de guardar la medición del canal, correspondiente.
class Channel{
   private:
      // Datos de la medición.
      char valor[7];
      //Numero del canal actual.
      int numCanal;
      int port;

   public:
     //Constructores
      Channel();
      Channel(int numCanal2,int port2);
      // Funcion que devuelve el codigo para la lectura
      void codigoPeticion();
      char* leer();
      void setNumCanal(int numCanal2);
      void setPort(int port2);

};
