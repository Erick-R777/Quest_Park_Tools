#include <Arduino.h>
#include <SoftwareSerial.h>

#define RX 8
#define TX 9

SoftwareSerial WaveSerial(RX, TX);

// Instelbare afstandslimieten (in centimeter)
const int MIN_DISTANCE = 50;
const int MAX_DISTANCE = 200;

bool previousPresence = false;  // Vorige aanwezigheid
bool currentPresence = false;   // Huidige aanwezigheid
int presenceCount = 0;          // Totaal aantal personen

unsigned long lastPrintTime = 0;            // Tijdstip laatste statusprint
const unsigned long printInterval = 1000;   // 1 seconde tussen statusprints

// Functieprototypes
void checkSensorData();
void detectNewPerson();
void printStatus();

void setup() 
{
  Serial.begin(115200);
  WaveSerial.begin(115200);
  Serial.println("Sensor gestart...");
}

void loop() 
{
  checkSensorData();
  detectNewPerson();
  printStatus();
}

// Lees afstandsgegevens van de sensor
void checkSensorData() 
{
  if (WaveSerial.available()) 
  {
    String incoming = WaveSerial.readStringUntil('\n');
    incoming.trim(); // Verwijder eventuele lege spaties

    if (incoming.startsWith("Range")) // Afstandsgegevens
    {
      int distance = incoming.substring(6).toInt();

      if (distance >= MIN_DISTANCE && distance <= MAX_DISTANCE) 
      {
        currentPresence = true;
      } else 
      {
        currentPresence = false;
      }
    }
  }
}

// Telt alleen nieuwe personen bij overgang van niet aanwezig → aanwezig
void detectNewPerson() 
{
  if (currentPresence && !previousPresence) 
  {
    presenceCount++;
    Serial.print("Nieuwe persoon gedetecteerd! Totaal: ");
    Serial.println(presenceCount);
  }

  previousPresence = currentPresence;
}

// Print 1x per seconde de huidige status (afstand en aanwezigheid)
void printStatus() 
{
  static String lastStatus = ""; // Vorige status
  if (millis() - lastPrintTime > printInterval) 
  {
    lastPrintTime = millis();

    String status = currentPresence ? "Aanwezig binnen afstand" : "Geen aanwezigheid";
    if (status != lastStatus) 
    {
      Serial.println("Status: " + status);
      lastStatus = status;
    }
  }
}
