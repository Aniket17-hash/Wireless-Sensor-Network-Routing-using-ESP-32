#define BLYNK_TEMPLATE_ID "TMPL3riJ4zKXV"
#define BLYNK_TEMPLATE_NAME "ESP32 WWSNR"
#define BLYNK_AUTH_TOKEN "hU2acY7AubhWnxMVwYkYTZK12KNmgeNa"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <esp_now.h>

char auth[] = "hU2acY7AubhWnxMVwYkYTZK12KNmgeNa";     // Blynk Auth Token
char ssid[] = "Guten morgen";      // WiFi SSID
char pass[] = "mjmn1178";  // WiFi Password

#define NUM_NODES 4
int graph[NUM_NODES][NUM_NODES] = {
  {0, 1, 1, 1},
  {1, 0, 1, 1},
  {1, 1, 0, 1},
  {1, 1, 1, 0}
};

int dist[NUM_NODES];
bool visited[NUM_NODES];
int parent[NUM_NODES];

int currentNodeIndex = 0; // Coordinator is node 0

WidgetTerminal terminal(V20);  // Terminal widget on Virtual Pin V20

// Structure for ESP-NOW messages
typedef struct struct_message {
  uint8_t senderMac[6];
  uint8_t destMac[6];
  char message[50];
  float temperature;
  int hopCount;
} struct_message;

struct_message incomingData; // Global variable to store received data

void OnDataRecv(const esp_now_recv_info_t *recvInfo, const uint8_t *incomingDataBuf, int len) {
  // Copy the received buffer into the struct_message object
  memcpy(&incomingData, incomingDataBuf, sizeof(struct_message));
  const uint8_t *mac = recvInfo->src_addr;
  int nodeIdx = getNodeIndex(mac);

  // Debug output for troubleshooting
  Serial.printf("Received from: %02X:%02X:%02X:%02X:%02X:%02X\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  Serial.printf("Message: '%s', Temp: %.2f°C, Hop: %d, NodeIdx: %d\n", incomingData.message, incomingData.temperature, incomingData.hopCount, nodeIdx);

  if (nodeIdx != -1) {
    // Handle temperature messages with exact node matching
    if (strcmp(incomingData.message, "Temp from Node 1") == 0) {
      Blynk.virtualWrite(V0, incomingData.temperature); // Send Node 1 temp to V0
      Serial.printf("Sent Node 1 Temp %.2f°C to V0\n", incomingData.temperature);
      for (int j = 0; j < NUM_NODES; j++) {
        graph[1][j] = 1;
        graph[j][1] = 1;
      }
    } else if (strcmp(incomingData.message, "Temp from Node 2") == 0) {
      Blynk.virtualWrite(V1, incomingData.temperature); // Send Node 2 temp to V1
      Serial.printf("Sent Node 2 Temp %.2f°C to V1\n", incomingData.temperature);
      for (int j = 0; j < NUM_NODES; j++) {
        graph[2][j] = 1;
        graph[j][2] = 1;
      }
    } else if (strcmp(incomingData.message, "Temp from Node 3") == 0) {
      Blynk.virtualWrite(V4, incomingData.temperature); // Send Node 3 temp to V4
      Serial.printf("Sent Node 3 Temp %.2f°C to V4\n", incomingData.temperature);
      for (int j = 0; j < NUM_NODES; j++) {
        graph[3][j] = 1;
        graph[j][3] = 1;
      }
    } else if (strcmp(incomingData.message, "NODE_DOWN") == 0) {
      Serial.println("Node marked down, recomputing Dijkstra...");
      for (int j = 0; j < NUM_NODES; j++) {
        graph[nodeIdx][j] = 0;
        graph[j][nodeIdx] = 0;
      }
    } else if (strcmp(incomingData.message, "PONG") == 0) {
      Serial.println("PONG received, node is active");
      for (int j = 0; j < NUM_NODES; j++) {
        graph[nodeIdx][j] = 1;
        graph[j][nodeIdx] = 1;
      }
    }
    dijkstra(currentNodeIndex);
    printRoutingTable();
  }
}

int getNodeIndex(const uint8_t *mac) {
  uint8_t node1Mac[] = {0x94, 0x54, 0xC5, 0xED, 0xCA, 0xEC}; // Node 1
  uint8_t node2Mac[] = {0xF4, 0x65, 0x0B, 0x4A, 0xBF, 0x90}; // Node 2
  uint8_t node3Mac[] = {0x3C, 0x8A, 0x1F, 0x5E, 0xD7, 0xF0}; // Node 3
  if (memcmp(mac, node1Mac, 6) == 0) return 1;
  if (memcmp(mac, node2Mac, 6) == 0) return 2;
  if (memcmp(mac, node3Mac, 6) == 0) return 3;
  return -1;
}

void setup() {
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass);

  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_recv_cb(OnDataRecv);

  esp_now_peer_info_t peerInfo = {};
  uint8_t node1Mac[] = {0x94, 0x54, 0xC5, 0xED, 0xCA, 0xEC};
  uint8_t node2Mac[] = {0xF4, 0x65, 0x0B, 0x4A, 0xBF, 0x90};
  uint8_t node3Mac[] = {0x3C, 0x8A, 0x1F, 0x5E, 0xD7, 0xF0};
  memcpy(peerInfo.peer_addr, node1Mac, 6); peerInfo.channel = 0; peerInfo.encrypt = false; esp_now_add_peer(&peerInfo);
  memcpy(peerInfo.peer_addr, node2Mac, 6); esp_now_add_peer(&peerInfo);
  memcpy(peerInfo.peer_addr, node3Mac, 6); esp_now_add_peer(&peerInfo);

  dijkstra(currentNodeIndex);
  printRoutingTable();
}

