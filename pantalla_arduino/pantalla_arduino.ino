// A4 (SDA) and A5 (SCL) I2C interface of nano, 

#define __DEBUG__

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Definir constantes
#define ANCHO_PANTALLA 128 // ancho pantalla OLED
#define ALTO_PANTALLA 64 // alto pantalla OLED

// Objeto de la clase Adafruit_SSD1306
Adafruit_SSD1306 display(ANCHO_PANTALLA, ALTO_PANTALLA, &Wire, -1);

void setup() 
{
  
#ifdef __DEBUG__
  Serial.begin(9600);
  delay(100);
  Serial.println("Iniciando pantalla OLED");
#endif

  // Iniciar pantalla OLED en la dirección 0x3C
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) 
  {
#ifdef __DEBUG__
    Serial.println("No se encuentra la pantalla OLED");
#endif
    while (true);
  }
}

void loop() 
{
  // Recorrer pantalla
  for (int y = 0; y < ALTO_PANTALLA; y++) 
  {
    for (int x = 0; x < ANCHO_PANTALLA; x++) 
    {
      // Limpiar buffer
      display.clearDisplay();
      // Dibujar un píxel
      display.drawPixel(x, y, WHITE);
      // Enviar a pantalla
      display.display();
    }
  }
}
