#include <Arduino.h>

#define TX  18   // for the wave sensor
#define RX  11   // for the wave sensor

HardwareSerial WaveSerial(1); // UART 1


void setup() 
{
    Serial.begin(115200);
    WaveSerial.begin(115200, SERIAL_8N1, RX, TX);
    Serial.println("ESP32-S3 UART1 ready");

}

void loop() 
{
  while (WaveSerial.available()) 
  {
    int data = WaveSerial.read();
    Serial.write(data);
  }
 
}

