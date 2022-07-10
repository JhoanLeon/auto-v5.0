#include <SPI.h>
#include <RF24.h>
 
RF24 radio(7, 8); // Crea un objeto NRF24 con CE, CSN
 
const uint8_t addresses[][6] = {"1Node","2Node","3Node","4Node","5Node"};

int datos[5];

int velocityPin = A0;
int velocityValue = 0;

int servoPin = A1;
int servoValue = 0;

int btnright = 2;
int btnleft = 4;

int btnlights = 6;


void setup() 
{
  pinMode(btnleft, INPUT);
  pinMode(btnright, INPUT);
  pinMode(btnlights, INPUT);
  
  radio.begin();
 
  radio.openWritingPipe(addresses[0]);

  radio.setPALevel(RF24_PA_MIN);
 
  radio.stopListening(); // Establece que se va a enviar información
}


void MainSend()
{
  velocityValue = analogRead(velocityPin);
  velocityValue = map(velocityValue, 0, 1023, 0, 255);

  datos[0] = velocityValue;

  servoValue = analogRead(servoPin);
  servoValue = map(servoValue, 0, 1023, 0, 180);
  
  datos[1] = servoValue;

  datos[2] = digitalRead(btnright);
  datos[3] = digitalRead(btnleft);

  datos[4] = digitalRead(btnlights); 
}

 
void loop() 
{
  MainSend();
 
  radio.write(&datos, sizeof(datos));
 
  delay(5); // Espera entre envío y envío
}
