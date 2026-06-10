#include <Arduino.h>
#include <WiFi.h>
#include <WiFiManager.h>
#include "camera.h"

const int radarPin = 38;
const int ledPin = 39;

void initWifi()
{
  WiFiManager wm;
  bool res;
  // wm.resetSettings(); <-- reset saved wifi
  res = wm.autoConnect("Doorbell-Setup");
  if (!res)
  {
    Serial.println("Failed to connect");
  }
  else
  {
    Serial.println("WiFi connected!");
    Serial.println(WiFi.localIP());
  }
}

void setup()
{
  Serial.begin(115200);
  delay(5000); // just to slow debug

  Serial.println("BOOT START");
  pinMode(radarPin, INPUT);
  pinMode(ledPin, OUTPUT);

  initWifi();
  initCamera();
  delay(2000);
}

void loop()
{

  if (digitalRead(radarPin) == HIGH)
  {
    Serial.print("Motion Detected: ");
    if (doesCameraWork())
    {
      digitalWrite(ledPin, HIGH);

      sendCameraFrame();
    }
  }
  else
  {
    digitalWrite(ledPin, LOW);
  }
  delay(1000);
}
