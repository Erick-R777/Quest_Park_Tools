#include <Arduino.h>
#include <SoftwareSerial.h>

#define RX 8
#define TX 9

SoftwareSerial WaveSerial(RX, TX);

// Presence tracking
bool previousPresence = false;
bool currentPresence = false;
unsigned long lastDetectionTime = 0;
int presenceCount = 0;


void setup() 
{
  Serial.begin(115200);
  WaveSerial.begin(115200);
  Serial.println("Starting presence counter...");
}


void loop() 
{
  checkSensorData();
  updatePresenceState();
  detectNewPerson();
}

// ---- Function to read sensor data ----
void checkSensorData() 
{
  if (WaveSerial.available()) 
  {
    // Read the data (we don’t care what it is)
    WaveSerial.read();

    // Presence is assumed when data is received
    currentPresence = true;
    lastDetectionTime = millis();
  }
}

// ---- Function to update presence status based on time ----
void updatePresenceState() 
{
  if (millis() - lastDetectionTime > 500) 
  {
    currentPresence = false;
  }
}

// ---- Function to detect new person (rising edge) ----
void detectNewPerson() 
{
  if (currentPresence && !previousPresence) 
  {
    presenceCount++;
    Serial.print("New person detected! Total count: ");
    Serial.println(presenceCount);
  }

  // Update previous state for next loop
  previousPresence = currentPresence;
}





