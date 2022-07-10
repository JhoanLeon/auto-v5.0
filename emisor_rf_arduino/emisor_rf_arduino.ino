/* Tranmsitter code for the Arduino Radio control with PWM output
 * Install the NRF24 library to your IDE
 * Upload this code to the Arduino UNO, NANO, Pro mini (5V,16MHz)
 * Connect a NRF24 module to it:
 
    Module // Arduino UNO,NANO
    
    GND    ->   GND
    VCC    ->   3.3V
    CE     ->   D9
    CSN    ->   D10
    CLK    ->   D13
    MOSI   ->   D11
    MISO   ->   D12

    OLED // Arduino UNO, NANO
    A4 -> (SDA) and 
    A5 -> (SCL) I2C interface 

This code transmits 7 channels with data from pins A0, A1, A2, A3, D2 and D3
*/

#define __DEBUG__

////// LIBRARIES //////
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


////// NRF24 DEFINITIONS //////
const uint64_t my_radio_pipe = 0xE8E8F0F0F0; // 40bits address, Remember that this code should be the same for the receiver

RF24 radio(7, 8);  //Set CE and CSN pins

// The sizeof this struct should not exceed 32 bytes
struct Data_to_be_sent {
  byte ch1;
  byte ch2;
  byte ch3;
  byte ch4;
  byte ch5;
  byte ch6;
};

// Create a variable with the structure above and name it sent_data
Data_to_be_sent sent_data;


////// OLED SCREEN DEFINITIONS //////
#define WIDTH         128 // width OLED
#define HEIGHT        64  // height OLED

Adafruit_SSD1306 display(WIDTH, HEIGHT, &Wire, -1); // Object of Adafruit_SSD1306 class 


////// PIN DEFINITIONS //////
int battery_pin = A0;

int joystick_x = A1;
int joystick_y = A2;
int potenciometer = A3;

int btn_right = 2;
int btn_left = 4;
int btn_lights = 6;


////// BATTERY VARIABLES //////
byte control_batt_level = 0;
byte car_batt_level = 0;


void setup()
{
  // Configure pins
  pinMode(btn_right, INPUT);
  pinMode(btn_left, INPUT);
  pinMode(btn_lights, INPUT);

  // Configure NRF24 transmitter
  radio.begin();
  radio.setAutoAck(false);
  radio.setPALevel(RF24_PA_HIGH);
  radio.setDataRate(RF24_250KBPS);
  radio.openWritingPipe(my_radio_pipe);
  radio.stopListening(); // stablish device as transmitter
  
  // Configure OLED screen


  // Reset each channel value
  sent_data.ch1 = 127; // joystick_x
  sent_data.ch2 = 127; // joystick_y
  sent_data.ch3 = 0; // potenciometer 
  sent_data.ch4 = 0; // button right
  sent_data.ch5 = 0; // button left
  sent_data.ch6 = 0; // button lights

  // Print an initialization sequence on screen


}


void loop()
{
  /*If your channel is reversed, just swap 0 to 255 by 255 to 0 below
  EXAMPLE:
  Normal:    data.ch1 = map( analogRead(A0), 0, 1024, 0, 255);
  Reversed:  data.ch1 = map( analogRead(A0), 0, 1024, 255, 0);  */

  // read all data from control to send to car
  sent_data.ch1 = map( analogRead(joystick_x), 0, 1024, 0, 255 );
  sent_data.ch2 = map( analogRead(joystick_y), 0, 1024, 0, 255 );
  sent_data.ch3 = map( analogRead(potenciometer), 0, 1024, 0, 255 );
  sent_data.ch4 = digitalRead(btn_right);
  sent_data.ch5 = digitalRead(btn_left);
  sent_data.ch6 = digitalRead(btn_lights);

  // send data to the car
  radio.write(&sent_data, sizeof(Data_to_be_sent));

  // read battery level of control
  control_batt_level = map( analogRead(battery_pin), 0, 1024, 0, 100 ); // hardware voltage divisor set to get 7.4V (8V) as 1024 value (5V)

  // read battery level from car and check connection of car receptor


  // print the battery level of the car and control (values between 0% a 100%)
  // print the state of connection of the car
  // print the current value of the joystick axes
  // print the current value of the potenciometer
  // print symbols for turn to right and left
  // print the current state of lights
}
