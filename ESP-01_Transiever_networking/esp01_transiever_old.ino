#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SoftwareSerial.h>

#define MAX_SERIAL_BUFFER_SIZE 128 // Maximum size of the serial input buffer

// #define RX_PIN 3        // Connect RX pin to the transmitting device
// #define TX_PIN 1        // Connect TX pin to the receiving device
// #define BAUD_RATE 9600  // Baud rate for both SoftwareSerial and Serial

// SoftwareSerial mySerial(RX_PIN, TX_PIN);

unsigned long previousMillis = 0;
const long interval = 2000;  // Interval in milliseconds for sending message

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

const char *ssid = "UIU-CAIR";
const char *password = "12345678";

void notifyClients() {
  ws.textAll(String("ping"));
}


void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo *)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    const char *message = (char *)data;
    Serial.println(message);

    if (strncmp(message, "door:", 5) == 0) {
      char door[2];
      int state;

      if (sscanf(message + 5, "%1[^,],state:%d", door, &state) == 2) {
        if (strcmp(door, "1") == 0) {
          if (state == 1) {
            ws.textAll("info: door, key:1, val:1;");
          } else if (state == 0) {
            ws.textAll("info: door, key:1, val:0;");
          }
        } else if (strcmp(door, "2") == 0) {
          if (state == 1) {
            ws.textAll("info: door, key:2, val:1;");
          } else if (state == 0) {
            ws.textAll("info: door, key:2, val:0;");
          }
        } else if (strcmp(door, "3") == 0) {
          if (state == 1) {
            ws.textAll("info: door, key:3, val:1;");
          } else if (state == 0) {
            ws.textAll("info: door, key:3, val:0;");
          }
        }
      }
    } else if (strcmp(message, "closeAll") == 0) {
      ws.textAll(String("info: door, key:alldoor, val: 0;"));
    } else if (strcmp(message, "openAll") == 0) {
      ws.textAll(String("info: door, key:alldoor, val: 1;"));
    } else if (strcmp(message, "reset") == 0) {
      ws.textAll(String("info: reset, key:state, val: 1;"));
    }
  }
}


void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}
void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
  Serial.println("WebSocket Initiated!");
}



void setup() {
  Serial.begin(9600);
  Serial.setDebugOutput(true);

  // mySerial.begin(BAUD_RATE);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  initWebSocket();
  server.begin();
}



char serialBuffer[MAX_SERIAL_BUFFER_SIZE]; // Buffer to hold incoming serial data
int serialBufferIndex = 0; // Index to keep track of the position in the buffer


void loop() {
  // Check if there is data available to read from the Serial port
  while (Serial.available() > 0) {
    // Read the incoming byte
    char incomingByte = Serial.read();
    
    // Check if the incoming byte is a newline character (end of string)
    if (incomingByte == '\n') {
      // Null-terminate the buffer to make it a C-style string
      serialBuffer[serialBufferIndex] = '\0';
      
      // Send the string to all WebSocket clients
      ws.textAll(String(serialBuffer));
      
      // Reset the buffer index to read new data
      serialBufferIndex = 0;
    } else {
      // Add the incoming byte to the buffer
      serialBuffer[serialBufferIndex] = incomingByte;
      
      // Increment the buffer index
      serialBufferIndex++;
      
      // Check if the buffer is full, truncate if necessary
      if (serialBufferIndex >= MAX_SERIAL_BUFFER_SIZE - 1) {
        serialBufferIndex = MAX_SERIAL_BUFFER_SIZE - 1;
      }
    }
  }
  
  // Clean up disconnected WebSocket clients
  ws.cleanupClients();
}
