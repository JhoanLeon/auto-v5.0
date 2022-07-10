/* Connections
 
    nRF24L01 (SPI interface) // Arduino UNO,NANO
    GND    ->   GND
    VCC    ->   3.3V
    CE     ->   D8
    CSN    ->   D9
    CLK    ->   D13
    MOSI   ->   D11
    MISO   ->   D12

    OLED (I2C interface) // Arduino UNO, NANO
    (SDA)  ->   A4   
    (SCL)  ->   A5  
*/

////// LIBRARIES //////
#include <SPI.h>
#include <RF24.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//#define __DEBUG__


////// TYPEDEF //////
union uint16_as_bytes
{
  uint16_t ival;
  uint8_t bval[2];
} uint16_2_bytes;


////// PINS DEFINITIONS //////
uint8_t LIGHT_SW = 2;
uint8_t RIGHT_BTN = 3;
uint8_t LEFT_BTN = 4; 
uint8_t STATUS_BTN = 5;

uint8_t BATTERY_PIN = A0;
uint8_t POTEN_PIN = A1;
uint8_t JX_PIN = A3;
uint8_t JY_PIN = A2;


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

////// OLED SCREEN DEFINITIONS //////
#define WIDTH         128 // width OLED
#define HEIGHT        64  // height OLED

Adafruit_SSD1306 display(WIDTH, HEIGHT, &Wire, -1); // object of Adafruit_SSD1306 class 


////// BATTERY VARIABLES //////
uint32_t control_battery_data = 0;
uint32_t control_battery_level = 0;

uint32_t car_battery_data = 0;
uint32_t car_battery_level = 0;


////// OTHER VARIABLES //////
uint8_t control_status = 0; // 1 or 0
uint8_t light_status = 0; // 1 or 0
String control_status_array[2] = {"STOP", "RUN"};
String light_status_array[2] = {"OFF", "ON"};

uint8_t connection_state = 0;
unsigned long last_msg_time = 0;


void setup() 
{
  #ifdef __DEBUG__
  Serial.begin(115200);
  #endif
  
  // IO configurations
  pinMode(LIGHT_SW, INPUT);
  pinMode(RIGHT_BTN, INPUT);
  pinMode(LEFT_BTN, INPUT);
  pinMode(STATUS_BTN, INPUT);
  
  // reset each channel value
  tx_data.ch1 = 0; // battery level
  tx_data.ch2 = 0; // joystick_x
  tx_data.ch3 = 0; // joystick_y
  tx_data.ch4 = 0; // potenciometer 
  tx_data.ch5 = 0; // status btn
  tx_data.ch6 = 0; // right btn
  tx_data.ch7 = 0; // left btn
  tx_data.ch8 = 0; // lights switch

  // configure NRF24 transmitter
  radio.begin();
  radio.setAutoAck(false);
  radio.setPALevel(RF24_PA_HIGH);
  radio.setDataRate(RF24_250KBPS);
  radio.openWritingPipe(tx_pipe);
  radio.openReadingPipe(1, rx_pipe);
  radio.stopListening(); // stablish device as transmitter

  // start the OLED screen
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) 
  {
    #ifdef __DEBUG__
    Serial.println("OLED screen not found");
    #endif
    
    while (true);
  }
  
  // print an initialization sequence on OLED screen
  display.clearDisplay();

  display.setTextColor(WHITE);
  display.setTextSize(3);
  display.setCursor(30,10);
  display.println("CAR");
  display.setCursor(30,40);
  display.println("V5.0");
  display.display();
  delay(1000);
    
  for (uint8_t y = 0; y < HEIGHT; y = y+4) 
  {
    display.drawFastHLine(0, y, WIDTH, WHITE);
    display.display();
  }

  for (uint8_t y = 0; y < HEIGHT; y = y+4)
  {
    display.drawFastHLine(0, y, WIDTH, BLACK);
    display.display();
  }

  for (uint8_t x = 0; x < WIDTH; x = x+4) 
  {
    display.drawFastVLine(x, 0, HEIGHT, WHITE);
    display.display();
  }
  
  for (uint8_t x = 0; x < WIDTH; x = x+4) 
  {
    display.drawFastVLine(x, 0, HEIGHT, BLACK);
    display.display();
  }

  display.clearDisplay(); // clear the buffer
  display.display(); // print the cleared buffer

  #ifdef __DEBUG__
  Serial.println("Hello World Control!");
  #endif
}


