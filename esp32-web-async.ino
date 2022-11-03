#include <Arduino.h>
#include <SPIFFS.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "heltec.h"

#define ECHO_PIN 12
#define TRIG_PIN 13

const char *ssid = "DJ WiFi";
const char *password = "secrets!";

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

void setLED(AsyncWebServerRequest *request) {
  if (request->hasParam("led", true)) {
    int led = request->getParam("led", true)->value().toInt();
    digitalWrite(25, led);
    request->send(200, "text/plain", "OK");
  } else {
    request->send(400, "text/plain", "Missing led param");
  }
}

void printToScreen(String s) {
  Heltec.display->clear();
  Heltec.display->drawString(0, 0, s);
  Heltec.display->display();
}

void setup() {
  Heltec.begin(true /*display*/, false /*LoRa*/, true /*Serial*/);
  printToScreen("READY");

  WiFi.softAP(ssid, password);
  printToScreen("My IP: " + WiFi.softAPIP().toString());

  if (!SPIFFS.begin()) {
    printToScreen("SPIFFS failed.");
    while (true);
  }

  server.serveStatic("/", SPIFFS, "/www/").setDefaultFile("index.html");
  server.onNotFound(notFound);
  server.on("/led", HTTP_POST, setLED);
  server.addHandler(&ws);
  server.begin();

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
}

void loop() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH);
  int distance = duration * 0.034 / 2; // speed of sound divided by 2 (half of round trip time)

  String message = "Distance: " + String(distance) + " cm";
  printToScreen(message);
  ws.textAll(message.c_str());
  delay(1000);
}