void loop() {
  Blynk.run();

  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate > 10000) {
    dijkstra(currentNodeIndex);
    printRoutingTable();
    lastUpdate = millis();
    Serial.println("Routing table updated");
  }
}

int minDistance() {
  int min = INT_MAX, minIndex;

  for (int v = 0; v < NUM_NODES; v++) {
    if (!visited[v] && dist[v] <= min) {
      min = dist[v];
      minIndex = v;
    }
  }

  Serial.print("minDistance selected node ");
  Serial.print(minIndex);
  Serial.print(" with dist ");
  Serial.println(min);

  return minIndex;
}

void dijkstra(int src) {
  for (int i = 0; i < NUM_NODES; i++) {
    dist[i] = INT_MAX;
    visited[i] = false;
    parent[i] = -1;
  }

  dist[src] = 0;

  for (int count = 0; count < NUM_NODES - 1; count++) {
    int u = minDistance();
    visited[u] = true;

    Serial.print("Processing node ");
    Serial.print(u);
    Serial.print(", dist[");
    Serial.print(u);
    Serial.print("] = ");
    Serial.println(dist[u]);

    for (int v = 0; v < NUM_NODES; v++) {
      if (!visited[v] && graph[u][v] && dist[u] != INT_MAX && dist[u] + graph[u][v] < dist[v]) {
        dist[v] = dist[u] + graph[u][v];
        parent[v] = u;
      }
    }
  }
}

void printRoutingTable() {
  Serial.println("Routing Table:");
  terminal.clear();
  terminal.println("Routing Table:");

  for (int i = 0; i < NUM_NODES; i++) {
    Serial.print("Node ");
    Serial.print(i);
    Serial.print(" -> Distance: ");
    Serial.print(dist[i]);
    Serial.print(", Next Hop: ");
    Serial.println(parent[i]);

    terminal.print("Node ");
    terminal.print(i);
    terminal.print(" -> Distance: ");
    terminal.print(dist[i]);
    terminal.print(", Next Hop: ");
    terminal.println(parent[i]);
  }

  terminal.flush();
}