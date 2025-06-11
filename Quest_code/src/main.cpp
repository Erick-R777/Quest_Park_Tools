#include <Arduino.h>
#include <Wire.h>
#include <BH1750.h>

BH1750 lightSensor;

void setup() 
{
  Serial.begin(115200);
  Wire.begin();

  if (lightSensor.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) 
  {
    Serial.println("BH1750 gestart...");
  } else 
  {
    Serial.println("Kon BH1750 niet starten. Controleer de bekabeling.");
    while (1);
  }
}

void loop() 
{
  float lux = readLightLevel();
  printLightCategory(lux);
  delay(1000);  // Wacht 1 seconde tussen metingen
}

// Leest lux-waarde van sensor
float readLightLevel() 
{
  float lux = lightSensor.readLightLevel();
  Serial.print("Lichtintensiteit: ");
  Serial.print(lux);
  Serial.println(" lux");
  return lux;
}

// Categoriseert de lichtsterkte
void printLightCategory(float lux) 
{
  String category;

  if (lux < 50) 
  {
    category = " Donker";
  } else if (lux < 1000) {

    category = " Normaal licht";
  } else 
  {
    category = " Fel licht";
  }

  Serial.println("Lichtcategorie: " + category);
}
