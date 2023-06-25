//Segundo Parcial SPD | Parte Práctica Domiciliaria

//Alumno: Mateo Spatola
//Division: 1D

//Enunciado:
/*
https://drive.google.com/file/d/1Af93a100_PRCv27KyVkwG_JjW-Zck8Dk/view
*/

#include <Servo.h>
Servo miServo;

#define TMP A0

#include <IRremote.h>
#define BOTON_ON_OFF 0xFF00BF00

#define SENSOR_IR 11

#define LED_VERDE A2
#define LED_ROJO A1

#include <LiquidCrystal.h>
LiquidCrystal LCD(4, 5, 6, 7, 8, 9);

bool flagLedVerdeEncendido;
bool flagLedRojoEncendido;

bool flagSistemaEncendido = false;

int lectura_temperatura;
int temperatura;
String mensaje;

void setup()
{
  //Salida digital
  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_ROJO, OUTPUT);
  
  //Entrada digital
  miServo.attach(3,500,2500);

  Serial.begin(9600);
  IrReceiver.begin(SENSOR_IR, DISABLE_LED_FEEDBACK);
  LCD.begin(16,2);
  Serial.println("Sistema de incendio conectado :)");
}

void loop()
{
  apagarEncenderConControlRemoto();
  
  if(flagSistemaEncendido == true)
  {
    prenderLed(LED_VERDE);
    apagarLed(LED_ROJO);
    
    lectura_temperatura = analogRead(TMP);
    temperatura = map(lectura_temperatura, 20, 358,-40,125);
    
    detectarSiHayIncendio(temperatura);
    modificarMensajeLCDSegunTemperatura(temperatura);
  }
  else
  {
    prenderLed(LED_ROJO);
    apagarLed(LED_VERDE);
    miServo.write(0);
    LCD.clear();
  }
}


//Funciones
void prenderLed(int led)
{
  switch(led)
  {
    case LED_VERDE:
    if(flagLedVerdeEncendido == false)
    {
      Serial.println("- Led verde encendido");
      flagLedVerdeEncendido = true;
    }
    break;
    case LED_ROJO:
    if(flagLedRojoEncendido == false)
    {
      Serial.println("- Led rojo encendido");
      flagLedRojoEncendido = true;
    }
    break;
  }
  digitalWrite(led, HIGH);
}

void apagarLed(int led)
{
  switch(led)
  {
    case LED_VERDE:
    if(flagLedVerdeEncendido == true)
    {
      Serial.println("- Led verde apagado");
      flagLedVerdeEncendido = false;
    }
    break;
    case LED_ROJO:
    if(flagLedRojoEncendido == true)
    {
      Serial.println("- Led rojo apagado");
      flagLedRojoEncendido = false;
    }
  }
  digitalWrite(led, LOW);
}

void apagarEncenderConControlRemoto()
{
  if(IrReceiver.decode())
  {
    Serial.print("Boton presionado --> ");
    Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX);
    if(IrReceiver.decodedIRData.decodedRawData == BOTON_ON_OFF)
    { 
      if(flagSistemaEncendido == false)
      {
        Serial.println("Sistema encendido");
        flagSistemaEncendido = true;
      }
      else
      {
        Serial.println("Sistema apagado");
        flagSistemaEncendido = false;
      }
    }
    IrReceiver.resume();
  }
}

void detectarSiHayIncendio(int temperatura)
{
  if(temperatura >= 60)
  {
    miServo.write(180);
    digitalWrite(LED_ROJO, HIGH);
    delay(200);
    digitalWrite(LED_ROJO, LOW);
    delay(200);
  }
  else
  {
    miServo.write(0);
  }
}

void modificarMensajeLCDSegunTemperatura(int temperatura)
{
  if(temperatura >= 60)
  {
    mensaje = "HAY UN INCENDIO!";
  } 
  else if(temperatura >= 30 && temperatura < 60)
  {
    mensaje = "Estacion: Verano";
  } 
  else if(temperatura >= 20 && temperatura < 30)
  {
    mensaje = "Estacion: Primavera";
  } 
  else if(temperatura >= 10 && temperatura < 20)
  {
    mensaje = "Estacion: Otoño";
  } 
  else
  {
    mensaje = "Estacion: Invierno";
  }

  LCD.setCursor(0, 0);
  LCD.print("Temperatura: ");
  LCD.print(temperatura);
  LCD.print("  ");

  LCD.setCursor(0, 1);
  LCD.print(mensaje);
}
