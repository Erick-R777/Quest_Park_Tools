#include <Arduino.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <BH1750.h>

// ---------- mmWave Sensor Config ----------
#define RX 8
#define TX 9
SoftwareSerial WaveSerial(RX, TX); 
const int MIN_DISTANCE = 50;
const int MAX_DISTANCE = 200;
bool previousPresence = false;
bool currentPresence = false;
int presenceCount = 0;

// ---------- BME280 Config ----------
Adafruit_BME280 bme;

// ---------- Sound Sensor Config ----------
#define Mic A0
#define sample_window 50
#define Uref 0.006

// ---------- Light Sensor Config ----------
BH1750 lightSensor;

// ---------- Timing ----------
unsigned long lastSensorRead = 0;
const unsigned long sensorInterval = 2000;

// ---------- Function Prototypes ----------
void checkPresence();
void detectNewPerson();
void displayPresenceStatus();
float readTemperature();
float readHumidity();
float readPressure();
void displayBMEReadings(float t, float h, float p);
int get_Peak_To_Peak(int mic, int window);
int peak_to_voltage(int peak_to_peak);
float Voltage_to_DB(float voltage);
void displaySoundInfo(float voltage, float dB);
float readLightLevel();
void displayLightCategory(float lux);

void setup() 
{
  Serial.begin(115200);
  Wire.begin();
  WaveSerial.begin(115200);

  // Init BME280
  if (!bme.begin(0x76)) 
  {
    Serial.println("BME280 niet gevonden!");
    while (1);
  }

  // Init BH1750
  if (!lightSensor.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) 
  {
    Serial.println("Kon BH1750 niet starten!");
    while (1);
  }

  Serial.println("Alle sensoren succesvol gestart...");
}

void loop() 
{
  checkPresence();
  detectNewPerson();
  displayPresenceStatus();

  if (millis() - lastSensorRead > sensorInterval) // Read sensors every 2 seconds
  {
    lastSensorRead = millis();

    float t = readTemperature();
    float h = readHumidity();
    float p = readPressure();
    displayBMEReadings(t, h, p);

    int pk = get_Peak_To_Peak(Mic, sample_window);
    float v = peak_to_voltage(pk);
    float dB = Voltage_to_DB(v);
    displaySoundInfo(v, dB);

    float lux = readLightLevel();
    displayLightCategory(lux);
  }
}

// ---------- Presence Functions ----------
/**
 * @brief Controleer of er een persoon aanwezig is
 * Lees de afstand uit van de mmWave sensor en controleer of deze
 * binnen de afstandslimieten valt.
 */
void checkPresence() 
{
  if (WaveSerial.available()) 
  {
    String incoming = WaveSerial.readStringUntil('\n');
    incoming.trim();
    if (incoming.startsWith("Range")) 
    {
      int distance = incoming.substring(6).toInt();
      currentPresence = (distance >= MIN_DISTANCE && distance <= MAX_DISTANCE);
    }
  }
}

/**
 * @brief Detecteer een nieuwe persoon
 * 
 * Als de huidige Presence-sensorstatus anders is dan de vorige, 
 * wordt de presenceCount verhoogd en wordt een bericht naar de 
 * seri le console gestuurd.
 */
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

/**
 * @brief Toon de huidige status van de Presence-sensor
 * 
 * Geeft een bericht naar de seri le console met de huidige
 * status van de Presence-sensor. Als de status veranderd is 
 * ten opzichte van de vorige keer, wordt er een nieuw bericht
 * gestuurd.
 */
void displayPresenceStatus() 
{
  static String lastStatus = "";
  String status = currentPresence ? "Aanwezig binnen afstand" : "Geen aanwezigheid";
  if (status != lastStatus) 
  {
    Serial.println("Status: " + status);
    lastStatus = status;
  }
}

