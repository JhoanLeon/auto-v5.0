////// LIBRARIES //////
#include <SPI.h>
#include <RF24.h>
#include <Servo.h>

//#define __DEBUG__


////// TYPEDEF //////
union uint16_as_bytes
{
  uint16_t ival;
  uint8_t bval[2];
} uint16_2_bytes;


////// PIN DEFINITIONS //////
uint8_t LIGHTS_FRONT = 2;
uint8_t LIGHTS_BACK = 10;

uint8_t SERVOMOTOR = 3;

uint8_t MOTOR_BACK_1 = 5;
uint8_t MOTOR_BACK_2 = 6;

uint8_t MOTOR_FRONT_1 = 4;
uint8_t MOTOR_FRONT_2 = 7;

Servo Servo_Cam;

uint8_t BATTERY_PIN = A7;


////// NRF24 DEFINITIONS //////
const uint8_t tx_pipe[6] = "00001"; // 5 bytes address, this code should be the same for the receiver
const uint8_t rx_pipe[6] = "00002";

RF24 radio(8, 9);  // set CE and CSN pins

// the size of this struct should not exceed 32 bytes
struct TX_Data {
  uint8_t ch1;
  uint8_t ch2;
  uint8_t ch3;
  uint8_t ch4;
  uint8_t ch5;
  uint8_t ch6;
  uint8_t ch7;
  uint8_t ch8;
};

// the size of this struct should not exceed 32 bytes
struct RX_Data {
  uint8_t ch1;
  uint8_t ch2;
};

TX_Data tx_data; // create a variable with the structure 
RX_Data rx_data; // create a variable with the structure 


////// RF CONNECTION VARIABLES //////
uint16_t car_battery_data = 0;

unsigned long last_msg_time = 0;

uint8_t FIRST_CONNECTION_FLAG = false;


void Motor_Back(uint8_t p_velocity, uint8_t p_lights)
{
  if (p_velocity > 130)
  {
    analogWrite(MOTOR_BACK_1, p_velocity);
    analogWrite(MOTOR_BACK_2, 0);

    if (p_lights == 1)
    {
      digitalWrite(LIGHTS_FRONT, HIGH);
      digitalWrite(LIGHTS_BACK, LOW);
    }
    else
    {
      digitalWrite(LIGHTS_FRONT, LOW);
      digitalWrite(LIGHTS_BACK, LOW);    
    }
  }
  else if (p_velocity < 120)
  {
    analogWrite(MOTOR_BACK_1, 0);
    analogWrite(MOTOR_BACK_2, 255-p_velocity);

    if (p_lights == 1)
    {
      digitalWrite(LIGHTS_FRONT, LOW);
      digitalWrite(LIGHTS_BACK, HIGH);
    }
    else
    {
      digitalWrite(LIGHTS_FRONT, LOW);
      digitalWrite(LIGHTS_BACK, LOW);    
    }
  }
  else
  {
    analogWrite(MOTOR_BACK_1, 0);
    analogWrite(MOTOR_BACK_2, 0);
    digitalWrite(LIGHTS_FRONT, LOW);
    digitalWrite(LIGHTS_BACK, LOW);   
  }
}


void Motor_Front(uint8_t p_right, uint8_t p_left)
{
  if ( (p_right == 1) && (p_left == 1) )
  {
    digitalWrite(MOTOR_FRONT_1, LOW);
    digitalWrite(MOTOR_FRONT_2, LOW);  
  }
  else if (p_right == 1)
  {
    digitalWrite(MOTOR_FRONT_1, HIGH);
    digitalWrite(MOTOR_FRONT_2, LOW);
  }
  else if (p_left == 1) 
  {
    digitalWrite(MOTOR_FRONT_1, LOW);
    digitalWrite(MOTOR_FRONT_2, HIGH);    
  }
  else
  {
    digitalWrite(MOTOR_FRONT_1, LOW);
    digitalWrite(MOTOR_FRONT_2, LOW);
  }
}


