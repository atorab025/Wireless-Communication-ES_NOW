#include <WiFi.h> 

void setup() {
  Serial.begin(115200);
  delay(1000); 
  Serial.println("Fetching MAC Address of ESP32...");
  String macAddress = WiFi.macAddress();
  Serial.println("MAC Address: " + macAddress);
}

void loop() {
}
