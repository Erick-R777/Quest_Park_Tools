//libraries
#include <Arduino.h>

// defines
#define Mic A0
#define sample_window 50 // Sample window width in mS (50 mS = 20Hz)
#define Uref 0.006 // Reference voltage 6mV

// prototypes
int get_Peak_To_Peak(int mic, int window);
int peak_to_voltage(int peak_to_peak);
float Voltage_to_DB(float voltage);
void print_values(float voltage, float dB);


void setup() 
{
  Serial.begin(9600);  
}

void loop() 
{
  int peak_to_peak = get_Peak_To_Peak(Mic, sample_window);
  float voltage = peak_to_voltage(peak_to_peak);
  float dB = Voltage_to_DB(voltage);

  print_values(voltage, dB);
 
  delay(500);
}

int get_Peak_To_Peak(int mic, int window)
{
  unsigned long startMillis = millis();
  int signalmax = 0;
  int signalMin = 1023;

  while (millis() - startMillis < window) 
  {
    int sample = analogRead(mic);
    if (sample < signalMin)    
      signalMin = sample;
    
    if (sample > signalmax) 
      signalmax = sample;
    
  }
  return signalmax - signalMin;
}


int peak_to_voltage(int peak_to_peak)
{
  return (peak_to_peak * 5) / 1023;
}

float Voltage_to_DB(float voltage)
{
  if (voltage == 0) return -100.0;  // Avoid log(0)
  return (20 * log10(voltage/ Uref));     // convert voltage to dB with the formula 20log(U1/Uref)
}

void print_values(float voltage, float dB)
{
  Serial.print("Voltage: ");
  Serial.print(voltage, 3);
  Serial.print(" V | ");
  Serial.print("Approx dB: ");
  Serial.print(dB, 1);
  Serial.println(" dB | ");
}