void Lights_Sequence(uint8_t p_type)
{
  if (p_type == 1)
  {
    digitalWrite(LIGHTS_FRONT, HIGH);
    digitalWrite(LIGHTS_BACK, HIGH);
    delay(100);
    digitalWrite(LIGHTS_FRONT, LOW);
    digitalWrite(LIGHTS_BACK, LOW);
    delay(100);
    digitalWrite(LIGHTS_FRONT, HIGH);
    digitalWrite(LIGHTS_BACK, HIGH);
    delay(100);
    digitalWrite(LIGHTS_FRONT, LOW);
    digitalWrite(LIGHTS_BACK, LOW);
    delay(100);
    digitalWrite(LIGHTS_FRONT, HIGH);
    digitalWrite(LIGHTS_BACK, HIGH);
    delay(100);
    digitalWrite(LIGHTS_FRONT, LOW);
    digitalWrite(LIGHTS_BACK, LOW);
    delay(100);    
  }
  else if (p_type == 2)
  {
    digitalWrite(LIGHTS_FRONT, HIGH);
    digitalWrite(LIGHTS_BACK, HIGH);
    delay(200);
    digitalWrite(LIGHTS_FRONT, LOW);
    digitalWrite(LIGHTS_BACK, LOW);
    delay(200);
    digitalWrite(LIGHTS_FRONT, HIGH);
    digitalWrite(LIGHTS_BACK, HIGH);
    delay(200);
    digitalWrite(LIGHTS_FRONT, LOW);
    digitalWrite(LIGHTS_BACK, LOW);
    delay(200);
  }
  else
  {
    digitalWrite(LIGHTS_FRONT, LOW);
    digitalWrite(LIGHTS_BACK, LOW);
  }
}


void setup() 
{
  #ifdef __DEBUG__
  Serial.begin(115200);
  #endif

  pinMode(LIGHTS_FRONT, OUTPUT);
  pinMode(LIGHTS_BACK, OUTPUT);
  
  pinMode(MOTOR_BACK_1, OUTPUT);
  pinMode(MOTOR_BACK_2, OUTPUT);
  
  pinMode(MOTOR_FRONT_1, OUTPUT);
  pinMode(MOTOR_FRONT_2, OUTPUT);
  
  Servo_Cam.attach(SERVOMOTOR);

  rx_data.ch1 = 0; // car battery level
  rx_data.ch2 = 0; // car battery level

  radio.begin();
  radio.setAutoAck(false);
  radio.setPALevel(RF24_PA_HIGH);
  radio.setDataRate(RF24_250KBPS);
  radio.openReadingPipe(0, tx_pipe);
  radio.openWritingPipe(rx_pipe);
  radio.startListening(); // stablish device as receptor

  // initialization sequence
  Lights_Sequence(1);

  #ifdef __DEBUG__
  Serial.println("Hello World Car!");
  #endif
}


void loop() 
{
  if (radio.available())
  {
    last_msg_time = millis(); // register the time to check the connection state 
    
    if (FIRST_CONNECTION_FLAG == false) // first time connection
    {
      Lights_Sequence(2);
      
      FIRST_CONNECTION_FLAG = true;
    }
    else
    {
      // do nothing
    }
    
    radio.read(&tx_data, sizeof(tx_data));

    if (tx_data.ch5 == 1) // enable of control
    {
      Motor_Back(tx_data.ch3, tx_data.ch8);
      Motor_Front(tx_data.ch6, tx_data.ch7);
      Servo_Cam.write(tx_data.ch4);     
    }
    else
    {
      // do not execute commands
    }
  }
  else
  {
    radio.stopListening(); // stablish device as transmitter
    
    car_battery_data = map( analogRead(BATTERY_PIN), 0, 1024, 0, 5000 );

    uint16_2_bytes.ival = car_battery_data;
    rx_data.ch1 = uint16_2_bytes.bval[1]; // MSB
    rx_data.ch2 = uint16_2_bytes.bval[0]; // LSB
    radio.write(&rx_data, sizeof(rx_data));

    if (millis() - last_msg_time > 1000) // 1s without new message from control
    {
      // stop movement of car
      analogWrite(MOTOR_BACK_1, 0);
      analogWrite(MOTOR_BACK_2, 0);
      digitalWrite(MOTOR_FRONT_1, LOW);
      digitalWrite(MOTOR_FRONT_2, LOW);
      digitalWrite(LIGHTS_FRONT, LOW);
      digitalWrite(LIGHTS_BACK, LOW);
    }
    else
    {
      // do nothing
    }
    
    radio.startListening(); // stablish device as receiver
    delay(10);
  }

  #ifdef __DEBUG__
  Serial.print(last_msg_time);
  Serial.print(" ; ");
  Serial.print(car_battery_data);
  Serial.print(" ; ");
  Serial.print(rx_data.ch1);
  Serial.print(" ; ");
  Serial.print(rx_data.ch2);
  Serial.print(" ; ");
  Serial.print(tx_data.ch1);
  Serial.print(" ; ");
  Serial.print(tx_data.ch2);
  Serial.print(" ; ");
  Serial.print(tx_data.ch3);
  Serial.print(" ; ");
  Serial.print(tx_data.ch4);
  Serial.print(" ; ");
  Serial.print(tx_data.ch5);
  Serial.print(" ; ");
  Serial.print(tx_data.ch6);
  Serial.print(" ; ");
  Serial.print(tx_data.ch7);
  Serial.print(" ; ");
  Serial.println(tx_data.ch8);
  #endif
}
