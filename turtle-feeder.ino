#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <Servo.h>

const char *ssid     = "YOU_THOUGHT";
const char *password = "I_COMMITTED_THIS?";

ESP8266WebServer server(80);

Servo feeder;

int servo_angle = 0;
int servo_angle_new = 0;

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

void handleFeeder() {
  String json = "{";

  if (server.method() == HTTP_GET) {
    json += "\"message\": \"sup dude?\",";
    json += "\"value\": \"";
    json += servo_angle;
    json += "\"";
  }
  else {
    String value = server.arg(0); // Assume first argument is the value for the servo, please

    Serial.print("Got servo value: ");
    Serial.println(value);
    
    servo_angle_new = value.toInt();

    json += "\"value\": \"";
    json += servo_angle_new;
    json += "\"";
  }

  json += "}";

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
  server.on("/feeder", handleFeeder);

  // Wow
  //server.on("/inline", []() {
  //  server.send(200, "text/plain", "this works as well");
  //});

  server.begin();

  feeder.attach(2);

  Serial.println("Turtle feeder has server started.");
}

void loop() {
  if (servo_angle != servo_angle_new) {    
    servo_angle = servo_angle_new;
    feeder.write(servo_angle); // This will be throttled
    delay(10); // There.

    Serial.print("Updated servo value: ");
    Serial.println(servo_angle);
  }

  server.handleClient();
}
