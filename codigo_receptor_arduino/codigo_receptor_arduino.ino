#include <SPI.h>
#include <RF24.h>
#include <Servo.h>
 
RF24 radio(7, 8); // CE, CSN
 
const uint8_t addresses[][6] = {"1Node","2Node","3Node","4Node","5Node"};

int datos[5];

//Motor trasero
int MOTORA1 = 10;
int MOTORA2 = 9;

//Motor de dirección
int MOTORB1 = 5;
int MOTORB2 = 6;

//Servomotor cámara
Servo servocam; 

//Luces delanteras y traseras
int lightfront = 2;
int lightback = 4;

//LED de conexión
int connled = 1;


void setup() 
{
  pinMode(MOTORA1, OUTPUT);
  pinMode(MOTORA2, OUTPUT);
  pinMode(MOTORB1, OUTPUT);
  pinMode(MOTORB2, OUTPUT);

  servocam.attach(3);

  pinMode(lightfront, OUTPUT);
  pinMode(lightback, OUTPUT);
  pinMode(connled, OUTPUT);
 
  radio.begin();
 
  radio.openReadingPipe(1,addresses[0]);
  radio.openReadingPipe(2,addresses[1]);
  radio.openReadingPipe(3,addresses[2]);
  radio.openReadingPipe(4,addresses[3]);
  radio.openReadingPipe(5,addresses[4]);
 
  radio.setPALevel(RF24_PA_MIN);
 
  radio.startListening();

  //secuencia de inicio
  digitalWrite(connled, HIGH);
  delay(300);
  digitalWrite(connled, LOW);
  delay(200);
  digitalWrite(connled, HIGH);
  delay(100);
  digitalWrite(connled, LOW);
  delay(100);
}


void MotorMain()
{
  if(datos[0] > 135)
  {
    analogWrite(MOTORA1, datos[0]);
    analogWrite(MOTORA2, 0);
    
    if(datos[4] == 1)
    {
      digitalWrite(lightfront, HIGH);
    }
  }
  else if(datos[0] < 125)
  {
    analogWrite(MOTORA1, 0);
    analogWrite(MOTORA2, 255-datos[0]);

    if(datos[4] == 1)
    {
      digitalWrite(lightback, HIGH);
    }
  }
  else
  {
    analogWrite(MOTORA1, 0);
    analogWrite(MOTORA2, 0);
    digitalWrite(lightfront, LOW);
    digitalWrite(lightback, LOW);
  }
}


void MotorDirection()
{
  if(datos[2] == 1)
  {
    digitalWrite(MOTORB1, HIGH);
    digitalWrite(MOTORB2, LOW);
  }
  else if(datos[3] == 1) 
  {
    digitalWrite(MOTORB1, LOW);
    digitalWrite(MOTORB2, HIGH);    
  }
  else
  {
    digitalWrite(MOTORB1, LOW);
    digitalWrite(MOTORB2, LOW); 
  }
}


void ServoCam()
{
  servocam.write(datos[1]);
}


void loop() 
{
  if (radio.available()) 
  {
    digitalWrite(connled, HIGH);
    
    radio.read(&datos, sizeof(datos)); 
    
    MotorMain();
    MotorDirection();
    //ServoCam();
  }
  else
  {
    digitalWrite(connled, LOW);
    analogWrite(MOTORA1, 0);
    analogWrite(MOTORA2, 0);
    digitalWrite(lightfront, LOW);
    digitalWrite(lightback, LOW);
    digitalWrite(MOTORB1, LOW);
    digitalWrite(MOTORB2, LOW);
    //servocam.write(90); 
  }
}
