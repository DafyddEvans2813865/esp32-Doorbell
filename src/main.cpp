#include <Arduino.h>
#include "camera.h"

const int radarPin = 38;
const int ledPin = 39;

void setup()
{
  Serial.begin(115200);
  pinMode(radarPin, INPUT);
  pinMode(ledPin, OUTPUT);
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

      camera_fb_t *fb = getFrame();
      Serial.printf("Frame OK size=%d bytes pinmap=%s\n", fb->len);
      esp_camera_fb_return(fb);
    }
  }
  else
  {
    digitalWrite(ledPin, LOW);
    Serial.print("No Motion Detected\n");
  }
  delay(1000);
}
