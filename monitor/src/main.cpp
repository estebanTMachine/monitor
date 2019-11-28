/*
 * ----------------------------------
 * CALCULA DISTANCIA Y ENVÍA MENSAJE
 * ----------------------------------
 */


#include <Arduino.h>
#include <SoftwareSerial.h>

//Configarión de los pines serial por software
SoftwareSerial mySerial =  SoftwareSerial(10,11);
SoftwareSerial SIM900(7, 8); 

//Pines a utilizar por ultrasonido 
#define echoPin 10
#define trigPin 11

//variables a utilizar por el sensor de ultrasonido
unsigned int reading;
byte readByte;
byte read_buffer[4];
byte crcCalc;
word distance;
String outText;
int umbral = 500;
word distanciaAnterior;

void setup() {
  //
  // Configuraciones de puerto
  //
  mySerial.begin (9600);
  Serial.begin (9600);
  Serial.println("start");
  
  //
  // Borrar el búfer de lectura
  //
  for (byte loopstep = 0; loopstep <= 3; loopstep++) {
    read_buffer[loopstep] = 0;
  }
}
void loop() {
//Serial.println ("estoy en el loop");
//delay(2000);
medir();
Serial.println ("distancia= " + String (distance));
Serial.println ("distancia anterior= " + String (distanciaAnterior));
  if(distance < umbral){
    Serial.println("Primer if" + String(umbral));
    if(distance < distanciaAnterior){
      crece();
    }
    }
    if(distance >= distanciaAnterior){
      decrece();
    }
     
  

}


void medir (){
  //
  // Comprobación de la disponibilidad de datos en el puerto COM
  //
  if (mySerial.available() < 1) {
    return; 
  }
  //
  readByte = mySerial.read();
  for (byte loopstep = 0; loopstep <= 2; loopstep++) {
    read_buffer[loopstep] = read_buffer[loopstep + 01];
  }
  read_buffer[03] = readByte;  
  //
  // Análisis de buffer
  //
  if (read_buffer[00] != 0xff) {
    return; // este no es el comienzo de los datos
  };
  crcCalc = read_buffer[00] + read_buffer[01] + read_buffer[02];
  if (read_buffer[03] != crcCalc) {
    return; // la suma de comprobación del paquete de datos no coincide
  };
  //
  // cálculo de distancia
  //
  
  distanciaAnterior = distance;
  distance = (read_buffer[01] * 0xff) + read_buffer[02];
  //
  // conclusión
  //
  outText = "bytes: ";
  outText = String(outText + read_buffer[00]);
  outText = String(outText + "+");
  outText = String(outText + read_buffer[01]);
  outText = String(outText + "+");
  outText = String(outText + read_buffer[02]);
  outText = String(outText + "+");
  outText = String(outText + read_buffer[03]);
  outText = String(outText + " = ");
  outText = String(outText + distance);
  outText = String(outText + " mm");
  Serial.println(outText);
  //
  // pausa - 2 segundos, cualquier valor es posible
  //
  delay(2000);
  //
  // eliminar datos del búfer que subió allí durante una pausa
  //
  
  while (mySerial.available() > 0) {
     readByte = mySerial.read();

  }



}

void decrece(){
  Serial.println("Decrece" + String(umbral) + String(distanciaAnterior));
  if(distance > umbral +100){
  umbral = umbral + 100;
  Serial.println ("umbral= " + String(umbral));
  }
 }

void crece(){
  Serial.println("Crece" + String(umbral) + String(distanciaAnterior));
    if(distance < umbral){
      Serial.println("Mensaje");
      //envioMensaje();
      delay(15000);
      umbral= umbral - 100;
      Serial.println ("umbral= " + String(umbral));
    }
  }

void envioMensaje() {
  SIM900.begin(19200);//Arduino se comunica con el SIM900 a una velocidad de 19200bps
  delay(20000);//Tiempo prudencial para el escudo inicie sesión de red con tu operador
  
  SIM900.print("AT+CMGF=1\r"); // comando AT para configurar el SIM900 en modo texto
  delay(200);
    SIM900.println("AT + CMGS = \"+59898074609\"");//reemplzar por el número a enviar el mensaje
   
  delay(200);
  SIM900.println("ATENCION El agua esta subiendo");// Reemplzar por el texto a enviar
  delay(200);
  //Finalizamos este comando con el caracter de sustitución (→) código Ascii 26 para el envio del SMS
  SIM900.println((char)26); 
  delay(200);
  SIM900.println();
  Serial.println("SMS sent successfully");
  //iniciamos el ultrasonido
  mySerial.begin (9600);
  Serial.begin (9600);
  Serial.println("reinicio medidas");

}