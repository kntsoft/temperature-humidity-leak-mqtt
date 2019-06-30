#include <Arduino.h>
#include <Ethernet.h>
#include <SPI.h>
#include <DHT.h>
#include <Adafruit_Sensor.h>
#include <ArduinoJson.h>
#include "config.h"

// Inicializo variables
DHT dht(DHT_PIN, DHT22);
const int capacity = JSON_OBJECT_SIZE(3);
const int threshold = 100;
EthernetClient client;

// Network configuration
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };


boolean readWaterSensor(){
  int confirmations = 1;
  int requiredConfirmations = 10;
  while (confirmations <= requiredConfirmations)
  {
    delay(100);
    if(analogRead(WATERDETECTION_PIN) >= threshold){
      confirmations++;
    }else{
      return false;
    }
  }
  return true;
}

void printNetworkInformation(){
  Serial.print("IP address . . . . . . : ");
  Serial.println(Ethernet.localIP());
  Serial.print("Subnet mask. . . . . . : ");
  Serial.println(Ethernet.subnetMask());
  Serial.print("Gateway ip address . . : ");
  Serial.println(Ethernet.gatewayIP());
  Serial.print("Dns server . . . . . . : ");
  Serial.println(Ethernet.dnsServerIP());
}

void printNetworkError(){
  if(Ethernet.hardwareStatus() == EthernetNoHardware){
    Serial.println("Ethernet shield was not found.");
  }else{
    switch (Ethernet.linkStatus())
    {
    case LinkON:
      Serial.println("Link status: On");
      break;
    case LinkOFF:
      Serial.println("Link status: Off");
      break;
    default:
      Serial.println("Link status unknown. Link status detection is only available with W5200 and W5500.");
      break;
    }
  }
}

void errorLed(boolean turn){
  if(turn){
    digitalWrite(LED_ERROR, HIGH);
  }else{
    digitalWrite(LED_ERROR, LOW);
  }
} 

void setup() {
  pinMode(LED_ERROR, OUTPUT);
  dht.begin();
  // Start
  Serial.begin(115200);
  Serial.print("Initialize Ethernet with DHCP: ");
  if(Ethernet.begin(mac) == 0){
    Serial.println("Failed to configure Ethernet using DHCP");
    errorLed(true);
    printNetworkError();
  }else{
    Serial.println("Connected");
    printNetworkInformation();
  }
}

void loop() {
  StaticJsonDocument<capacity> doc;

  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  if(isnan(humidity) || isnan(temperature)){
    Serial.println("Failed to read from DHT sensor!");
    errorLed(true);
  }else{
    doc["temperature"] = dht.readHumidity();
    doc["humidity"] = dht.readTemperature();
  }
  doc["leak"] = (readWaterSensor()) ? 1 : 0;
  if(!doc.isNull()){
    serializeJsonPretty(doc,Serial);
    Serial.println();
  }
  delay(ACCURACY);
}