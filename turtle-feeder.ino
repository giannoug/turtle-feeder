#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <Servo.h>

#define SERVO_MIN 30
#define SERVO_MAX 180

const char *ssid     = "YOU_THOUGHT";
const char *password = "I_COMMITTED_THIS?";

ESP8266WebServer server(80);

Servo feeder;

byte servo_angle = SERVO_MIN;

void move_feeder() {
  feeder.write(servo_angle);
  delay(15);
}

void sweep_feeder() {
  byte pos;

  Serial.println("[sweep_feeder] Starting.");

  Serial.println("[sweep_feeder] Going from 0 to 180.");

  for (pos = SERVO_MIN; pos <= SERVO_MAX; pos++) {
    Serial.print("[sweep_feeder] pos: ");
    Serial.println(pos);
    servo_angle = pos;
    move_feeder();
  }

  Serial.println("[sweep_feeder] Going back now.");

  for (pos = SERVO_MAX; pos >= SERVO_MIN; pos--) {
    Serial.print("[sweep_feeder] pos: ");
    Serial.println(pos);
    servo_angle = pos;
    move_feeder();
  }

  Serial.println("[sweep_feeder] Ended.");
}

void handle404() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += server.method() == HTTP_GET ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }

  server.send(404, "text/plain", message);
}

void handleRoot() {
  char temp[400];

  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;

  snprintf(temp, 400,
"<html>\
  <head>\
    <title>Turtle feeder v1.0</title>\
  </head>\
  <body>\
    <h1>This is turtle feeder v1.0!</h1>\
    <p>Uptime: %02d:%02d:%02d</p>\
  </body>\
</html>",
  hr, min % 60, sec % 60
);

  server.send(200, "text/html", temp);
}

void handleFeederMove() {
  String json = "{";

  if (server.method() == HTTP_GET) {
    json += "\"value\": \"";
    json += servo_angle;
    json += "\"";
  }
  else {
    String value = server.arg(0); // Assume first argument is the value for the servo, please

    Serial.print("Got servo value: ");
    Serial.println(value);

    servo_angle = value.toInt();
    move_feeder();

    json += "\"value\": \"";
    json += servo_angle;
    json += "\"";
  }

  json += "}";

  server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
  server.send(200, "application/json", json);
}

void handleFeederSweep() {
  String json = "{";
  json += "\"status\": \"done\"";
  json += "}";

  sweep_feeder();

  server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
  server.send(200, "application/json", json);
}

void handleStatus() {
  String json = "{";

  json += "\"esp\":{"; // Start the "ESP" object
  json += "\"free_heap\":\"";
  json += ESP.getFreeHeap();
  json += "\",";
  json += "\"chip_id\":\"";
  json += ESP.getChipId();
  json += "\",";
  json += "\"flash_chip_id\":\"";
  json += ESP.getFlashChipId();
  json += "\",";
  json += "\"flash_chip_size\":\"";
  json += ESP.getFlashChipSize();
  json += "\",";
  json += "\"flash_chip_speed\":\"";
  json += ESP.getFlashChipSpeed();
  json += "\",";
  json += "\"cycle_count\":\"";
  json += ESP.getCycleCount();
  json += "\",";
  json += "\"vcc\":\"";
  json += ESP.getVcc();
  json += "\"";
  json += "},"; // End the "ESP" object

  json += "\"feeder\":{"; // Start the "feeder" object
  json += "\"value\":\"";
  json += servo_angle;
  json += "\"";
  json += "}"; // End the "feeder" object

  json += "}";

  server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
  server.send(200, "application/json", json);
}

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);

  // Wait for WiFi connection
  Serial.print("Attempting connection to WiFi...");

  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    Serial.print(".");
  }

  Serial.println(" Done!");

  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.onNotFound(handle404);
  server.on("/", handleRoot);
  server.on("/api/feeder/move", handleFeederMove);
  server.on("/api/feeder/sweep", handleFeederSweep);
  server.on("/api/status", handleStatus);

  // Wow
  //server.on("/inline", []() {
  //  server.send(200, "text/plain", "this works as well");
  //});

  server.begin();

  feeder.attach(2);
  move_feeder();

  Serial.println("Turtle feeder has server started.");
}

void loop() {
  server.handleClient();
}