void loop()
{
  // read all data from control to send to car (if is reversed change 0,255 to 255,0)
  if (digitalRead(STATUS_BTN) == 0)
  {
    control_status = control_status ^ 1; // XOR 1 to toggle value
    delay(100); // debounce delay
  }
  else
  {
    // do nothing
  }

  light_status = digitalRead(LIGHT_SW);
  
  tx_data.ch1 = map( analogRead(BATTERY_PIN), 0, 1024, 0, 255 );
  tx_data.ch2 = map( analogRead(JX_PIN), 0, 1024, 255, 0 );
  tx_data.ch3 = map( analogRead(JY_PIN), 0, 1024, 255, 0 );
  tx_data.ch4 = map( analogRead(POTEN_PIN), 0, 1024, 0, 180 );
  tx_data.ch5 = control_status;
  tx_data.ch6 = digitalRead(RIGHT_BTN);
  tx_data.ch7 = digitalRead(LEFT_BTN);
  tx_data.ch8 = light_status;

  // send data to the car
  radio.stopListening(); // stablish device as transmitter
  radio.write(&tx_data, sizeof(TX_Data));

  // read battery level of control
  control_battery_data = map( analogRead(BATTERY_PIN), 0, 1024, 0, 5000 ); // 1024 value = 5000mV = 5.0V
  control_battery_level = control_battery_data/0.643091; // Vout = Vin*k, k = 0.643091, Vin(battery) = Vout(adc)/k (hardware voltage divider)

  // read battery level from car and check connection of car
  radio.startListening(); // stablish device as receiver
  delay(10);
  
  if (radio.available())
  {
    last_msg_time = millis(); // register the time to check the connection state 
    connection_state = 1; // to update the connection state
    
    radio.read(&rx_data, sizeof(rx_data));
    
    uint16_2_bytes.bval[1] = rx_data.ch1;
    uint16_2_bytes.bval[0] = rx_data.ch2;
    car_battery_data = uint16_2_bytes.ival;

    // car battery voltage conversion
    car_battery_level = car_battery_data/0.403092; // Vout = Vin*k, k = 0.643091, Vin(battery) = Vout(adc)/k (hardware voltage divider)  
  }
  else
  {
    if (millis() - last_msg_time > 1000) // 1s without new message from car
    {
      connection_state = 0; // to reset the connection state
    }
    else
    {
      // do nothing
    }
  }
  
  #ifdef __DEBUG__
  Serial.print(last_msg_time);
  Serial.print(" ; ");
  Serial.print(connection_state);
  Serial.print(" ; ");
  Serial.print(car_battery_data);
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

  // print the system information on the screen
  display.clearDisplay(); // clear the buffer
  
  display.setTextColor(WHITE);
  display.setTextSize(1);
  
  display.setCursor(0,0);
  display.print("C:");
  display.print(control_battery_level, DEC); 
  display.print("mV");
  
  display.setCursor(58,0);
  if (connection_state == 0) // no connection state
  {
    display.write(8);
  }
  else if ( (control_battery_level < 7600) || (car_battery_level < 11400) ) // critical battery voltage
  {
    display.write(15);
  }
  else // everything ok
  {
    display.write(9);
  }
  
  display.setCursor(74,0);
  display.print("A:");
  display.print(car_battery_level, DEC);
  display.print("mV");

  display.setCursor(5,12);
  display.write(30);
  display.setCursor(5,22);
  display.write(31);
  display.setCursor(15,17);
  display.print("JY:");
  display.print(tx_data.ch3, DEC); 

  display.setCursor(112,17);
  display.write(17);
  display.setCursor(122,17);
  display.write(16);
  display.setCursor(74,17);
  display.print("JX:");
  display.print(tx_data.ch2, DEC); 

  display.setTextSize(3);
  display.setCursor(15,30);
  if (tx_data.ch7 == 1)
  {
    display.setTextColor(BLACK, WHITE);
    display.write(27);
  }
  else
  {
    display.setTextColor(WHITE);
    display.write(27);
  }
  
  display.setTextSize(3);
  display.setCursor(99,30);
  if (tx_data.ch6 == 1)
  {
    display.setTextColor(BLACK, WHITE);
    display.write(26);
  }
  else
  {
    display.setTextColor(WHITE);
    display.write(26);
  }

  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(56,32);
  display.print("POT");
  display.setCursor(56,42);
  display.print(tx_data.ch4, DEC); 
  
  display.setCursor(0,57);
  display.print("MODE:");
  if (control_status == 0)
  {
    display.setTextColor(WHITE);
    display.print(control_status_array[control_status]);
  }
  else
  {
    display.setTextColor(BLACK, WHITE);
    display.print(control_status_array[control_status]);
  }
  
  display.setTextColor(WHITE);
  display.setCursor(74,57);
  display.print("LIGHT:");
  if (light_status == 1)
  {
    display.setTextColor(BLACK, WHITE);
    display.print(light_status_array[light_status]);
  }
  else
  {
    display.setTextColor(WHITE);
    display.print(light_status_array[light_status]);
  }

  display.display();
}
