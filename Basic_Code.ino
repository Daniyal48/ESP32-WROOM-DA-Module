#include <WiFi.h>
#include <HTTPClient.h>

// Definitions
#define CONNECTION_TIMEOUT 10

// Server definitions
WiFiServer httpServer(80);
WiFiServer telnetServer(23);
WiFiServer mqttServer(1883);
WiFiServer coapServer(5683);
WiFiServer upnpServer(1900);

// Mutex for Serial
SemaphoreHandle_t serialMutex;

// Flags to manage service states
bool httpActive = true;
bool telnetActive = true;
bool mqttActive = true;
bool coapActive = true;
bool upnpActive = true;

// Server Initialization
void setupServers() {
    httpServer.begin();
    telnetServer.begin();
    mqttServer.begin();
    coapServer.begin();
    upnpServer.begin();
    Serial.println("Servers are initialized.");
}

// HTTP Web Server
void handleHTTP(void *param) {
    while (true) {
        if (httpActive) {
            WiFiClient client = httpServer.available();
            if (client) {
                xSemaphoreTake(serialMutex, portMAX_DELAY);
                Serial.println("HTTP Connection detected");
                xSemaphoreGive(serialMutex);
                client.println("HTTP/1.1 200 OK");
                client.println("Content-Type: text/html");
                client.println();
                client.println("<h1> HTTP Service Active </h1>");
                client.stop();
            }
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

// Telnet Server
void handleTelnet(void *param) {
    while (true) {
        if (telnetActive) {
            WiFiClient client = telnetServer.available();
            if (client) {
                xSemaphoreTake(serialMutex, portMAX_DELAY);
                Serial.println("Telnet Connection detected");
                xSemaphoreGive(serialMutex);
                client.println("Welcome to the Telnet Service!");
                client.stop();
            }
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

// MQTT Server
void handleMQTT(void *param) {
    while (true) {
        if (mqttActive) {
            WiFiClient client = mqttServer.available();
            if (client) {
                xSemaphoreTake(serialMutex, portMAX_DELAY);
                Serial.println("MQTT Connection detected");
                xSemaphoreGive(serialMutex);
                client.println("MQTT/3.1 200 OK");
                client.println("Connected to Mock MQTT Broker");
                client.stop();
            }
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

// CoAP Server
void handleCoAP(void *param) {
    while (true) {
        if (coapActive) {
            WiFiClient client = coapServer.available();
            if (client) {
                xSemaphoreTake(serialMutex, portMAX_DELAY);
                Serial.println("CoAP Connection detected");
                xSemaphoreGive(serialMutex);
                client.println("2.05 Content");
                client.println("CoAP Service Active");
                client.stop();
            }
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

// UPnP Server
void handleUPnP(void *param) {
    while (true) {
        if (upnpActive) {
            WiFiClient client = upnpServer.available();
            if (client) {
                xSemaphoreTake(serialMutex, portMAX_DELAY);
                Serial.println("UPnP Connection detected");
                xSemaphoreGive(serialMutex);
                client.println("HTTP/1.1 200 OK");
                client.println("Content-Type: text/xml");
                client.println();
                client.println("<device><type>Mock IoT Device</type></device>");
                client.stop();
            }
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

// Shell Command Handler
void handleCommand(const String &command) {
    if (command.equalsIgnoreCase("network-info")) {
        Serial.printf("Network Information:\nSSID: %s\nIP: %s\n", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());
    } else if (command.equalsIgnoreCase("list-services")) {
        Serial.println("Active Services:");
        if (httpActive) Serial.println("- HTTP");
        if (telnetActive) Serial.println("- Telnet");
        if (mqttActive) Serial.println("- MQTT");
        if (coapActive) Serial.println("- CoAP");
        if (upnpActive) Serial.println("- UPnP");
    } else if (command.startsWith("toggle-service")) {
        String service = command.substring(command.indexOf(" ") + 1);
        if (service.equalsIgnoreCase("HTTP")) httpActive = !httpActive;
        else if (service.equalsIgnoreCase("Telnet")) telnetActive = !telnetActive;
        else if (service.equalsIgnoreCase("MQTT")) mqttActive = !mqttActive;
        else if (service.equalsIgnoreCase("CoAP")) coapActive = !coapActive;
        else if (service.equalsIgnoreCase("UPnP")) upnpActive = !upnpActive;
        else Serial.println("Invalid Service Name");
        Serial.printf("%s Service Toggled\n", service.c_str());
    } else if (command.equalsIgnoreCase("help")) {
        Serial.println("Available Commands:");
        Serial.println("- network-info: Show network details");
        Serial.println("- list-services: List active services");
        Serial.println("- toggle-service [service]: Enable/Disable a service");
        Serial.println("- help: Show this help message");
        Serial.println("- exit: End shell session");
    } else if (command.equalsIgnoreCase("exit")) {
        Serial.println("Ending Shell Session...");
        ESP.restart();
    } else {
        Serial.println("Unknown Command. Type 'help' for available commands.");
    }
}

// WiFi Setup
const char *ssid = "<YOUR_WIFI_SSID>";
const char *key = "<YOUR_WIFI_PASSWORD>";

void setupWiFi() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, key);
    Serial.println("\nConnecting...");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConnected!");
    Serial.printf("IP Address: %s\n", WiFi.localIP().toString().c_str());
}

// Setup Function
void setup() {
    Serial.begin(115200);
    delay(1000);

    // Initialize Mutex
    serialMutex = xSemaphoreCreateMutex();

    // Setup WiFi and Servers
    setupWiFi();
    setupServers();

    // Start Server Tasks
    xTaskCreate(handleHTTP, "HTTP Task", 2048, NULL, 1, NULL);
    xTaskCreate(handleTelnet, "Telnet Task", 2048, NULL, 1, NULL);
    xTaskCreate(handleMQTT, "MQTT Task", 2048, NULL, 1, NULL);
    xTaskCreate(handleCoAP, "CoAP Task", 2048, NULL, 1, NULL);
    xTaskCreate(handleUPnP, "UPnP Task", 2048, NULL, 1, NULL);

    Serial.println("System Ready! Type 'help' to see commands.");
}

// Loop Function
void loop() {
    // Shell Input
    if (Serial.available() > 0) {
        String command = Serial.readStringUntil('\n');
        command.trim();
        handleCommand(command);
    }
}
