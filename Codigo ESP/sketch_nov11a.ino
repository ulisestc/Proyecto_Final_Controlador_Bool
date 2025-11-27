/*
  ESP32 Web Server - Lógica Booleana
  
  Conexiones (INPUT_PULLUP):
  - Botón A: Pin 15 -> GND
  - Botón B: Pin 4  -> GND
  - Botón C: Pin 17 -> GND
  
  Conexiones LEDs (220ohm -> GND):
  - Progreso: 13, 25, 21, 32, 23, 26
  - Estado: 27
*/

#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h> 

const char* ssid = "TC";
const char* password = "KY49w371";
// const char* ssid = "BUAP_Estudiantes";
// const char* password = "f85ac21de4";

// Pines de Botones
const int pinBotonA = 15;
const int pinBotonB = 4;
const int pinBotonC = 17;

// Pines de LEDs
const int led_pins_progress[] = {13, 25, 21, 32, 23, 26};
const int led_pin_status = 27;

WebServer server(80);

void handleStatus() {
  Serial.println("Petición recibida en /status");

  bool estadoA = (digitalRead(pinBotonA) == LOW);
  bool estadoB = (digitalRead(pinBotonB) == LOW);
  bool estadoC = (digitalRead(pinBotonC) == LOW);

  StaticJsonDocument<200> doc;
  doc["boton_a"] = estadoA;
  doc["boton_b"] = estadoB;
  doc["boton_c"] = estadoC;

  String jsonResponse;
  serializeJson(doc, jsonResponse);
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "application/json", jsonResponse);
}

void handleLED() {
  Serial.print("Petición recibida en /led - ");
  
  if (server.hasArg("pin") && server.hasArg("state")) {
    int pin = server.arg("pin").toInt();
    int state = server.arg("state").toInt();

    digitalWrite(pin, state); 

    Serial.print("Pin: "); Serial.print(pin);
    Serial.print(", Estado: "); Serial.println(state);

    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "text/plain", "OK");
    
  } else {
    Serial.println("Error: Faltan parametros 'pin' o 'state'");
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(400, "text/plain", "Error: Faltan parametros 'pin' o 'state'");
  }
}

void handleRoot() {
  Serial.println("Petición recibida en / (root)");
  String html = "<html><head><title>ESP32 Server</title></head><body>";
  html += "<h1>Servidor de Lógica Booleana</h1>";
  html += "<p>Endpoints activos:</p>";
  html += "<ul><li><code>/status</code> (para leer botones)</li>";
  html += "<li><code>/led?pin=X&state=Y</code> (para controlar LEDs)</li></ul>";
  html += "<p><a href='/status'>Probar /status ahora</a></p>";
  html += "</body></html>";
  
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "text/html", html);
}

void handleNotFound() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(404, "text/plain", "404: Not Found");
}

void setup() {
  Serial.begin(115200);

  pinMode(pinBotonA, INPUT_PULLUP);
  pinMode(pinBotonB, INPUT_PULLUP);
  pinMode(pinBotonC, INPUT_PULLUP);

  for (int pin : led_pins_progress) {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW); 
  }
  pinMode(led_pin_status, OUTPUT);
  digitalWrite(led_pin_status, LOW); 

  Serial.print("Conectando a ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi conectado!");
  Serial.print("Dirección IP: ");
  Serial.println(WiFi.localIP()); 

  server.on("/", handleRoot);
  server.on("/status", handleStatus);
  server.on("/led", handleLED); 
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("Servidor HTTP iniciado");
}

void loop() {
  server.handleClient();
}