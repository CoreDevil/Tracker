#include <WiFi.h>
#include <HTTPClient.h>

// WiFi credentials
const char* ssid = "Uma-2.4G";
const char* password = "Siva@infy21";

// PubNub keys
String pubKey = "pub-c-daf4b9aa-8030-4420-97e0-e03abc8dac99";
String subKey = "sub-c-874e74cd-ed21-4746-b075-50ff52c124a7";
String channel = "bus_tracker";

// Dummy route: Tambaram → Sholinganallur
float route[][2] = {
  {12.9229, 80.1275},
  {12.9255, 80.1352},
  {12.9318, 80.1490},
  {12.9392, 80.1578},
  {12.9476, 80.1685},
  {12.9528, 80.1794},
  {12.9602, 80.1880},
  {12.9640, 80.1960},
  {12.9678, 80.2065},
  {12.9715, 80.2200}
};

int totalPoints = sizeof(route) / sizeof(route[0]);
int currentPoint = 0;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ Connected to WiFi!");
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    float lat = route[currentPoint][0];
    float lon = route[currentPoint][1];
    sendToPubNub(lat, lon);

    Serial.print("Sent point ");
    Serial.print(currentPoint + 1);
    Serial.print("/");
    Serial.println(totalPoints);

    // Move to next point
    currentPoint++;
    if (currentPoint >= totalPoints) currentPoint = 0; // Loop route
  } else {
    Serial.println("⚠️ WiFi disconnected, reconnecting...");
    WiFi.reconnect();
  }

  delay(5000); // 5 seconds between updates
}

void sendToPubNub(float lat, float lon) {
  if (WiFi.status() != WL_CONNECTED) return;

  HTTPClient http;

  // PubNub publish endpoint
  String url = "https://ps.pndsn.com/publish/" + String(pubKey) + "/" + String(subKey) + "/0/" + String(channel) + "/0";

  // Create proper JSON message body
  String payload = "{\"lat\":" + String(lat, 6) + ",\"lon\":" + String(lon, 6) + "}";

  Serial.println("📤 Sending to PubNub...");
  Serial.println("URL: " + url);
  Serial.println("Payload: " + payload);

  http.begin(url);
  http.addHeader("Content-Type", "application/json");

  int httpCode = http.POST(payload);

  if (httpCode > 0) {
    Serial.printf("✅ HTTP Response code: %d\n", httpCode);
    String response = http.getString();
    Serial.println("Response: " + response);
  } else {
    Serial.printf("❌ HTTP Error: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();
}
