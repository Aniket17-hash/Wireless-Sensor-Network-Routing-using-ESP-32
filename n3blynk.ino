#define BLYNK_TEMPLATE_ID "TMPL3riJ4zKXV"
#define BLYNK_TEMPLATE_NAME "ESP32 WWSNR"
#define BLYNK_AUTH_TOKEN "hU2acY7AubhWnxMVwYkYTZK12KNmgeNa"

#include <esp_now.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

// Wi-Fi Credentials
#define WIFI_SSID "Guten morgen"
#define WIFI_PASS "mjmn1178"

// Coordinator MAC Address
uint8_t coordinatorMac[] = {0xF4, 0x65, 0x0B, 0x48, 0xEF, 0x54};

typedef struct struct_message {
  uint8_t senderMac[6];
  uint8_t destMac[6];
  char message[50];
  float temperature;
  int hopCount;
} struct_message;

struct_message myData;

unsigned long lastSend = 0;
bool isDown = false;

void OnDataRecv(const esp_now_recv_info_t *recvInfo, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.printf("Received from: %02X:%02X:%02X:%02X:%02X:%02X\n", myData.senderMac[0], myData.senderMac[1], myData.senderMac[2], myData.senderMac[3], myData.senderMac[4], myData.senderMac[5]);
  Serial.printf("Message: %s, Hop: %d\n", myData.message, myData.hopCount);

  if (strcmp(myData.message, "PING") == 0) {
    Serial.println("Received PING, sending PONG");
    struct_message pong = {0};
    getMacAddress(pong.senderMac);
    memcpy(pong.destMac, coordinatorMac, 6);
    strcpy(pong.message, "PONG");
    pong.hopCount = 0;
    esp_now_send(coordinatorMac, (uint8_t *) &pong, sizeof(pong));
  }
}

void setup() {
  Serial.begin(115200);

  Blynk.begin(BLYNK_AUTH_TOKEN, WIFI_SSID, WIFI_PASS);

  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_recv_cb(OnDataRecv);

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, coordinatorMac, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  esp_now_add_peer(&peerInfo);
}

void loop() {
  Blynk.run();

  if (millis() - lastSend > 10000 && !isDown) {
    struct_message tempData = {0};
    getMacAddress(tempData.senderMac);
    memcpy(tempData.destMac, coordinatorMac, 6);
    strcpy(tempData.message, "Temp from Node 3");
    tempData.temperature = random(37.7, 38.5) + (random(0, 100) / 100.0);
    tempData.hopCount = 0;
    esp_now_send(coordinatorMac, (uint8_t *) &tempData, sizeof(tempData));
    Serial.printf("Sent Temp: %.2f°C to Coordinator\n", tempData.temperature);

    if (random(0, 100) < 10) {
      isDown = true;
      struct_message downData = {0};
      getMacAddress(downData.senderMac);
      memcpy(downData.destMac, coordinatorMac, 6);
      strcpy(downData.message, "NODE_DOWN");
      downData.hopCount = 0;
      esp_now_send(coordinatorMac, (uint8_t *) &downData, sizeof(downData));
      Serial.println("Sent NODE_DOWN");
    }
    lastSend = millis();
  } else if (isDown && random(0, 100) < 20) {
    isDown = false;
    Serial.println("Node recovered");
  }

  delay(1000);
}

void getMacAddress(uint8_t *mac) {
  String macStr = WiFi.macAddress();
  sscanf(macStr.c_str(), "%x:%x:%x:%x:%x:%x", &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]);
}