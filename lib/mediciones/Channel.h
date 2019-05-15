//Clase canal, se encargará de guardar la medición del canal, correspondiente.
class Channel{
   private:
      // Datos de la medición.
      //Numero del canal actual.
      int numCanal;
      int port;
      Array<char> valor = Array<char>(sadfaf,7);;
      char sadfaf[7] = {'x','x','x','x','x','x','x'};
      Array<char> nulo = Array<char>(sadfaf, 7);

   public:
     //Constructores
      Channel();
      Channel(int numCanal2,int port2);
      ~Channel();
      void destruir();
      // Funcion que devuelve el codigo para la lectura
      void codigoPeticion();
      Array<char> leer();
      void setNumCanal(int numCanal2);
      void setPort(int port2);

};
