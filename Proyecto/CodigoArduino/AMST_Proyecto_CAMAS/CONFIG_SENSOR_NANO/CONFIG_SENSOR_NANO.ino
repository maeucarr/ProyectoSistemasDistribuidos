  #include <SoftwareSerial.h>  //Libreria para realizar la conexión Serial 
  #define pinA1 2
  #define LEDpin 8
  #define BatAlta 10
  #define BatMedia 11
  #define BatBaja 3
  #define AnalogPin 3
  
int limiteMaximoBateria;
int *bateria_Actual;
int bateria_Act;
int *bateria_Anterior;
int bateria_Ant;
String estado;     //Estado del sensor de presion -> Puede ser "ocupado" o "desocupado"
int ResRead;         // La Lectura de la Resistencia por División de Tensión.
int *pContador = 0;    // Contador único para detectar el primer estado de la cama al iniciar el sistema. 
int contador=0;
SoftwareSerial SUART(9,7);  //Comunicación serial con el ESP 8266.

//------------------------------ SET UP----------------------
void setup()
{
  Serial.begin(115200);       // Enviaremos la información de depuración a través del Monitor de Serial.
  iniciarConfiguracion();
  limiteMaximoBateria = calibrarBateria(1000, 470, 9);
  pContador = &contador;
  bateria_Anterior = &bateria_Ant;
  bateria_Actual = &bateria_Act;
}

// ---------------------------- LOOP -----------------------
void loop()
{
  //Sensar nivel de la bateria
  int porcentajeBateria = leerPorcentajeBateria(limiteMaximoBateria);
  *bateria_Actual = enviarBateria(porcentajeBateria);
  if( contador == 1){
    enviarEstadoBateria(*bateria_Anterior,*bateria_Actual);    
  }
  //Lectura del sensor
  int lectura = leerSensor(AnalogPin);
  // Detección del estado inicial del sensor y envio al ESP8266 a traves del método print.
  lecturaInicial(estado,lectura,*pContador,*bateria_Actual);
  //Envio de datos unicamente cuando existe un cambio de estado.
  estado = enviarDatosSerial(lectura,estado);
  delay(500); //Cien “ms” de espera en cada lectura
}

//------------------------- FUNCIONES ----------------------

void iniciarConfiguracion(){
  SUART.begin(115200);        // BaudRate al que trabaja el ESP8266.
  pinMode(LEDpin, OUTPUT);    // Pin de salida de LED conxión.
  // Luces LED indicadoras de porcentaje de batería.
  pinMode(BatBaja,OUTPUT);
  pinMode(BatMedia,OUTPUT);
  pinMode(BatAlta,OUTPUT);
  
  while(!Serial){             // Se asegura de realizar la conexión serial antes de avanzar al ciclo infinito.
  }       
  // Luz LED indicadora de bateria.    
  digitalWrite(LEDpin, HIGH); // Enciende el LED indicador del sistema activo.;
  // Mensaje de bienvenida.
  Serial.println("Bienvenidos a Get A Bed v1.0");
  
}

int leerSensor(int analogPin){
  ResRead = analogRead(analogPin); // La Resistencia es igual a la lectura del sensor (Analog 0)
  Serial.print("Lectura Analogica = ");
  Serial.println(ResRead);
  return ResRead;
}

void lecturaInicial(String &estado, int lectura,int &contador,int &porcentaje){
    if(lectura>500 & contador == 0){
      estado = "ocupado";
      Serial.print("ocupado");
      SUART.print('o');
      delay(3000);
    } else if(lectura<500 & contador == 0) {
      estado = "desocupado";
      SUART.print('d');
      delay(3000);
    }
    
    if(contador == 0){
      *bateria_Anterior = porcentaje;
      SUART.print(porcentaje);
    }
      // Bandera para detener la detección inicial del sensor.
    if(contador == 0){
      contador = contador + 1;
    }
}

