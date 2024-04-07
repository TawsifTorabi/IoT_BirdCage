/*
Set Wifi SSID and Password Without Coding.
send this command on serial "setwifi ssid--YOUR_WIFI_SSID password-YOUR_WIFI_PASSWORD -end"
It'll set the wifi SSID and Password on EEPROM
After not getting connected to Wifi, the ESP will prompt on serial monitor for setting Wifi SSID and Password.
*/
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <EEPROM.h>

#define MAX_SERIAL_BUFFER_SIZE 128 // Maximum size of the serial input buffer

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

const int ssidLength = 32; // Max length for SSID
const int passLength = 64; // Max length for password

void initWebSocket() {
  ws.onEvent([](AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len) {
    // Handle WebSocket events here
  });

  server.addHandler(&ws);
  Serial.println("WebSocket Initiated!");
}

void setup() {
  Serial.begin(9600);
  //Serial.setDebugOutput(true);

  // Initialize EEPROM
  EEPROM.begin(512);

  // Read SSID and password from EEPROM
  char storedSSID[ssidLength + 1]; // +1 for null terminator
  char storedPass[passLength + 1]; // +1 for null terminator

  for (int i = 0; i < ssidLength; i++) {
    storedSSID[i] = EEPROM.read(i);
  }
  storedSSID[ssidLength] = '\0'; // Null-terminate the string

  for (int i = 0; i < passLength; i++) {
    storedPass[i] = EEPROM.read(ssidLength + i);
  }
  storedPass[passLength] = '\0'; // Null-terminate the string

  // Check if SSID and password are stored in EEPROM
  if (strlen(storedSSID) == 0 || strlen(storedPass) == 0) {
    // SSID and password not found in EEPROM, wait for serial input
    Serial.println("Please enter SSID and password (format: setwifi ssid--SSID_NAME password-WIFI_PASSWORD-end):");
    while (!Serial.available()) {
      delay(100);
    }

    // Read input from serial
    String input = Serial.readStringUntil('\n');
    input.trim(); // Remove leading/trailing whitespace

    // Extract SSID and password from input
    String ssidInput = getValue(input, "ssid--", " password-");
    String passInput = getValue(input, " password-", " -end");

    // Copy SSID and password to char arrays
    ssidInput.toCharArray(storedSSID, ssidLength + 1);
    passInput.toCharArray(storedPass, passLength + 1);

    // Store SSID and password in EEPROM
    for (int i = 0; i < ssidLength; i++) {
      EEPROM.write(i, storedSSID[i]);
    }
    for (int i = 0; i < passLength; i++) {
      EEPROM.write(ssidLength + i, storedPass[i]);
    }
    EEPROM.commit();
  }
  Serial.print("ssid: ");
  Serial.println(storedSSID);
  Serial.print("pass: ");
  Serial.println(storedPass);

  // Connect to WiFi using stored SSID and password
  WiFi.begin(storedSSID, storedPass);

  Serial.println("Connecting to WiFi...");
  
  // Initialize connection attempts counter
  int connectionAttempts = 0;
  
  // Keep attempting to connect to WiFi until successful or maximum attempts reached
  while (WiFi.status() != WL_CONNECTED && connectionAttempts < 5) {
    delay(1000); // Wait for 1 second before next attempt
    Serial.print(".");
    connectionAttempts++;
  }

  // If WiFi connection successful
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.println("Connected to WiFi");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    // Initialize WebSocket and server
    server.begin();
    initWebSocket();
  } else {
    // Maximum connection attempts reached, prompt user to input SSID and password via serial
    Serial.println();
    Serial.println("Maximum connection attempts reached. Please enter SSID and password:");
  }
}



char serialBuffer[MAX_SERIAL_BUFFER_SIZE]; // Buffer to hold incoming serial data
int serialBufferIndex = 0; // Index to keep track of the position in the buffer

void loop() {
  static int connectionAttempts = 0;
  
  // Handle OTA updates
  //ArduinoOTA.handle();

  // Handle WebSocket events
  // Add your WebSocket event handling code here

  // Handle server events
  // Add your server event handling code here

  // Check if there is data available to read from the Serial port
  while (Serial.available() > 0) {
    // Read the incoming byte
    char incomingByte = Serial.read();
    
    // Check if the incoming byte is a newline character (end of string)
    if (incomingByte == '\n') {
      // Null-terminate the buffer to make it a C-style string
      serialBuffer[serialBufferIndex] = '\0';
      
      // Process the serial input
      processSerialInput();
      
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
  
  // If not connected to WiFi and reached maximum connection attempts
  if (WiFi.status() != WL_CONNECTED && connectionAttempts >= 5) {
    // Prompt user to input SSID and password via serial
    Serial.println("Maximum connection attempts reached. Please enter SSID and password:");
    // Reset connection attempts counter
    connectionAttempts = 0;
  }
  
  // If not connected to WiFi and maximum attempts not reached
  if (WiFi.status() != WL_CONNECTED && connectionAttempts < 5) {
    // Increment connection attempts counter
    connectionAttempts++;
    // Attempt to reconnect to WiFi
    WiFi.begin();
    delay(5000); // Wait for 5 seconds before retrying
  }
  
  // If connected to WiFi
  if (WiFi.status() == WL_CONNECTED) {
    // Reset connection attempts counter
    connectionAttempts = 0;
    
    // Clean up disconnected WebSocket clients
    ws.cleanupClients();
  }
}

void processSerialInput() {
  // Convert serialBuffer to String
  String serialInput = String(serialBuffer);
  serialInput.trim(); // Remove leading/trailing whitespace

  if (serialInput.startsWith("setwifi")) {
    // Extract SSID and password from serial input
    String ssidInput = getValue(serialInput, "ssid--", " password-");
    String passInput = getValue(serialInput, " password-", " -end");

    // Copy SSID and password to char arrays
    char ssidBuffer[ssidLength + 1];
    char passBuffer[passLength + 1];
    ssidInput.toCharArray(ssidBuffer, ssidLength + 1);
    passInput.toCharArray(passBuffer, passLength + 1);

    // Store SSID and password in EEPROM
    for (int i = 0; i < ssidLength; i++) {
      EEPROM.write(i, ssidBuffer[i]);
    }
    for (int i = 0; i < passLength; i++) {
      EEPROM.write(ssidLength + i, passBuffer[i]);
    }
    EEPROM.commit();

    // Restart ESP8266 to apply changes
    Serial.println("ESP8266 Restarting...");
    ESP.restart();
  }

  // Send the string to all WebSocket clients
  ws.textAll(String(serialBuffer));  

}

String getValue(String data, String separator, String terminator) {
  int separatorIndex = data.indexOf(separator);
  if (separatorIndex != -1) {
    int startIndex = separatorIndex + separator.length();
    int terminatorIndex = data.indexOf(terminator, startIndex);
    if (terminatorIndex != -1) {
      return data.substring(startIndex, terminatorIndex);
    }
  }
  return "";
}


