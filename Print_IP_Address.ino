#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>

SoftwareSerial s(D9, D10);

const char* ssid = "GUSTO University";
const char* password = "weloveGUSTO";
WiFiClientSecure client;
HTTPClient sender;

const size_t capacity = 5 * JSON_ARRAY_SIZE(7) + 8 * JSON_OBJECT_SIZE(8) + 5000;
DynamicJsonDocument doc(capacity);

void setup() {
  Serial.begin(9600); // serial monitoring

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
  }

  client.setInsecure();
  client.connect("iotweb-c6665-default-rtdb.asia-southeast1.firebasedatabase.app/cardInfo.json", 443);
}

void loop() {
  if (sender.begin(client, "https://iotweb-c6665-default-rtdb.asia-southeast1.firebasedatabase.app/cardInfo.json")) {
    int httpCode = sender.GET();

    if (httpCode > 0) {
      if (httpCode == HTTP_CODE_OK) {
        String payload = sender.getString();
        deserializeJson(doc, payload);

        // Access each element in the array and concatenate the 'cardID' and 'isRemotelyOpened' fields
        String cardIDs[7];

        for (int i = 0; i < 7; i++) {
          if(doc[i]["isActivated"] == true) {
            cardIDs[i] = doc[i]["cardID"].as<String>() + ":" + String(doc[i]["isRemotelyOpened"].as<bool>() ? "true" : "false");
          }
        }

        String allCardIDs = "";
        for (int i = 0; i < 7; i++) {
          allCardIDs += cardIDs[i] + ",";
        }

        // Print the entire array in a single line
        Serial.println(allCardIDs);

      } else {
        Serial.printf("HTTP-Error: %s\n", sender.errorToString(httpCode).c_str());
      }
    }
  } else {
    Serial.println("Other HTTP error");
    sender.end();
  }

}
