#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

Adafruit_BME280 bme;  // I2C interface

void setup() 
{
  Serial.begin(9600);
  
  // Initialize BME280 sensor
  if (!bme.begin(0x76)) 
  {  // Try 0x77 if 0x76 doesn't work
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);  // Stop here if sensor is not found
  }

  Serial.println("BME280 Sensor Initialized.");
}

void loop() 
{
  float temperature = readTemperature();
  float humidity = readHumidity();
  float pressure = readPressure();

  displayReadings(temperature, humidity, pressure);

  delay(2000);  // Update every 2 seconds
}

// Function to read temperature in Celsius
float readTemperature() 
{
  return bme.readTemperature();
}

// Function to read humidity in percentage
float readHumidity() 
{
  return bme.readHumidity();
}

// Function to read pressure in hPa
float readPressure() 
{
  return bme.readPressure() / 100.0F;  // Convert Pa to hPa
}

// Function to display all sensor readings
void displayReadings(float temp, float hum, float pres) 
{
  Serial.print("Temperature: ");
  Serial.print(temp);
  Serial.print(" °C | Humidity: ");
  Serial.print(hum);
  Serial.print(" % | Pressure: ");
  Serial.print(pres);
  Serial.println(" hPa");
}