String enviarDatosSerial(int lectura, String estado){
  if(lectura>500 && estado.equals("desocupado")){
    estado = "ocupado";
    SUART.print('o');
    Serial.println("Ocupado");
  } else if (ResRead<500 && estado.equals("ocupado")){
    estado = "desocupado";
    SUART.print('d');
    Serial.println("Desocupado");
  }
  return estado;
}
/**
   @brief   Función que se debe utilizar de forma obligatoria si se requiere obtener el porcentaje de batería.
            Es necesario que se implemente un divisor de voltaje.
            Establece el voltaje máximo de entrada que entrega la batería en el rango de [0, 1023].
            Debe utilizarse en la función setup() luego de inicializar las variables de la librería.
   @param   rBajo: valor de la resistencia en Ohmios (Ω) que se encuentra conectada al negativo.
   @param   rArriba: valor de la resistencia en Ohmios Ohmios (Ω) que está conectada al positivo.
   @param   VIn: voltaje de la batería, puede indicar un voltaje nominal o realizar mediciones con un multímetro.
*/
int calibrarBateria(float rBajo, float rArriba, float vIn) {
  float vMax = (rBajo / (rBajo + rArriba)) * vIn;
  int limiteMaximoBateria = (int)(vMax * (1023 / 5));
  Serial.print("Calibrado! - El valor analogico maximo de bateria es = ");
  Serial.println(limiteMaximoBateria);
  delay(1000);
  return limiteMaximoBateria;
}

/**
   @brief   Función que se encarga de mapear el nivel de la batería en el rango de [0, 100]
   @return  porcentajeBateria: valor de tipo int que indica el nivel actual de carga de la batería.
*/
int leerPorcentajeBateria(int limiteMaximoBateria) {
  int bateria = analogRead(pinA1);
  delay(5); // permite que se estabilice el convertidor analógico-digital (ADC).
  int porcentajeBateria = map((int) bateria, 0, limiteMaximoBateria, 0, 100);
  return porcentajeBateria;
}

/**
    @brief   Obtiene el nivel de bateria más bajo que ha sido censado.
             Si existe una caída del nivel de la batería mayor al 5% el porcentaje no cambiará.
    @param   porcentajeBatería: valor que será leído continuamente en el programa.
    @return  nivelBateriaMayor: devuelve el porcentaje de batería mayor que ha sido leído.
*/
int compararNivelBateria(int porcentajeBateria) {
  int nivelBateriaMayor = 100;
  if ((porcentajeBateria <= nivelBateriaMayor)) {
    nivelBateriaMayor = porcentajeBateria;
  }
  if ((porcentajeBateria > nivelBateriaMayor - 5)) {
    nivelBateriaMayor = porcentajeBateria;
  }
  return nivelBateriaMayor;
}

/**
   @brief   Función que determina el envío de la batería dado cierto intervalo de tiempo.
            La batería solo se envía cuando el nivel es mayor a 30%.
   @param   porcentaje:
*/
int enviarBateria(int porcentaje)
{
  int porcentaje_real = compararNivelBateria(porcentaje);
  if (porcentaje_real > 70) {
    digitalWrite(BatAlta,HIGH);
    digitalWrite(BatMedia,LOW);
    digitalWrite(BatBaja,LOW);
  } else if (30<porcentaje_real <=70){
    digitalWrite(BatAlta,LOW);
    digitalWrite(BatMedia,HIGH);
    digitalWrite(BatBaja,LOW);
  }else {
    digitalWrite(BatAlta,LOW);
   digitalWrite(BatMedia,LOW);
    digitalWrite(BatBaja,HIGH);
  }
  return porcentaje_real;
}

void enviarEstadoBateria(int &cargaAnterior, int &cargaActual){
  if(cargaAnterior != cargaActual){
    cargaAnterior = cargaActual;
    SUART.print(cargaActual);
  }
}

    
  
  
