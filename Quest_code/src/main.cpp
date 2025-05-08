#include <Arduino.h>
#define RX 44  // Board RX (connected to sensor TX)
#define TX 43  // Board TX (connected to sensor RX)

HardwareSerial WaveSerial(1);

void setup() 
{
  Serial.begin(115200);
  delay(1000); // Give USB serial time to initialize
  Serial.println("Booting...");
  WaveSerial.begin(115200, SERIAL_8N1, RX, TX);
  Serial.println("Sensor UART ready");
}

void loop() 
{
  while (WaveSerial.available()) 
  {
    Serial.write(WaveSerial.read());
  }
}