// ---------- BME280 Functions ----------
float readTemperature() 
{ 
  return bme.readTemperature(); 
}
float readHumidity()    
{ 
  return bme.readHumidity(); 
}
float readPressure()    
{ 
  return bme.readPressure() / 100.0F; 
}
/**
 * @brief Toon de huidige BME280-waarden op de seri le console
 * 
 * Geeft de huidige temperatuur, luchtvochtigheid en druk op de
 * seri le console. De waarden worden omgezet naar de juiste
 * eenheden en getoond in een leesbaar formaat.
 * 
 * @param t huidige temperatuur in graden Celsius
 * @param h huidige luchtvochtigheid in procenten
 * @param p huidige druk in hectopascal
 */
void displayBMEReadings(float t, float h, float p) 
{
  Serial.print("Temp: "); Serial.print(t); Serial.print(" °C | ");
  Serial.print("Luchtvocht: "); Serial.print(h); Serial.print(" % | ");
  Serial.print("Druk: "); Serial.print(p); Serial.println(" hPa");
}

// ---------- Sound Functions ----------
/**
 * @brief Lees de piek-tot-piek amplitude van een signaal van een microfoon
 * 
 * Lees de analoge waarde van de microfoon en bereken de piek-tot-piek amplitude
 * van het signaal over een bepaalde periode.
 * 
 * @param mic de pin van de microfoon
 * @param window de tijd (in ms) over welke de amplitude wordt berekend
 * @return de piek-tot-piek amplitude
 */
int get_Peak_To_Peak(int mic, int window) 
{
  unsigned long start = millis();
  int maxV = 0, minV = 1023;
  while (millis() - start < window) 
  {
    int val = analogRead(mic);
    if (val > maxV) maxV = val;
    if (val < minV) minV = val;
  }
  return maxV - minV;
}

/**
 * @brief Bereken de spanning van een piek-tot-piek amplitude
 * 
 * Bereken de spanning van een piek-tot-piek amplitude uit een
 * analoge waarde van een microfoon.
 * 
 * @param pk de piek-tot-piek amplitude uit een analoge waarde van een microfoon
 * @return de spanning in Volt
 */
int peak_to_voltage(int pk) 
{
  return (pk * 5) / 1023;
}

/**
 * @brief Bereken het geluidsniveau in decibel uit een spanning
 *
 * Bereken het geluidsniveau in decibel uit een spanning, uitgaande van
 * een referentiespanning van 6 mV.
 *
 * @param voltage de spanning in Volt
 * @return het geluidsniveau in decibel
 */
float Voltage_to_DB(float voltage) 
{
  if (voltage == 0) return -100.0;
  return (20 * log10(voltage / Uref));
}

/**
 * @brief Toon het geluidsniveau op de seri le console
 * 
 * Geeft het geluidsniveau op de seri le console. Het niveau wordt
 * weergegeven in decibel (dB) en een omschrijving van het niveau
 * (Stil, Normaal of Luid).
 * 
 * @param voltage de spanning in Volt
 * @param dB het geluidsniveau in decibel
 */
void displaySoundInfo(float voltage, float dB) 
{
  Serial.print("Geluidsniveau: ");
  Serial.print(dB, 1); Serial.print(" dB (");
  if (dB < 40) Serial.println("Stil)");
  else if (dB < 70) Serial.println("Normaal)");
  else Serial.println("Luid)");
}

// ---------- Light Sensor ----------
float readLightLevel() 
{
  return lightSensor.readLightLevel();
}

/**
 * @brief Toon de categorie van het lichtniveau op de seri le console
 * 
 * Geeft de categorie van het lichtniveau op de seri le console. De categorie
 * wordt bepaald door de waarde van het lichtniveau in lux.
 * 
 * @param lux de lichtsterkte in lux
 */
void displayLightCategory(float lux) 
{
  Serial.print("Licht: "); Serial.print(lux); Serial.print(" lux => ");
  if (lux < 50) Serial.println("Donker");
  else if (lux < 1000) Serial.println("Normaal licht");
  else Serial.println("Fel licht");
}
