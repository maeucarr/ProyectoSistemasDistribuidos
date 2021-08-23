#include <ESP8266WiFi.h>      //Libreria para la conexión WiFi
#include <SoftwareSerial.h>   //Libreria para la comunicación Serial 
#include <FirebaseArduino.h>  //Libreria para la conexión a base de datos en tiempo real FireBase

#define FIREBASE_HOST "amst-grupo4-camas-uci.firebaseio.com"      //API KEY o Dirección del HOST de la base de datos
#define FIREBASE_AUTH "6aMyQhLCpma4yjVP0L1eZrmi2SGMbYsO8HKSRBej"  //"Secretos de la base de datos".

const char* ssid = "NETLIFE-BAILON";      //Nombre de la red WiFi.
const char* password = "ENRIQUE092427";   //Clave de la red WiFi.
const String direccion = "hospital-prueba/secciones/1/camas/1/dispositivo/estado"; //Direccion en donde se ubica el campo "Estado" de la cama.
const String direccion_bat = "hospital-prueba/secciones/1/camas/1/dispositivo/bateria";
String estado;  //Estado de la cama.
SoftwareSerial mySerial(4,5);
// NUEVO

const byte numChars = 32;
char receivedChars[numChars];
boolean newData=false;
int dataNumber=0;

// NUEVO
void setup() {
  Serial.begin(115200); // BaudRate de 115200 para la comunicación Serial.
  mySerial.begin(115200);
  pinMode(4,INPUT);
  WiFi.mode(WIFI_STA);            // Modo WiFi Station para conectarse a la red.
  WiFi.begin(ssid,password);      // Inicio del intento de conexión a la red.
  Serial.print("Conectando a:\t");// Mensaje de conexión a la red WiFi.
  Serial.println(ssid);           // Imprimir nombre de la red.

  // Lazo while ejecutado hasta conectarse exitosamente a la red.
  while(WiFi.status() != WL_CONNECTED){
    delay(200);
    Serial.print(".");
  }

  //Impresion por consola de la conexión exitosa a la red.
  Serial.println();
  Serial.print("Conectado a:\t");
  Serial.println(WiFi.SSID());
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());
  
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}

void loop() {
  if(Firebase.failed()){
    delay(500);
    Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
    Serial.println("Error");
    delay(500);
  } else{
    char bateria[32];
    int bateria_real;
    char x;             // Variable a almacenar el valor enviado.
    mySerial.flush();     // Método para recibir exitosamente los datos.
    if( mySerial.available() != 0){ // Valida que existan datos a recibir.
      
      x = mySerial.read();// Lectura del dato recibido.
      Serial.println(x);
      if( x == 'o'){         
        estado = "Ocupado";       // Se establece el String de Ocupado si el datos recibido es 1.
      } else if( x == 'd'){  
        estado = "Desocupado";    // Se establece el String de Desocupado si el datos recibido es 0.
      } else {
        for(int i =0; i<2;i++){
          bateria[i] = x;
          if(i!=2){
            x = mySerial.read();
          }
        }
        bateria_real = atoi(bateria);
      }
      //Serial.println("Estado: + " + estado + ".");// Impresion en consola del estado actual.
      }
    if(!(Firebase.getString(direccion).equals(estado)) && estado != "." && estado != ""){
      Firebase.setString(direccion,estado);       // Envio de datos a la base de datos Firebase para su actualización.  
    }
    if(!(Firebase.getInt(direccion_bat) == bateria_real) && bateria_real <=100){
      Firebase.setInt(direccion_bat,bateria_real);       // Envio de datos a la base de datos Firebase para su actualización.  
    }
  }

  delay(1000); // Espera de un segundo durante cada iteración.
}
