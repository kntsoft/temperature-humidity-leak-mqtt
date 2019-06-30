#include <Arduino.h>
#include <SPI.h>
#include <DHT.h>
#include <Adafruit_Sensor.h>
#include <ArduinoJson.h>
#include "config.h"

// Inicializo variables
DHT dht(DHT_PIN, DHT22);
const int capacity = JSON_OBJECT_SIZE(3);
const int threshold = 100;


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

void setup() {
  pinMode(LED_ERROR, OUTPUT);
  Serial.begin(115200);
  dht.begin();
}

void loop() {
  StaticJsonDocument<capacity> doc;

  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  if(isnan(humidity) || isnan(temperature)){
    Serial.println("Failed to read from DHT sensor!");
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